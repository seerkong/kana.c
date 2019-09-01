#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include "cps_interpreter.h"
#include "keyword/prefix.h"
#include "../gc.h"

bool IsSelfEvaluated(KN source)
{
    if (KN_IS_FIXNUM(source)
        || KN_IS_FLONUM(source)
        || KN_IS_STRING(source)
        || KN_IS_SYNTAX_MARKER(source)
        // $abc
        || (KN_IS_IDENTIFIER(source))
        // $'abc'
        || (KN_IS_SYMBOL(source) && CAST_Kon(Symbol, source)->Type == KN_SYM_STRING)
        || KN_IS_QUOTE(source)
        || KN_IS_VECTOR(source)
        || KN_IS_TABLE(source)
        || source == KN_TRUE
        || source == KN_FALSE
        || source == KN_NIL
        || source == KN_UKN
        || source == KN_UNDEF
    ) {
        return true;
    }
    else {
        return false;
    }
}

KonTrampoline* ApplyProcedureArguments(KonState* kstate, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* cont)
{
    KonTrampoline* bounce;
    // TODO assert if is a procedure
    if (proc->Type == KN_NATIVE_FUNC) {
        KonNativeFuncRef funcRef = proc->NativeFuncRef;
        KN applyResult = KN_ApplyArgsToNativeFunc(kstate, proc, argList);
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->Run.Value = applyResult;
        bounce->Cont = cont;
    }
    else if (proc->Type == KN_NATIVE_OBJ_METHOD) {
        // treat as plain procedure when apply arg list
        // the first item in arg list is the object
        KonNativeFuncRef funcRef = proc->NativeFuncRef;
        KN applyResult = KN_ApplyArgsToNativeFunc(kstate, proc, argList);
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->Run.Value = applyResult;
        bounce->Cont = cont;
    }
    else if (proc->Type == KN_COMPOSITE_LAMBDA) {
        bounce = KN_ApplyCompositeLambda(kstate, proc, argList, env, cont);
    }
    else if (proc->Type == KN_COMPOSITE_FUNC) {
        bounce = KN_ApplyCompositeFunc(kstate, proc, argList, env, cont);
    }

    // NOTE! the arguments are quoted before, unbox here
    else if (proc->Type == KN_COMPOSITE_MACRO_LAMBDA) {
        KN unboxed = KN_UNBOX_QUOTE(KN_CAR(argList));
        argList = KN_CONS(kstate, unboxed, KN_NIL);
        bounce = KN_ApplyCompositeLambda(kstate, proc, argList, env, cont);
    }
    else if (proc->Type == KN_COMPOSITE_MACRO_FUNC) {
        KN unboxed = KN_UNBOX_QUOTE(KN_CAR(argList));
        argList = KN_CONS(kstate, unboxed, KN_NIL);
        bounce = KN_ApplyCompositeFunc(kstate, proc, argList, env, cont);
    }

    else if (proc->Type == KN_COMPOSITE_BLK) {
        bounce = KN_ApplyCompositeBlk(kstate, proc, KN_NIL, env, cont);
    }
    else if (proc->Type == KN_COMPOSITE_OBJ_METHOD) {
        // treat as plain procedure when apply arg list
        // the first item in arg list is the object
        bounce = KN_ApplyCompositeLambda(kstate, proc, argList, env, cont);
    }
    else {
        // TODO throw exception
    }
    return bounce;
}

KonTrampoline* AssignValueToAccessor(KonState* kstate, KN accessorKon, KN assignTo, KonEnv* env, KonContinuation* cont)
{
    KonTrampoline* bounce;
    KonAccessor* accessor = CAST_Kon(Accessor, accessorKon);
    if (accessor->IsDir) {
        KxHashTable_Destroy(accessor->Dir);
    }
    
    if (accessor->Setter != NULL) {
        KN assignArgList = KN_NIL;
        assignArgList = KN_CONS(kstate, assignTo, assignArgList);
        assignArgList = KN_CONS(kstate, (KN)accessor, assignArgList);

        bounce = ApplyProcedureArguments(kstate, accessor->Setter, assignArgList, env, cont);
    }
    else {
        accessor->IsDir = false;
        accessor->Value = assignTo;
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->Run.Value = assignTo;
        bounce->Cont = cont;
    }
    return bounce;
}

KN UnBoxAccessorValue(KN konValue)
{
    if (!KN_IS_ACCESSOR(konValue)) {
        return konValue;
    }
    KonAccessor* accessor = CAST_Kon(Accessor, konValue);
    if (accessor->IsDir == true) {
        return (KN)accessor;
    }
    KN value = accessor->Value;
    while (KN_IS_ACCESSOR(value)
        && !CAST_Kon(Accessor, value)->IsDir
    ) {
        value = CAST_Kon(Accessor, value)->Value;
    }
    return value;
}

