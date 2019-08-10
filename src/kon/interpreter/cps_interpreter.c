#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include "cps_interpreter.h"
#include "keyword/prefix.h"

bool IsSelfEvaluated(KN source)
{
    if (KON_IS_FIXNUM(source)
        || KON_IS_FLONUM(source)
        || KON_IS_STRING(source)
        || KON_IS_SYNTAX_MARKER(source)
        // /abc
        || KON_IS_QUERY_PATH(source)
        // .append
        || (KON_IS_SYMBOL(source) && CAST_Kon(Symbol, source)->Type == KON_MSG_SIGNAL)
        // $abc
        || (KON_IS_IDENTIFIER(source))
        // $'abc'
        || (KON_IS_SYMBOL(source) && CAST_Kon(Symbol, source)->Type == KON_SYM_STRING)
        || KON_IS_QUOTE(source)
        || source == KON_TRUE
        || source == KON_FALSE
        || source == KON_NIL
        || source == KON_NULL
    ) {
        return true;
    }
    else {
        return false;
    }
}

KonTrampoline* ApplySubjVerbAndObjects(KonState* kstate, KN subj, KN argList, KonEnv* env, KonContinuation* cont)
{
    KN subjFmtStr = KON_ToFormatString(kstate, subj, false, 0, " ");
    KN objectsFmtStr = KON_ToFormatString(kstate, argList, false, 0, " ");

    KON_DEBUG("subj: %s, objects: %s", KON_StringToCstr(subjFmtStr), KON_StringToCstr(objectsFmtStr));
    KN firstObj = KON_CAR(argList);

    KonTrampoline* bounce;
    if (KON_IS_SYNTAX_MARKER(firstObj)) {
        // apply args to a procedure like % 1 2;
        if (CAST_Kon(SyntaxMarker, firstObj)->Type == KON_SYNTAX_MARKER_APPLY) {
            // call-cc, subject is a continuation
            if (KON_IS_CONTINUATION(subj)) {
                // call-cc's continuation, just receive 1 argument;
                bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
                bounce->Run.Value = KON_CADR(argList);
                // goto this continuation directly. skip next exprs
                bounce->Run.Cont = subj;
                return bounce;
            }

            // unbox attr slot
            if (KON_IS_ATTR_SLOT(subj)) {
                subj = ((KonAttrSlot*)subj)->Value;
            }
            KonProcedure* subjProc = (KonProcedure*)subj;
            argList = KON_CDR(argList);
            // TODO assert subj is a procedure
            if (subjProc->Type == KON_NATIVE_FUNC) {
                KonNativeFuncRef funcRef = subjProc->NativeFuncRef;
                KN applyResult = (*funcRef)(kstate, argList);
                bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
                bounce->Run.Value = applyResult;
                bounce->Run.Cont = cont;
            }
            else if (subjProc->Type == KON_NATIVE_OBJ_METHOD) {
                // treat as plain procedure when apply arg list
                // the first item in arg list is the object
                KonNativeFuncRef funcRef = subjProc->NativeFuncRef;
                KN applyResult = (*funcRef)(kstate, argList);
                bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
                bounce->Run.Value = applyResult;
                bounce->Run.Cont = cont;
            }
            else if (subjProc->Type == KON_COMPOSITE_LAMBDA) {
                bounce = KON_ApplyCompositeLambda(kstate, subjProc, argList, env, cont);
            }
            else if (subjProc->Type == KON_COMPOSITE_FUNC) {
                bounce = KON_ApplyCompositeFunc(kstate, subjProc, argList, env, cont);
            }
            else if (subjProc->Type == KON_COMPOSITE_BLK) {
                bounce = KON_ApplyCompositeBlk(kstate, subjProc, KON_NIL, env, cont);
            }
            else if (subjProc->Type == KON_COMPOSITE_OBJ_METHOD) {
                // treat as plain procedure when apply arg list
                // the first item in arg list is the object
                bounce = KON_ApplyCompositeLambda(kstate, subjProc, argList, env, cont);
            }
        }
        // pipe args to a procedure like | add 1 2;
        else if (CAST_Kon(SyntaxMarker, firstObj)->Type == KON_SYNTAX_MARKER_PROC_PIPE) {

            // unbox attr slot
            KN pipeArg = KON_CADR(argList);
            if (KON_IS_ATTR_SLOT(pipeArg)) {
                pipeArg = ((KonAttrSlot*)pipeArg)->Value;
            }
            if (KON_IS_ATTR_SLOT(subj)) {
                subj = ((KonAttrSlot*)subj)->Value;
            }
            
            KonProcedure* pipeProc = (KonProcedure*)pipeArg;
            // prepend the subject to arg list
            argList = KON_CONS(kstate, subj, KON_CDDR(argList));
            // TODO assert subj is a procedure
            if (pipeProc->Type == KON_NATIVE_FUNC) {
                KonNativeFuncRef funcRef = pipeProc->NativeFuncRef;
                KN applyResult = (*funcRef)(kstate, argList);
                bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
                bounce->Run.Value = applyResult;
                bounce->Run.Cont = cont;
            }
            else if (pipeProc->Type == KON_NATIVE_OBJ_METHOD) {
                // treat as plain procedure when apply arg list
                // the first item in arg list is the object
                KonNativeFuncRef funcRef = pipeProc->NativeFuncRef;
                KN applyResult = (*funcRef)(kstate, argList);
                bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
                bounce->Run.Value = applyResult;
                bounce->Run.Cont = cont;
            }
            else if (pipeProc->Type == KON_COMPOSITE_LAMBDA) {
                bounce = KON_ApplyCompositeLambda(kstate, pipeProc, argList, env, cont);
            }
            else if (pipeProc->Type == KON_COMPOSITE_FUNC) {
                bounce = KON_ApplyCompositeFunc(kstate, pipeProc, argList, env, cont);
            }
            else if (pipeProc->Type == KON_COMPOSITE_OBJ_METHOD) {
                // treat as plain procedure when apply arg list
                // the first item in arg list is the object
                bounce = KON_ApplyCompositeLambda(kstate, pipeProc, argList, env, cont);
            }
        }
    }


    //  get attr value like /abc
    else if (KON_IS_ATTR_SLOT(subj) && KON_IS_QUERY_PATH(firstObj)) {
        KonAttrSlot* slot = (KonAttrSlot*)subj;
        KN slotValue = KxHashTable_AtKey(slot->Folder, KON_UNBOX_SYMBOL(firstObj));
        KON_DEBUG("get slotValue %s", KON_UNBOX_SYMBOL(firstObj));
        bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
        bounce->Run.Value = slotValue;
        bounce->Run.Cont = cont;
    }

    // send msg like .add 1 2;
    else if (KON_IS_SYMBOL(firstObj) && ((KonSymbol*)firstObj)->Type == KON_MSG_SIGNAL) {
        // get dispatcher and eval OnMethodCall
        // this dispatcherId is unboxed
        KN dispatcherId = ((KonBase*)subj)->MsgDispatcherId;
        char dispatcherIdStr[64] = { '\0' };
        itoa(dispatcherId, dispatcherIdStr, 10);
        KonMsgDispatcher* dispatcher = KON_EnvDispatcherLookup(kstate, env, dispatcherIdStr);

        KonProcedure* procedure = dispatcher->OnMethodCall;
        // dispatcher functions should receive 3 arg
        // 1st is the object
        // 2nd is the message symbol
        // 3rd is the argument list
        KN dispatchArgList = KON_NIL;
        dispatchArgList = KON_CONS(kstate, KON_CDR(argList), dispatchArgList);
        dispatchArgList = KON_CONS(kstate, firstObj, dispatchArgList);
        dispatchArgList = KON_CONS(kstate, subj, dispatchArgList);

        // call method
        if (procedure->Type == KON_NATIVE_FUNC) {
            KonNativeFuncRef funcRef = procedure->NativeFuncRef;
            KN applyResult = (*funcRef)(kstate, dispatchArgList);
            bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
            bounce->Run.Value = applyResult;
            bounce->Run.Cont = cont;
        }
        else if (procedure->Type == KON_COMPOSITE_LAMBDA) {
            bounce = KON_ApplyCompositeLambda(kstate, procedure, dispatchArgList, env, cont);
        }
        else if (procedure->Type == KON_COMPOSITE_FUNC) {
            bounce = KON_ApplyCompositeFunc(kstate, procedure, dispatchArgList, env, cont);
        }
        else if (procedure->Type == KON_COMPOSITE_OBJ_METHOD) {
            // treat as plain procedure when apply arg list
            // the first item in arg list is the object
            bounce = KON_ApplyCompositeLambda(kstate, procedure, dispatchArgList, env, cont);
        }

    }

    return bounce;
}




////
// @param sentenceRestWords sentence words except subject,
//        if a sentence is {"zhangsan " name | upcase}, then this
//        param should be {name | upcase}
KN SplitClauses(KonState* kstate, KN sentenceRestWords)
{
    // TODO parse symbol type verb
    
    
    KxVector* clauseListVec = KxVector_Init();
    
    KxVector* clauseVec = KxVector_Init();

    KonPair* iter = sentenceRestWords;
    
    int state = 1; // 1 need verb, 2 need objects
    do {
        KN item = KON_CAR(iter);
        
        if (state == 1) {
            if (KON_IS_SYNTAX_MARKER(item)
                && CAST_Kon(SyntaxMarker, item)->Type != KON_SYNTAX_MARKER_CLAUSE_END
            ) {
                // meet %
                KxVector_Push(clauseVec, item);
                state = 2;
            }
            // meet slot like /abc
            else if (KON_IS_QUERY_PATH(item)) {
                KxVector* slotClause = KxVector_Init();
                KxVector_Push(slotClause, item);
                KxVector_Push(clauseListVec, slotClause);
            }
            else if (KON_IS_VECTOR(item)
                || KON_IsPairList(item)
                || KON_IS_CELL(item)
                || KON_IS_SYMBOL(item)
                || KON_IS_QUOTE(item)
                || KON_IS_QUASIQUOTE(item)
                || KON_IS_UNQUOTE(item)
            ) {
                KxVector_Push(clauseVec, item);
            }
            
            else {
                // TODO throw exception
            }
        }
        else {
            // meet ;
            if (KON_IS_SYNTAX_MARKER(item)
                && CAST_Kon(SyntaxMarker, item)->Type == KON_SYNTAX_MARKER_CLAUSE_END
            ) {
                KxVector_Push(clauseListVec, clauseVec);
                // reset state
                clauseVec = KxVector_Init();
                state = 1;
            }
            else {
                KxVector_Push(clauseVec, item);
            }
        }
        
        
        iter = KON_CDR(iter);
    } while (iter != KON_NIL);
    
    if (KxVector_Length(clauseVec) > 0) {
        KxVector_Push(clauseListVec, clauseVec);
    }

    KN result = KON_NIL;
    int clauseListVecLen = KxVector_Length(clauseListVec);
    for (int i = clauseListVecLen - 1; i >= 0; i--) {
        KxVector* clauseWords = KxVector_AtIndex(clauseListVec, i);
        KN clause = KON_VectorToKonPairList(kstate, clauseWords);
        result = KON_CONS(kstate, clause, result);
    }

    return result;
}