KonTrampoline* ApplySubjVerbAndObjects(KonState* kstate, KN subj, KN argList, KonEnv* env, KonContinuation* cont)
{
    KN subjFmtStr = KN_ToFormatString(kstate, subj, false, 0, " ");
    KN objectsFmtStr = KN_ToFormatString(kstate, argList, false, 0, " ");

    KN_DEBUG("subj: %s, objects: %s", KN_StringToCstr(subjFmtStr), KN_StringToCstr(objectsFmtStr));
    KN firstObj = KN_CAR(argList);

    KonTrampoline* bounce;
    if (KN_IS_SYNTAX_MARKER(firstObj)) {
        // apply args to a procedure like % 1 2;
        if (CAST_Kon(SyntaxMarker, firstObj)->Type == KN_SYNTAX_MARKER_APPLY) {
            // call-cc, subject is a continuation
            if (KN_IS_CONTINUATION(subj)) {
                // call-cc's continuation, just receive 1 argument;
                bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
                bounce->Run.Value = KN_CADR(argList);
                // goto this continuation directly. skip next exprs
                bounce->Cont = (KonContinuation*)subj;
                return bounce;
            }

            // unbox attr slot
            subj = UnBoxAccessorValue(subj);
            KonProcedure* subjProc = (KonProcedure*)subj;
            argList = KN_CDR(argList);
            bounce = ApplyProcedureArguments(kstate, subjProc, argList, env, cont);
        }
        else if (CAST_Kon(SyntaxMarker, firstObj)->Type == KN_SYNTAX_MARKER_ASSIGN) {
            KN assignTo = KN_CADR(argList);
            // subj must be a accessor
            if (KN_IS_ACCESSOR(subj)) {
                bounce = AssignValueToAccessor(kstate, subj, assignTo, env, cont);
            }
            else {
                bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
                bounce->Run.Value = assignTo;
                bounce->Cont = cont;
            }
            
        }
        // pipe args to a procedure like | add 1 2;
        else if (CAST_Kon(SyntaxMarker, firstObj)->Type == KN_SYNTAX_MARKER_PROC_PIPE) {

            // unbox attr slot
            KN pipeArg = KN_CADR(argList);
            pipeArg = UnBoxAccessorValue(pipeArg);
            subj = UnBoxAccessorValue(subj);

            KonProcedure* pipeProc = (KonProcedure*)pipeArg;
            // prepend the subject to arg list
            argList = KN_CONS(kstate, subj, KN_CDDR(argList));
            bounce = ApplyProcedureArguments(kstate, pipeProc, argList, env, cont);

        }

        // send msg like . add 1 2;
        else if (CAST_Kon(SyntaxMarker, firstObj)->Type == KN_SYNTAX_MARKER_MSG_SIGNAL) {
            KN signalSym = KN_CADR(argList);

            unsigned int dispatcherId = KN_NodeDispacherId(kstate, subj);
            KonMsgDispatcher* dispatcher = KN_GetMsgDispatcher(kstate, dispatcherId);

            KonProcedure* procedure = dispatcher->OnSymbol;

            // dispatcher functions should receive 3 arg
            // 1st is the object
            // 2nd is the symbol
            KN dispatchArgList = KN_NIL;
            dispatchArgList = KN_CONS(kstate, signalSym, dispatchArgList);
            dispatchArgList = KN_CONS(kstate, subj, dispatchArgList);

            // call method
            bounce = ApplyProcedureArguments(kstate, procedure, dispatchArgList, env, cont);

            
        }
        else if (CAST_Kon(SyntaxMarker, firstObj)->Type == KN_SYNTAX_MARKER_GET_SLOT) {
            KN signalSym = KN_CADR(argList);
            // argList = KN_CDDR(argList);
            KonAccessor* slot = (KonAccessor*)subj;
            KN slotValue = KxHashTable_AtKey(slot->Dir, KN_UNBOX_SYMBOL(signalSym));
            KN_DEBUG("get slotValue %s", KN_UNBOX_SYMBOL(signalSym));
            bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
            bounce->Run.Value = slotValue;
            bounce->Cont = cont;
        }
    }
    // send msg like {123 $+ 5}
    else if (KN_IS_SYMBOL(firstObj)) {
        KN signalSym = firstObj;

        argList = KN_CDR(argList);

        // get dispatcher and eval OnMethodCall
        // this dispatcherId is unboxed
        unsigned int dispatcherId = KN_NodeDispacherId(kstate, subj);
        KonMsgDispatcher* dispatcher = KN_GetMsgDispatcher(kstate, dispatcherId);

        KonProcedure* procedure = dispatcher->OnMethodCall;

        // dispatcher functions should receive 3 arg
        // 1st is the object
        // 2nd is the message symbol
        // 3rd is the argument list
        KN dispatchArgList = KN_NIL;
        dispatchArgList = KN_CONS(kstate, argList, dispatchArgList);
        dispatchArgList = KN_CONS(kstate, signalSym, dispatchArgList);
        dispatchArgList = KN_CONS(kstate, subj, dispatchArgList);

        bounce = ApplyProcedureArguments(kstate, procedure, dispatchArgList, env, cont);
    }
    else if (KN_IS_VECTOR(firstObj)) {
        // get dispatcher and eval OnVisitVector
        // this dispatcherId is unboxed
        unsigned int dispatcherId = KN_NodeDispacherId(kstate, subj);
        KonMsgDispatcher* dispatcher = KN_GetMsgDispatcher(kstate, dispatcherId);

        KonProcedure* procedure = dispatcher->OnVisitVector;
        KN dispatchArgList = KN_NIL;
        dispatchArgList = KN_CONS(kstate, firstObj, dispatchArgList);
        dispatchArgList = KN_CONS(kstate, subj, dispatchArgList);

        bounce = ApplyProcedureArguments(kstate, procedure, dispatchArgList, env, cont);
    }
    else if (KN_IS_TABLE(firstObj)) {
        // get dispatcher and eval OnVisitTable
        // this dispatcherId is unboxed
        unsigned int dispatcherId = KN_NodeDispacherId(kstate, subj);
        KonMsgDispatcher* dispatcher = KN_GetMsgDispatcher(kstate, dispatcherId);

        KonProcedure* procedure = dispatcher->OnVisitTable;
        KN dispatchArgList = KN_NIL;
        dispatchArgList = KN_CONS(kstate, firstObj, dispatchArgList);
        dispatchArgList = KN_CONS(kstate, subj, dispatchArgList);

        bounce = ApplyProcedureArguments(kstate, procedure, dispatchArgList, env, cont);
    }
    // send msg like {abc $[1]}
    else if (KN_IS_QUOTE_PAIR(firstObj)) {
        // get dispatcher and eval OnVisitList
        // this dispatcherId is unboxed
        unsigned int dispatcherId = KN_NodeDispacherId(kstate, subj);
        KonMsgDispatcher* dispatcher = KN_GetMsgDispatcher(kstate, dispatcherId);
        
        KonProcedure* procedure = dispatcher->OnVisitList;
        KN dispatchArgList = KN_NIL;
        dispatchArgList = KN_CONS(kstate, KN_UNBOX_QUOTE(firstObj), dispatchArgList);
        dispatchArgList = KN_CONS(kstate, subj, dispatchArgList);
        
        bounce = ApplyProcedureArguments(kstate, procedure, dispatchArgList, env, cont);
    }
    else if (KN_IS_QUOTE_CELL(firstObj)) {
        // get dispatcher and eval OnVisitCell
        // this dispatcherId is unboxed
        unsigned int dispatcherId = KN_NodeDispacherId(kstate, subj);
        KonMsgDispatcher* dispatcher = KN_GetMsgDispatcher(kstate, dispatcherId);

        KonProcedure* procedure = dispatcher->OnVisitCell;
        KN dispatchArgList = KN_NIL;
        dispatchArgList = KN_CONS(kstate, KN_UNBOX_QUOTE(firstObj), dispatchArgList);
        dispatchArgList = KN_CONS(kstate, subj, dispatchArgList);

        bounce = ApplyProcedureArguments(kstate, procedure, dispatchArgList, env, cont);
    }
    else {
        unsigned int dispatcherId = KN_NodeDispacherId(kstate, subj);
        KonMsgDispatcher* dispatcher = KN_GetMsgDispatcher(kstate, dispatcherId);

        KonProcedure* procedure = dispatcher->OnOtherType;
        KN dispatchArgList = KN_NIL;
        dispatchArgList = KN_CONS(kstate, firstObj, dispatchArgList);
        dispatchArgList = KN_CONS(kstate, subj, dispatchArgList);

        bounce = ApplyProcedureArguments(kstate, procedure, dispatchArgList, env, cont);
    }

    KN_DEBUG("EnterGcSafepoint after apply subj: %s, objects: %s", KN_StringToCstr(subjFmtStr), KN_StringToCstr(objectsFmtStr));
    KN_EnterGcSafepoint(kstate);
    return bounce;
}