KonContinuation* AllocContinuationWithType(KonContinuationType type)
{
    KonContinuation* cont = (KonContinuation*)malloc(sizeof(KonContinuation));
    assert(cont);
    cont->Base.Tag = KON_T_CONTINUATION;
    cont->Type = type;
    return cont;
}

KonTrampoline* AllocBounceWithType(KonBounceType type)
{
    KonTrampoline* bounce = (KonTrampoline*)malloc(sizeof(KonTrampoline));
    assert(bounce);
    bounce->Type = type;
    return bounce;
}

// pop the top continuation
// the bounce continuation should be cont->Cont
KonTrampoline* KON_RunContinuation(KonState* kstate, KonContinuation* contBeingInvoked, KN val)
{
    // all sentences finished, return last value
    if (kon_continuation_type(contBeingInvoked) == KON_CONT_RETURN) {
        KonTrampoline* bounce = AllocBounceWithType(KON_TRAMPOLINE_LAND);
        bounce->Land.Value = val;
        return bounce;
    }
    else if (kon_continuation_type(contBeingInvoked) == KON_CONT_NATIVE_CALLBACK) {
        KonContFuncRef callbackFunc = contBeingInvoked->Native.Callback;
        KonTrampoline* bounce = callbackFunc(kstate, val, contBeingInvoked);
        return bounce;
    }
    else if (kon_continuation_type(contBeingInvoked) == KON_CONT_EVAL_SENTENCE_LIST) {
        KN lastSentenceVal = val;
        KN env = contBeingInvoked->Env;
        KN restSentences = contBeingInvoked->RestJobs;
        if (restSentences == KON_NIL) {
            // block sentences all finished
            KonTrampoline* bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
            bounce->Run.Cont = contBeingInvoked->Cont;
            bounce->Run.Value = lastSentenceVal;
            return bounce;
        }
        else {
            return KON_EvalSentences(kstate, restSentences, env, contBeingInvoked->Cont);
        }
    }
    else if (kon_continuation_type(contBeingInvoked) == KON_CONT_EVAL_SUBJ) {
        // subj evaled, now should eval clauses
        KN subj = val;
        KN restWords = contBeingInvoked->RestJobs;

        if (restWords == KON_NIL) {
            // no other words besids subj, is a sentence like {"abc"}
            // finish this sentence. use subj as return val
            KonTrampoline* bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
            bounce->Run.Cont = contBeingInvoked->Cont;
            bounce->Run.Value = subj;
            return bounce;
        }
        else {
            // split sub clauses
            // TODO split % xxx; .xxx a a ; | xx aaa; sss
            // as {{% xxx} {. xxx a a} {| xx aaa} {sss}}
            KN clauses = SplitClauses(kstate, restWords);
            KON_DEBUG("splited clauses %s", KON_StringToCstr(KON_Stringify(kstate, clauses)));
            
            if (clauses == KON_NIL) {
                // no clauses like {{a}}
                KonTrampoline* bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
                bounce->Run.Cont = contBeingInvoked->Cont;
                bounce->Run.Value = subj;
                return bounce;
            }

            // eval the first clause
            KonContinuation* k = AllocContinuationWithType(KON_CONT_EVAL_CLAUSE_LIST);
            k->Cont = contBeingInvoked->Cont;
            k->Env = contBeingInvoked->Env;
            k->RestJobs = KON_CDR(clauses);
            
            KonTrampoline* bounce = AllocBounceWithType(KON_TRAMPOLINE_CLAUSE_LIST);
            bounce->SubjBounce.Subj = subj;
            bounce->SubjBounce.Cont = k;
            bounce->SubjBounce.Env = contBeingInvoked->Env;
            bounce->SubjBounce.Value = KON_CAR(clauses);
            return bounce;
        }
        
    }
    else if (kon_continuation_type(contBeingInvoked) == KON_CONT_EVAL_CLAUSE_LIST) {
        // last clause eval finshed, eval next clause
        // last clause eval result is the subj of the next clause
        KN subj = val;
        KN restClauseList = contBeingInvoked->RestJobs;
        if (restClauseList == KON_NIL) {
            // no other clauses, is a sentence like {writeln % "abc"}
            // finish this sentence. use last clause eval result as return val
            KonTrampoline* bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
            bounce->Run.Cont = contBeingInvoked->Cont;
            bounce->Run.Value = subj;
            return bounce;
        }
        else {
            // eval the next clause
            KonContinuation* k = AllocContinuationWithType(KON_CONT_EVAL_CLAUSE_LIST);
            k->Cont = contBeingInvoked->Cont;
            k->Env = contBeingInvoked->Env;
            k->RestJobs = KON_CDR(restClauseList);
            
            KonTrampoline* bounce = AllocBounceWithType(KON_TRAMPOLINE_CLAUSE_LIST);
            bounce->SubjBounce.Subj = subj;
            bounce->SubjBounce.Cont = k;
            bounce->SubjBounce.Env = contBeingInvoked->Env;
            bounce->SubjBounce.Value = KON_CAR(restClauseList);
            return bounce;
        }
    }
    else if (kon_continuation_type(contBeingInvoked) == KON_CONT_EVAL_CLAUSE_ARGS) {
        KN lastArgEvaled = val;
        KN subj = contBeingInvoked->EvalClauseArgs.Subj;
        KN restArgList = contBeingInvoked->EvalClauseArgs.RestArgList;
        KN evaledArgList = contBeingInvoked->EvalClauseArgs.EvaledArgList;
        // NOTE! the evaluated arg list here is reverted saved
        // should reverted back when apply the arguments
        evaledArgList = KON_CONS(kstate, lastArgEvaled, evaledArgList);

        if (restArgList == KON_NIL) {
            // this clause args all eval finished
            KN argList = KON_PairListRevert(kstate, evaledArgList);
            KON_DEBUG("before ApplySubjVerbAndObjects");
            // next continuation should be KON_CONT_EVAL_CLAUSE_LIST
            return ApplySubjVerbAndObjects(kstate, subj, argList, contBeingInvoked->Env, contBeingInvoked->Cont);
        }
        else {
            // eval the next clause
            KonContinuation* k = AllocContinuationWithType(KON_CONT_EVAL_CLAUSE_ARGS);
            k->Cont = contBeingInvoked->Cont;
            k->Env = contBeingInvoked->Env;
            k->EvalClauseArgs.Subj = subj;
            k->EvalClauseArgs.RestArgList = KON_CDR(restArgList);
            k->EvalClauseArgs.EvaledArgList = evaledArgList;
            
            KonTrampoline* bounce = AllocBounceWithType(KON_TRAMPOLINE_ARG_LIST);
//            bounce->SubjBounce.Subj = subj;
            bounce->Bounce.Cont = k;
            bounce->Bounce.Env = contBeingInvoked->Env;
            bounce->Bounce.Value = KON_CAR(restArgList);
            return bounce;
        }
    }

    else {
        // TODO throw error
    }
}