// 1 don't need semicolon to seperate next clause
// {xx := 2}
// 2 don't need semicolon to seperate next clause
// convert word to identifier
// xxx / abc / efg
// xxx / @var-a / @var-b
// xxx . $abc . $efg
// xxx . abc . efg
// 3 need semicolon to seperate next clause
// xxx % arg1 arg2 arg3 ...;
// xxx | proc arg2 arg3 ...;
// 4 need semicolon to seperate next clause
// convert word to identifier
// xxx abc arg1 arg2 ...;
// xxx @var-a arg1 arg2 ...;
KN SplitClauses(KonState* kstate, KN sentenceRestWords)
{
    // TODO parse symbol type verb
    
    
    KxVector* clauseListVec = KxVector_Init();
    
    KxVector* clauseVec = KxVector_Init();

    KonPair* iter = sentenceRestWords;
    
    int state = 1; // 1 need verb, 2 need objects, 3 need only 1 object
    do {
        KN item = KN_CAR(iter);
        
        if (state == 1) {
            if (KN_IS_SYNTAX_MARKER(item)
                && (
                    CAST_Kon(SyntaxMarker, item)->Type == KN_SYNTAX_MARKER_APPLY
                    
                    || CAST_Kon(SyntaxMarker, item)->Type == KN_SYNTAX_MARKER_PROC_PIPE
                )
            ) {
                // meet % |
                KxVector_Push(clauseVec, item);
                state = 2;
            }
            // meet . / := eg: .name /name := 5 only need one object
            else if (KN_IS_SYNTAX_MARKER(item)
                && (
                    CAST_Kon(SyntaxMarker, item)->Type == KN_SYNTAX_MARKER_GET_SLOT
                    || CAST_Kon(SyntaxMarker, item)->Type == KN_SYNTAX_MARKER_MSG_SIGNAL
                    || CAST_Kon(SyntaxMarker, item)->Type == KN_SYNTAX_MARKER_ASSIGN
                )) {
                
                KxVector_Push(clauseVec, item);
                
                state = 3;
            }
            else if (KN_IS_VECTOR(item)
                || KN_IS_TABLE(item)
                || KN_IsPairList(item)
                || KN_IsBlock(item)
                || KN_IS_CELL(item)
                || KN_IS_QUOTE(item)
                || KN_IS_QUASIQUOTE(item)
                || KN_IS_EXPAND(item)
            ) {
                KxVector* singleItemClause = KxVector_Init();
                KxVector_Push(clauseListVec, singleItemClause);
                KxVector_Push(singleItemClause, item);
            }
            else {
                KxVector_Push(clauseVec, item);
                state = 2;
            }
        }
        else if (state == 3) {
            KxVector_Push(clauseVec, item);
            KxVector_Push(clauseListVec, clauseVec);
            // reset state
            clauseVec = KxVector_Init();
            state = 1;
        }
        else {
            // state == 2
            // meet ;
            if (KN_IS_SYNTAX_MARKER(item)
                && CAST_Kon(SyntaxMarker, item)->Type == KN_SYNTAX_MARKER_CLAUSE_END
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
        
        iter = KN_CDR(iter);
    } while ((KN)iter != KN_NIL);
    
    if (KxVector_Length(clauseVec) > 0) {
        KxVector_Push(clauseListVec, clauseVec);
    }

    KN result = KN_NIL;
    int clauseListVecLen = KxVector_Length(clauseListVec);
    for (int i = clauseListVecLen - 1; i >= 0; i--) {
        KxVector* clauseWords = KxVector_AtIndex(clauseListVec, i);
        KN clause = KN_VectorToKonPairList(kstate, clauseWords);
        result = KN_CONS(kstate, clause, result);
    }

    return result;
}

KonContinuation* AllocContinuationWithType(KonState* kstate, KonContinuationType type)
{
    KonContinuation* cont = (KonContinuation*)tb_allocator_malloc0(kstate->Allocator, sizeof(KonContinuation));
    assert(cont);
    cont->Base.Tag = KN_T_CONTINUATION;
    cont->Type = type;

    // add to heap ptr store
    KN_RecordNewKonNode(kstate, cont);
    return cont;
}

KonTrampoline* AllocBounceWithType(KonState* kstate, KonBounceType type)
{
    KonTrampoline* bounce = (KonTrampoline*)tb_allocator_malloc0(kstate->Allocator, sizeof(KonTrampoline));
    assert(bounce);
    bounce->Type = type;
    return bounce;
}

// pop the top continuation
// the bounce continuation should be cont->Cont
KonTrampoline* KN_RunContinuation(KonState* kstate, KonContinuation* contBeingInvoked, KN val)
{
    // update for gc
    KN_SwitchContinuation(kstate, contBeingInvoked);

    // all sentences finished, return last value
    if (kon_continuation_type(contBeingInvoked) == KN_CONT_RETURN) {
        KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_LAND);
        bounce->Land.Value = val;
        return bounce;
    }
    else if (kon_continuation_type(contBeingInvoked) == KN_CONT_NATIVE_CALLBACK) {
        KonContFuncRef callbackFunc = contBeingInvoked->Native.Callback;
        KonTrampoline* bounce = callbackFunc(kstate, val, contBeingInvoked);
        return bounce;
    }
    else if (kon_continuation_type(contBeingInvoked) == KN_CONT_EVAL_SENTENCE_LIST) {
        KN lastSentenceVal = val;
        KonEnv* env = contBeingInvoked->Env;
        KN restSentences = contBeingInvoked->RestJobs;
        if (restSentences == KN_NIL) {
            // block sentences all finished
            KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
            bounce->Cont = contBeingInvoked->Cont;
            bounce->Run.Value = lastSentenceVal;
            return bounce;
        }
        else {
            return KN_EvalSentences(kstate, restSentences, env, contBeingInvoked->Cont);
        }
    }
    else if (kon_continuation_type(contBeingInvoked) == KN_CONT_EVAL_SUBJ) {
        // subj evaled, now should eval clauses
        KN subj = val;
        KN restWords = contBeingInvoked->RestJobs;

        if (restWords == KN_NIL) {
            // no other words besids subj, is a sentence like {"abc"}
            // finish this sentence. use subj as return val
            KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
            bounce->Cont = contBeingInvoked->Cont;
            bounce->Run.Value = subj;
            return bounce;
        }
        else {
            // split sub clauses
            // TODO split % xxx; .xxx a a ; | xx aaa; sss
            // as {{% xxx} {. xxx a a} {| xx aaa} {sss}}
            KN clauses = SplitClauses(kstate, restWords);
            KN_DEBUG("splited clauses %s", KN_StringToCstr(KN_Stringify(kstate, clauses)));
            
            if (clauses == KN_NIL) {
                // no clauses like {{a}}
                KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
                bounce->Cont = contBeingInvoked->Cont;
                bounce->Run.Value = subj;
                return bounce;
            }

            // eval the first clause
            KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_EVAL_CLAUSE_LIST);
            k->Cont = contBeingInvoked->Cont;
            k->Env = contBeingInvoked->Env;
            k->EvalClauseList.Subj = subj;
            k->EvalClauseList.RestClauses = KN_CDR(clauses);

            KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_CLAUSE_LIST);
            bounce->Cont = k;
            bounce->Bounce.Value = KN_CAR(clauses);
            bounce->Bounce.Env = contBeingInvoked->Env;
            return bounce;
        }
        
    }
    else if (kon_continuation_type(contBeingInvoked) == KN_CONT_EVAL_CLAUSE_LIST) {
        // last clause eval finshed, eval next clause
        // last clause eval result is the subj of the next clause
        KN subj = val;
        // KN restClauseList = contBeingInvoked->RestJobs;
        KN restClauseList = contBeingInvoked->EvalClauseList.RestClauses;
        if (restClauseList == KN_NIL) {
            // no other clauses, is a sentence like {writeln % "abc"}
            // finish this sentence. use last clause eval result as return val
            KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
            bounce->Cont = contBeingInvoked->Cont;
            bounce->Run.Value = subj;
            return bounce;
        }
        else {
            // eval the next clause
            KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_EVAL_CLAUSE_LIST);
            k->Cont = contBeingInvoked->Cont;
            k->Env = contBeingInvoked->Env;
            k->EvalClauseList.Subj = subj;
            k->EvalClauseList.RestClauses = KN_CDR(restClauseList);

            KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_CLAUSE_LIST);
            bounce->Cont = k;
            bounce->Bounce.Value = KN_CAR(restClauseList);
            bounce->Bounce.Env = contBeingInvoked->Env;
            
            return bounce;
        }
    }
    else if (kon_continuation_type(contBeingInvoked) == KN_CONT_EVAL_CLAUSE_ARGS) {
        KN lastArgEvaled = val;
        KN subj = contBeingInvoked->EvalClauseArgs.Subj;
        KN restArgList = contBeingInvoked->EvalClauseArgs.RestArgList;
        KN evaledArgList = contBeingInvoked->EvalClauseArgs.EvaledArgList;
        // NOTE! the evaluated arg list here is reverted saved
        // should reverted back when apply the arguments
        evaledArgList = KN_CONS(kstate, lastArgEvaled, evaledArgList);

        if (restArgList == KN_NIL) {
            // this clause args all eval finished
            KN argList = KN_PairListRevert(kstate, evaledArgList);
            KN_DEBUG("before ApplySubjVerbAndObjects");
            // next continuation should be KN_CONT_EVAL_CLAUSE_LIST
            return ApplySubjVerbAndObjects(kstate, subj, argList, contBeingInvoked->Env, contBeingInvoked->Cont);
        }
        else {
            // eval the next arg
            KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_EVAL_CLAUSE_ARGS);
            k->Cont = contBeingInvoked->Cont;
            k->Env = contBeingInvoked->Env;
            k->EvalClauseArgs.Subj = subj;
            k->EvalClauseArgs.RestArgList = KN_CDR(restArgList);
            k->EvalClauseArgs.EvaledArgList = evaledArgList;
            
            KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_ARG_LIST);
            bounce->Cont = k;
            bounce->Bounce.Env = contBeingInvoked->Env;
            bounce->Bounce.Value = KN_CAR(restArgList);
            return bounce;
        }
    }

    else if (kon_continuation_type(contBeingInvoked) == KN_CONT_EVAL_QUASI_LIST_ITEMS) {
        KN lastItemEvaled = val;
        KN restList = contBeingInvoked->EvalListItems.RestList;
        KN evaledList = contBeingInvoked->EvalListItems.EvaledList;
        // NOTE! the evaluated  list here is reverted saved
        // should reverted back when rest is nil
        evaledList = KN_CONS(kstate, lastItemEvaled, evaledList);

        if (restList == KN_NIL) {
            KN evaledQuasi = KN_PairListRevert(kstate, evaledList);
            KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
            bounce->Cont = contBeingInvoked->Cont;
            bounce->Run.Value = evaledQuasi;
            return bounce;
        }
        else {
            // eval the next list item
            KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_EVAL_QUASI_LIST_ITEMS);
            k->Cont = contBeingInvoked->Cont;
            k->Env = contBeingInvoked->Env;
            k->EvalListItems.RestList = KN_CDR(restList);
            k->EvalListItems.EvaledList = evaledList;
            
            KN listItem = KN_CAR(restList);
            
            KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_QUASI_ITEM);
            bounce->Cont = k;
            bounce->Bounce.Env = contBeingInvoked->Env;
            bounce->Bounce.Value = listItem;
            return bounce;
        }
    }

    else {
        // TODO throw error
//        KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
//        bounce->Cont = contBeingInvoked->Cont;
//        bounce->Run.Value = KON_TRUE;
//        return bounce;
        printf("unhandled continuation type\n");
        exit(1);
    }
}

bool KN_IsPrefixMarcro(KN word) {
    if (!(KN_CHECK_TAG(word, KN_T_SYMBOL))) {
        return false;
    }
    int type = ((KonSymbol*)word)->Type;
    if (type != KN_SYM_PREFIX_WORD && type != KN_SYM_WORD) {
        return false;
    }
    
    // check reserved keywords
    const char* prefix = KN_UNBOX_SYMBOL(word);

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
        || strcmp(prefix, "macro-func") == 0
        || strcmp(prefix, "macro-lambda") == 0
        || strcmp(prefix, "or") == 0
        || strcmp(prefix, "set") == 0
        || strcmp(prefix, "set-dispatcher") == 0
        || strcmp(prefix, "sh") == 0
    ) {
        return true;
    }
    else {
        return false;
    }
}

KonTrampoline* KN_EvalExpression(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KonTrampoline* bounce;
    if (IsSelfEvaluated(expression)) {
        bounce = KN_RunContinuation(kstate, cont, expression);
    }
    else if (KN_IS_CELL(expression)) {
        // passed a sentence like {writeln "abc" "efg"}
        KonCell* cell = CAST_Kon(Cell, expression);
        KN first = cell->Core;
        if (KN_IsPrefixMarcro(first)) {
            const char* prefix = KN_UNBOX_SYMBOL(first);
            if (strcmp(prefix, "apply") == 0) {
                bounce = KN_EvalPrefixApply(kstate, (KN)cell, env, cont);
            }
            else if (strcmp(prefix, "eval") == 0) {
                bounce = KN_EvalPrefixEval(kstate, (KN)cell, env, cont);
            }
            else if (strcmp(prefix, "let") == 0) {
                bounce = KN_EvalPrefixLet(kstate, KN_DNR(cell), env, cont);
            }
            else if (strcmp(prefix, "set") == 0) {
                bounce = KN_EvalPrefixSet(kstate, KN_DNR(cell), env, cont);
            }
            else if (strcmp(prefix, "lambda") == 0) {
                bounce = KN_EvalPrefixLambda(kstate, (KN)cell, env, cont);
            }
            else if (strcmp(prefix, "func") == 0) {
                bounce = KN_EvalPrefixFunc(kstate, (KN)cell, env, cont);
            }
            else if (strcmp(prefix, "macro-lambda") == 0) {
                bounce = KN_EvalPrefixMacroLambda(kstate, (KN)cell, env, cont);
            }
            else if (strcmp(prefix, "macro-func") == 0) {
                bounce = KN_EvalPrefixMacroFunc(kstate, (KN)cell, env, cont);
            }
            else if (strcmp(prefix, "blk") == 0) {
                bounce = KN_EvalPrefixBlk(kstate, (KN)cell, env, cont);
            }
            else if (strcmp(prefix, "do") == 0) {
                bounce = KN_EvalPrefixDo(kstate, (KN)cell, env, cont);
            }
            else if (strcmp(prefix, "cond") == 0) {
                bounce = KN_EvalPrefixCond(kstate, KN_DNR(cell), env, cont);
            }
            else if (strcmp(prefix, "if") == 0) {
                bounce = KN_EvalPrefixIf(kstate, KN_DNR(cell), env, cont);
            }
            else if (strcmp(prefix, "for") == 0) {
                bounce = KN_EvalPrefixFor(kstate, (KN)cell, env, cont);
            }
            else if (strcmp(prefix, "break") == 0) {
                bounce = KN_EvalPrefixBreak(kstate, KN_DNR(cell), env, cont);
            }
            else if (strcmp(prefix, "continue") == 0) {
                bounce = KN_EvalPrefixContinue(kstate, KN_DNR(cell), env, cont);
            }
            else if (strcmp(prefix, "call-cc") == 0) {
                bounce = KN_EvalPrefixCallcc(kstate, KN_DNR(cell), env, cont);
            }

            else if (strcmp(prefix, "def-dispatcher") == 0) {
                bounce = KN_EvalPrefixDefDispatcher(kstate, KN_DNR(cell), env, cont);
            }
            else if (strcmp(prefix, "set-dispatcher") == 0) {
                bounce = KN_EvalPrefixSetDispatcher(kstate, KN_DNR(cell), env, cont);
            }
            else if (strcmp(prefix, "def-builder") == 0) {
                bounce = KN_EvalPrefixDefBuilder(kstate, KN_DNR(cell), env, cont);
            }

            else if (strcmp(prefix, "and") == 0) {
                bounce = KN_EvalPrefixAnd(kstate, KN_DNR(cell), env, cont);
            }
            else if (strcmp(prefix, "or") == 0) {
                bounce = KN_EvalPrefixOr(kstate, KN_DNR(cell), env, cont);
            }
            else if (strcmp(prefix, "sh") == 0) {
                bounce = KN_EvalPrefixSh(kstate, KN_DNR(cell), env, cont);
            }

            else {
                KN_DEBUG("error! unhandled keyword");
                bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
                bounce->Run.Value = KN_UKN;
                bounce->Cont = cont;
            }
        }
        else {
            bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_SUBJ);
            KN restJobs;

            if (KN_IS_PREFIX_MARCRO(first)) {
                KonQuote* tmp = KN_ALLOC_TYPE_TAG(kstate, KonQuote, KN_T_QUOTE);
                tmp->Type = KN_QUOTE_CELL;
                tmp->Inner = cell;

                KonSyntaxMarker* applyMarker = KN_ALLOC_TYPE_TAG(kstate, KonSyntaxMarker, KN_T_SYNTAX_MARKER);
                applyMarker->Type = KN_SYNTAX_MARKER_APPLY;

                // [${!xxx}]
                KN wrapperedArgs = KN_CONS(kstate, tmp, KN_NIL);
                // [% ${!xxx}]
                restJobs = KN_CONS(kstate, applyMarker, wrapperedArgs);
            }
            else {
                // transform cell to list
                restJobs = KN_CellToWordList(kstate, KN_DNR(cell));
            }
            
            KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_EVAL_SUBJ);
            k->Cont = cont;
            k->Env = env;
            k->RestJobs = restJobs;
            
            bounce->Bounce.Value = first;  // get subj word
            bounce->Cont = k;
            bounce->Bounce.Env = env;
        }
    }
    else if (KN_IsPairList(expression)) {
        // passed a sentence like [writeln % "abc" "efg"]
        KN words = expression;
        KN first = KN_CAR(words);

        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_SUBJ);

        // seperate and transform [+ 1 2 3] to subj: +, restJobs: [% 1 2 3]
        KonSyntaxMarker* applyMarker = KN_ALLOC_TYPE_TAG(kstate, KonSyntaxMarker, KN_T_SYNTAX_MARKER);
        applyMarker->Type = KN_SYNTAX_MARKER_APPLY;

        KN arguments = KN_CDR(words);

        // if the first is a macro marker, quote the arguments
        if (KN_IS_PREFIX_MARCRO(first)) {
            KonQuote* tmp = KN_ALLOC_TYPE_TAG(kstate, KonQuote, KN_T_QUOTE);
            tmp->Type = KN_QUOTE_LIST;
            tmp->Inner = arguments;
            KN wrapperedArgs = KN_CONS(kstate, tmp, KN_NIL);

            arguments = wrapperedArgs;
        }

        KN restJobs = KN_CONS(kstate, applyMarker, arguments);
        

        KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_EVAL_SUBJ);
        k->Cont = cont;
        k->Env = env;
        k->RestJobs = restJobs;
        
        bounce->Bounce.Value = first;  // get subj word
        bounce->Cont = k;
        bounce->Bounce.Env = env;

    }
    else if (KN_IS_REFERENCE(expression)) {
        // a code block like { a }
        // TODO asert should be a SYM_IDENTIFIER
        // env lookup this val
        KN val = KN_EnvLookup(kstate, env, KN_SymbolToCstr(expression));
        assert(val != KN_UNDEF);
        bounce = KN_RunContinuation(kstate, cont, val);
    }
    else if (KN_IS_ACCESSOR(expression)) {
        // unbox
        KN val = UnBoxAccessorValue(expression);
        bounce = KN_RunContinuation(kstate, cont, val);
    }
    else if (KN_IS_QUASI_PAIR(expression)) {
        KN_DEBUG("eval quansiquote list expression");
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_QUASI_ITEM);
        KN innerList = KN_UNBOX_QUASI(expression);
        KN restJobs = KN_CDR(innerList);

        KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_EVAL_QUASI_LIST_ITEMS);
        k->Cont = cont;
        k->Env = env;
        k->EvalListItems.RestList = restJobs;
        k->EvalListItems.EvaledList = KN_NIL;
        
        bounce->Bounce.Value = KN_CAR(innerList);  // get subj word
        bounce->Cont = k;
        bounce->Bounce.Env = env;
    }