bool KON_IsPrefixMarcro(KN word) {
    if (!(KON_CHECK_TAG(word, KON_T_SYMBOL))) {
        return false;
    }
    int type = ((KonSymbol*)word)->Type;
    if (type != KON_SYM_PREFIX_WORD && type != KON_SYM_WORD) {
        return false;
    }
    
    // check reserved keywords
    const char* prefix = KON_UNBOX_SYMBOL(word);

    if (strcmp(prefix, "and") == 0
        || strcmp(prefix, "apply") == 0
        || strcmp(prefix, "blk") == 0
        || strcmp(prefix, "break") == 0
        || strcmp(prefix, "call-cc") == 0
        || strcmp(prefix, "cond") == 0
        || strcmp(prefix, "continue") == 0
        || strcmp(prefix, "def-builder") == 0
        || strcmp(prefix, "def-dispatcher") == 0
        || strcmp(prefix, "do") == 0
        || strcmp(prefix, "eval") == 0
        || strcmp(prefix, "for") == 0
        || strcmp(prefix, "func") == 0
        || strcmp(prefix, "global") == 0
        || strcmp(prefix, "if") == 0
        || strcmp(prefix, "lambda") == 0
        || strcmp(prefix, "let") == 0
        || strcmp(prefix, "letrec") == 0
        || strcmp(prefix, "letstar") == 0
        || strcmp(prefix, "local") == 0
        || strcmp(prefix, "or") == 0
        || strcmp(prefix, "set") == 0
        || strcmp(prefix, "sh") == 0
    ) {
        return true;
    }
    else {
        return false;
    }
}

KonTrampoline* KON_EvalExpression(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KonTrampoline* bounce;
    if (IsSelfEvaluated(expression)) {
        bounce = KON_RunContinuation(kstate, cont, expression);
    }
    else if (KON_IsPairList(expression)) {
        // passed a sentence like {writeln % "abc" "efg"}
        KN words = expression;
        KN first = KON_CAR(words);
        if (KON_IsPrefixMarcro(first)) {
            const char* prefix = KON_UNBOX_SYMBOL(first);
            if (strcmp(prefix, "apply") == 0) {
                bounce = KON_EvalPrefixApply(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "if") == 0) {
                bounce = KON_EvalPrefixIf(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "let") == 0) {
                bounce = KON_EvalPrefixLet(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "set") == 0) {
                bounce = KON_EvalPrefixSet(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "lambda") == 0) {
                bounce = KON_EvalPrefixLambda(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "func") == 0) {
                bounce = KON_EvalPrefixFunc(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "blk") == 0) {
                bounce = KON_EvalPrefixBlk(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "do") == 0) {
                bounce = KON_EvalPrefixDo(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "and") == 0) {
                bounce = KON_EvalPrefixAnd(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "or") == 0) {
                bounce = KON_EvalPrefixOr(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "cond") == 0) {
                bounce = KON_EvalPrefixCond(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "call-cc") == 0) {
                bounce = KON_EvalPrefixCallcc(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "eval") == 0) {
                bounce = KON_EvalPrefixEval(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "for") == 0) {
                bounce = KON_EvalPrefixFor(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "break") == 0) {
                bounce = KON_EvalPrefixBreak(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "continue") == 0) {
                bounce = KON_EvalPrefixContinue(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "def-dispatcher") == 0) {
                bounce = KON_EvalPrefixDefDispatcher(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "def-builder") == 0) {
                bounce = KON_EvalPrefixDefBuilder(kstate, KON_CDR(words), env, cont);
            }
            else if (strcmp(prefix, "sh") == 0) {
                bounce = KON_EvalPrefixSh(kstate, KON_CDR(words), env, cont);
            }
            else {
                KON_DEBUG("error! unhandled prefix marcro %s", prefix);
                bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
                bounce->Run.Value = KON_NULL;
                bounce->Run.Cont = cont;
            }
        }
        // TODO quasiquote unquote, etc.
        else {
            bounce = AllocBounceWithType(KON_TRAMPOLINE_SUBJ);
            
            KonContinuation* k = AllocContinuationWithType(KON_CONT_EVAL_SUBJ);
            k->Cont = cont;
            k->Env = env;
            k->RestJobs = KON_CDR(words);
            
            bounce->Bounce.Value = first;  // get subj word
            bounce->Bounce.Cont = k;
            bounce->Bounce.Env = env;
        }
    }
    else if (KON_IS_VARIABLE(expression) || KON_IS_WORD(expression)) {
        // a code block like { a }
        // TODO asert should be a SYM_IDENTIFIER
        // env lookup this val
        KN val = KON_EnvLookup(kstate, env, KON_SymbolToCstr(expression));
        assert(val != KON_NULL);
        bounce = KON_RunContinuation(kstate, cont, val);
    }
    else {
        KON_DEBUG("unhandled expression type");
        exit(1);
    }
    return bounce;
}