//    else if (KN_IS_UNQUOTE(expression)) {
//        // illegal! syntax error [+ $.x], should be @[+ $.x]
//        bounce = KN_RunContinuation(kstate, cont, expression);
//    }
//    else if (KN_IS_EXPAND(expression)) {
//        // TODO
//        bounce = KN_RunContinuation(kstate, cont, expression);
//    }
    else {
        KN_DEBUG("unhandled expression type");
        exit(1);
    }
    return bounce;
}

KonTrampoline* KN_EvalSentences(KonState* kstate, KN sentences, KonEnv* env, KonContinuation* cont)
{
    if (sentences == KN_NIL) {
        // block is empty, no sentence
        KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->Run.Value = KN_TRUE;
        bounce->Cont = cont;
        return bounce;
    }
    else {
        KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_BLOCK);
        
        KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_EVAL_SENTENCE_LIST);
        k->Cont = cont;
        k->Env = env;
        k->RestJobs = KN_CDR(sentences);
        
        bounce->Bounce.Value = KN_CAR(sentences);
        bounce->Cont = k;
        bounce->Bounce.Env = env;
        return bounce;
    }
}



KN KN_ProcessSentences(KonState* kstate, KN sentences, KonEnv* rootEnv)
{
    // TODO add step count when debug
    KN formated = KN_ToFormatString(kstate, sentences, true, 0, "  ");
    KN_DEBUG("%s", KN_StringToCstr(formated));
    
    KonContinuation* firstCont = AllocContinuationWithType(kstate, KN_CONT_RETURN);
    firstCont->Env = rootEnv;
    // set root level return
    KN_EnvDefine(kstate, rootEnv, "return", firstCont);
    KonTrampoline* bounce = KN_EvalSentences(kstate, sentences, rootEnv, firstCont);
    
    // update current code
    kstate->CurrCode = sentences;
    KN_SwitchContinuation(kstate, firstCont);

    while (kon_bounce_type(bounce) != KN_TRAMPOLINE_LAND) {

        if (kon_bounce_type(bounce) == KN_TRAMPOLINE_RUN) {
            KonTrampoline* oldBounce = bounce;
            KN value = bounce->Run.Value;
            KonContinuation* cont = bounce->Cont;
            bounce = KN_RunContinuation(kstate, cont, value);
        }
        else if (kon_bounce_type(bounce) == KN_TRAMPOLINE_BLOCK) {
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->Cont;
            KonEnv* env = bounce->Bounce.Env;
            KN sentence = bounce->Bounce.Value;

            bounce = KN_EvalExpression(kstate, sentence, env, cont);
        }
        else if (kon_bounce_type(bounce) == KN_TRAMPOLINE_SUBJ) {
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->Cont;
            KonEnv* env = bounce->Bounce.Env;
            KN subj = bounce->Bounce.Value;
            if (KN_IsPairList(subj)) {
                // the subj position is a list, should be evaluated
                // like the first sentence in this eg block {{ {zhangsan name} |upcase }}
                KN subjExpr = subj;
                bounce = KN_EvalExpression(kstate, subjExpr, env, cont);
            }
            else if (KN_IS_REFERENCE(subj)) {
                // lookup subject in env
                KN val = KN_EnvLookup(kstate, env, KN_SymbolToCstr(subj));
                assert(val != KN_UNDEF);
                bounce = KN_RunContinuation(kstate, cont, val);
            }
            else if (KN_IS_CELL(subj)) {
                // TODO !!! verify cell inner content(tag, list, vector, table )
                // whether have Quasiquote, Expand, Unquote, KN_SYM_VARIABLE node
                KN subjExpr = subj;
                bounce = KN_EvalExpression(kstate, subjExpr, env, cont);
            }
            // TODO quasiquote unquote, etc.
            
            else if (IsSelfEvaluated(subj)) {
                bounce = KN_RunContinuation(kstate, cont, subj);
            }
        }
        else if (kon_bounce_type(bounce) == KN_TRAMPOLINE_CLAUSE_LIST) {
            
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->Cont;
            
            KN subj = cont->EvalClauseList.Subj;
            KonEnv* env = bounce->Bounce.Env;
            KN clauseArgList = bounce->Bounce.Value;
            

            KN firstArg = KN_CAR(clauseArgList);

            ////
            // infix processing
            
            // infix: % | / .
            if (KN_IS_SYNTAX_MARKER(firstArg)) {
                KN evaledArgList;
                KN restArgList;
                KN firstToEval;
                
                // syntax sugar.
                // 1 the first word symbol arg after . marker, don't need to eval
                //   eg: "zhangsan" . length
                // 2 the first word symbol arg after / maker, don't need eval
                if ((CAST_Kon(SyntaxMarker, firstArg)->Type == KN_SYNTAX_MARKER_MSG_SIGNAL
                        || CAST_Kon(SyntaxMarker, firstArg)->Type == KN_SYNTAX_MARKER_GET_SLOT
                    )
                    && KN_IS_WORD(KN_CADR(clauseArgList))
                ) {
                    KN firstAfterMarker = KN_CADR(clauseArgList);
                    ((KonSymbol*)firstAfterMarker)->Type = KN_SYM_IDENTIFIER;
                }
                evaledArgList = KN_NIL;
                firstToEval = firstArg;
                restArgList = KN_CDR(clauseArgList);
                
                KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_EVAL_CLAUSE_ARGS);
                k->Cont = cont;
                k->Env = env;
                k->EvalClauseArgs.Subj = subj;
                k->EvalClauseArgs.RestArgList = restArgList;
                k->EvalClauseArgs.EvaledArgList = evaledArgList;

                bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_ARG_LIST);
                bounce->Cont = k;
                bounce->Bounce.Env = env;
                bounce->Bounce.Value = firstToEval; // the first arg is % or . or |

            }
            // infix word, list, cell, convert it to data type
            else if (KN_IS_SYMBOL(firstArg)
                || KN_IS_PAIR(firstArg)
                || KN_IS_CELL(firstArg)
            ) {

                KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_EVAL_CLAUSE_ARGS);
                k->Cont = cont;
                k->Env = env;
                k->EvalClauseArgs.Subj = subj;
                k->EvalClauseArgs.RestArgList = KN_CDR(clauseArgList);
                k->EvalClauseArgs.EvaledArgList = KN_NIL;

                // eg: {obj clone}, convert clone to $clone
                if (KN_IS_WORD(firstArg)) {
                    ((KonSymbol*)firstArg)->Type = KN_SYM_IDENTIFIER;
                }
                else if (KN_IS_PAIR(firstArg)) {
                    KonQuote* tmp = KN_ALLOC_TYPE_TAG(kstate, KonQuote, KN_T_QUOTE);
                    tmp->Type = KN_QUOTE_LIST;
                    tmp->Inner = firstArg;
                    firstArg = (KN)tmp;
                }
                else if (KN_IS_CELL(firstArg)) {
                    KonQuote* tmp = KN_ALLOC_TYPE_TAG(kstate, KonQuote, KN_T_QUOTE);
                    tmp->Type = KN_QUOTE_CELL;
                    tmp->Inner = firstArg;
                    firstArg = (KN)tmp;
                }
                bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_ARG_LIST);
                bounce->Cont = k;
                bounce->Bounce.Env = env;
                bounce->Bounce.Value = firstArg; // the first arg is /abc
            }
            else {
                KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_EVAL_CLAUSE_ARGS);
                k->Cont = cont;
                k->Env = env;
                k->EvalClauseArgs.Subj = subj;
                k->EvalClauseArgs.RestArgList = KN_CDR(clauseArgList);
                k->EvalClauseArgs.EvaledArgList = KN_NIL;
                
                bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_ARG_LIST);
                bounce->Cont = k;
                bounce->Bounce.Env = env;
                bounce->Bounce.Value = firstArg;
            }
        }

        else if (kon_bounce_type(bounce) == KN_TRAMPOLINE_ARG_LIST) {
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->Cont;
            KonEnv* env = bounce->Bounce.Env;
            KN arg = bounce->Bounce.Value;
            // eager evaluation
            bounce = KN_EvalExpression(kstate, arg, env, cont);
        }
        else if (kon_bounce_type(bounce) == KN_TRAMPOLINE_QUASI_ITEM) {
            // eval each quasi list item
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->Cont;
            KonEnv* env = bounce->Bounce.Env;
            KN item = bounce->Bounce.Value;
            if (KN_IS_UNQUOTE(item)) {
                bounce = KN_EvalExpression(kstate, KN_UNBOX_UNQUOTE(item), env, cont);
            }
            else {
                bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
                bounce->Run.Value = item;
                bounce->Cont = cont;
            }
        }
        else {
            printf("unhandled bounce type\n");
            exit(1);
        }
    }
    // no other continuations
    KN_SwitchContinuation(kstate, NULL);
    return bounce->Land.Value;
}