KonTrampoline* KON_EvalSentences(KonState* kstate, KN sentences, KN env, KonContinuation* cont)
{
    if (sentences == KON_NIL) {
        // TODO throw error
        // should not go here
        return KON_NULL;
    }
    else {
        KonTrampoline* bounce = AllocBounceWithType(KON_TRAMPOLINE_BLOCK);
        
        KonContinuation* k = AllocContinuationWithType(KON_CONT_EVAL_SENTENCE_LIST);
        k->Cont = cont;
        k->Env = env;
        k->RestJobs = KON_CDR(sentences);
        
        bounce->Bounce.Value = KON_CAR(sentences);
        bounce->Bounce.Cont = k;
        bounce->Bounce.Env = env;
        return bounce;
    }
}



KN KON_ProcessSentences(KonState* kstate, KN sentences, KN rootEnv)
{
    // TODO add step count when debug
    KN formated = KON_ToFormatString(&kstate, sentences, true, 0, "  ");
    //  KN formated = KON_ToFormatString(&kstate, root, false, 0, " ");
    KON_DEBUG("%s", KON_StringToCstr(formated));
    
    KonContinuation* firstCont = AllocContinuationWithType(KON_CONT_RETURN);
    firstCont->Env = rootEnv;
    // set root level return
    KON_EnvDefine(kstate, rootEnv, "return", firstCont);
    KonTrampoline* bounce = KON_EvalSentences(kstate, sentences, rootEnv, firstCont);

    while (kon_bounce_type(bounce) != KON_TRAMPOLINE_LAND) {
        if (kon_bounce_type(bounce) == KON_TRAMPOLINE_RUN) {
            KonTrampoline* oldBounce = bounce;
            KN value = bounce->Run.Value;
            KonContinuation* cont = bounce->Run.Cont;
            free(oldBounce);
            bounce = KON_RunContinuation(kstate, cont, value);
        }
        else if (kon_bounce_type(bounce) == KON_TRAMPOLINE_BLOCK) {
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->Bounce.Cont;
            KN env = bounce->Bounce.Env;
            KN sentence = bounce->Bounce.Value;

            bounce = KON_EvalExpression(kstate, sentence, env, cont);
        }
        else if (kon_bounce_type(bounce) == KON_TRAMPOLINE_SUBJ) {
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->Bounce.Cont;
            KN env = bounce->Bounce.Env;
            KN subj = bounce->Bounce.Value;
            if (KON_IsPairList(subj)) {
                // the subj position is a list, should be evaluated
                // like the first sentence in this eg block {{ {zhangsan name} |upcase }}
                KN subjExpr = subj;
                bounce = KON_EvalExpression(kstate, subjExpr, env, cont);
            }
            else if (KON_IS_VARIABLE(subj) || KON_IS_WORD(subj)) {
                // lookup subject in env
                KN val = KON_EnvLookup(kstate, env, KON_SymbolToCstr(subj));
                assert(val != KON_NULL);
                bounce = KON_RunContinuation(kstate, cont, val);
            }
            // TODO quasiquote unquote, etc.
            
            else if (IsSelfEvaluated(subj)) {
                bounce = KON_RunContinuation(kstate, cont, subj);
            }
        }
        else if (kon_bounce_type(bounce) == KON_TRAMPOLINE_CLAUSE_LIST) {
            
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->SubjBounce.Cont;
            KN env = bounce->SubjBounce.Env;
            KN clauseArgList = bounce->SubjBounce.Value;
            KN subj = bounce->SubjBounce.Subj;
            KN firstArg = KON_CAR(clauseArgList);

            if (KON_IS_SYNTAX_MARKER(firstArg)) {
                // % . |
                // this kind bouce value is a clause word list like {% "a" "b"}

                KonContinuation* k = AllocContinuationWithType(KON_CONT_EVAL_CLAUSE_ARGS);
                k->Cont = cont;
                k->Env = env;
                k->EvalClauseArgs.Subj = subj;
                k->EvalClauseArgs.RestArgList = KON_CDR(clauseArgList);
                k->EvalClauseArgs.EvaledArgList = KON_NIL;

                bounce = AllocBounceWithType(KON_TRAMPOLINE_ARG_LIST);
                bounce->Bounce.Cont = k;
                bounce->Bounce.Env = env;
                bounce->Bounce.Value = KON_CAR(clauseArgList); // the first arg is % or . or |

            }
            else if (KON_IS_SYMBOL(firstArg)) {
                // a clause like {{kon /list}}
                // should dispatch this msg to the object's visitor protocol
                KonContinuation* k = AllocContinuationWithType(KON_CONT_EVAL_CLAUSE_ARGS);
                k->Cont = cont;
                k->Env = env;
                k->EvalClauseArgs.Subj = subj;
                k->EvalClauseArgs.RestArgList = KON_CDR(clauseArgList);
                k->EvalClauseArgs.EvaledArgList = KON_NIL;
                
                bounce = AllocBounceWithType(KON_TRAMPOLINE_ARG_LIST);
                bounce->Bounce.Cont = k;
                bounce->Bounce.Env = env;
                bounce->Bounce.Value = KON_CAR(clauseArgList); // the first arg is /abc
            }
            else if (KON_IS_SYMBOL(clauseArgList)) {
                // a clause like {length} in sentence {{"abc" length}}
                // should dispatch this msg to the object's visitor protocol
            }

        }

        else if (kon_bounce_type(bounce) == KON_TRAMPOLINE_ARG_LIST) {
            // eval each argment
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->Bounce.Cont;
            KN env = bounce->Bounce.Env;
            KN arg = bounce->Bounce.Value;
            if (KON_IsPairList(arg)) {
                // the subj position is a list, should be evaluated
                // like the first sentence in this eg block {{ + % 5 {+ % 1 2} }}
                KN argExpr = arg;
                bounce = KON_EvalExpression(kstate, argExpr, env, cont);
            }
            else if (KON_IS_VECTOR(arg)) {
                // TODO !!! verify cell inner content
                // whether have Quasiquote, Expand, Unquote, KON_SYM_VARIABLE node
                bounce = KON_RunContinuation(kstate, cont, arg);
            }
            else if (KON_IS_TABLE(arg)) {
                // TODO !!! verify cell inner content key, value
                // whether have Quasiquote, Expand, Unquote, KON_SYM_VARIABLE node
                bounce = KON_RunContinuation(kstate, cont, arg);
            }
            else if (KON_IS_CELL(arg)) {
                // TODO !!! verify cell inner content(tag, list, vector, table )
                // whether have Quasiquote, Expand, Unquote, KON_SYM_VARIABLE node
                bounce = KON_RunContinuation(kstate, cont, arg);
            }
            else if (KON_IS_QUOTE(arg)) {
                // treat as pure data, don't eval
                bounce = KON_RunContinuation(kstate, cont, arg);
            }
            else if (KON_IS_QUASIQUOTE(arg)) {
                // TODO eval the EXPAND, UNQUOTE nodes in ast
                bounce = KON_RunContinuation(kstate, cont, arg);
            }
            else if (KON_IS_EXPAND(arg)) {
                // TODO
                bounce = KON_RunContinuation(kstate, cont, arg);
            }
            else if (KON_IS_UNQUOTE(arg)) {
                // TODO
                bounce = KON_RunContinuation(kstate, cont, arg);
            }
            else if (KON_IS_WORD(arg) || KON_IS_VARIABLE(arg)) {
                // env lookup this val
                KN val = KON_EnvLookup(kstate, env, KON_SymbolToCstr(arg));;
                assert(val != NULL);
                bounce = KON_RunContinuation(kstate, cont, val);
            }
            else if (IsSelfEvaluated(arg)) {
                bounce = KON_RunContinuation(kstate, cont, arg);
            }
        }
    }
    return bounce->Land.Value;
}

