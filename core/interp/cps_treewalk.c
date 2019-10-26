#include "cps_treewalk.h"
#include "op_handlers.h"


KN KN_EvalFile(KonState* knstate, char* filePath)
{
    KN_DEBUG("KN_EvalFile enter");

    KonReader* reader = KSON_ReaderInit(knstate);
    if (!reader) {
        KN_DEBUG("KN_EvalFile init failed");
        exit(1);
    }

    KN result = KN_UNDEF;

    bool openRes = KSON_ReaderFromFile(reader, filePath);
    KN_DEBUG("---eval file %s\n", filePath);
    
    if (openRes) {
        KN root = KSON_Parse(reader);
        KN_ShowGcStatics(knstate);
        KSON_ReaderCloseStream(reader);
        // 释放读取器 
        KSON_ReaderExit(reader);
        if (KN_IsPairList(root)) {
            KonEnv* rootEnv = KN_MakeRootEnv(knstate);

        //     // DefineReservedDispatcher(knstate, env);

        //     // KN result = KN_ExecAst(knstate, root, knstate->value.Context.rootEnv);
            KonEnv* processEnv = KN_MakeChildEnv(knstate, rootEnv);
            result = KN_ExecAst(knstate, root, processEnv);
            
            KN_DEBUG("eval sentences success");
            KN formated = KN_ToFormatString(knstate, result, true, 0, "  ");
            KN_DEBUG("%s", KN_StringToCstr(formated));

            KN_ShowGcStatics(knstate);
            KN_DEBUG("---------\n");
        }
        
    }
    else {
        KN_DEBUG("open stream failed");
    }
    

    return result;
}

OpHandlerRef opHandlers[] = {
    OpHandler_NONE,                 // OPC_LAND
    OpHandler_NONE,                 // OPC_NOP
    OpHandler_HELLOWORLD,           // OPC_HELLOWORLD
};

KN KN_ExecAst(KonState* knstate, KN sourceCodeAst, KonEnv* rootEnv)
{
    KonEnv* curEnv = rootEnv;
    KonContinuation* curCont = AllocContinuationWithType(knstate, KN_CONT_RETURN, curEnv, NULL);

    KN* globalKonRegs = (KN*)tb_nalloc0(2, sizeof(KN));
    // void** globalPtrRegs = (void**)tb_nalloc0(2, sizeof(void*));
    // int32_t* globalRegs32 = (int32_t*)tb_nalloc0(2, sizeof(int32_t));
    KN_OP* nextOp = (KN_OP*)tb_nalloc0(1, sizeof(KN_OP));
    nextOp->code = OPC_HELLOWORLD;

    GS_LAST_VAL = KN_UKN;
    GS_NEXT_CODE = sourceCodeAst;

    // TODO add step count when debug
    KN formatedCode = KN_ToFormatString(knstate, GS_NEXT_CODE, true, 0, "  ");
    KN_DEBUG("%s", KN_StringToCstr(formatedCode));

    static const void* opLabels[] = {
        &&opc0, &&opc1, &&opc2, // &&opc3, &&opc4, &&opc5, &&opc6, &&opc7, &&opc8, &&opc9, &&opc10,
        // &&opc11, &&opc12, &&opc13, &&opc14, &&opc15, &&opc16, &&opc17, &&opc18, &&opc19, &&opc20,
        // &&opc21, &&opc22, &&opc23, &&opc24, &&opc25, &&opc26, &&opc27, &&opc28, &&opc29, &&opc30,
        // &&opc31, &&opc32, &&opc33, &&opc34, &&opc35, &&opc36, &&opc37, &&opc38, &&opc39, &&opc40,
        // &&unused,
    };
    
    DISPATCH(nextOp->code);

    DEF_OPC(OPC_DISPATCH_BY_TYPE)
    {
        // printf("HELLO OP EXECUTED\n");
        // HANDLE_OP(HELLOWORLD, (knstate, curEnv, curCont, globalKonRegs, nextOp));
        // DISPATCH(nextOp->code);
        DISPATCH(OPC_HELLOWORLD);
    }

    DEF_OPC(OPC_HELLOWORLD)
    {
        printf("HELLO OP EXECUTED\n");
        HANDLE_OP(HELLOWORLD, (knstate, curEnv, curCont, globalKonRegs, nextOp));
        DISPATCH(nextOp->code);
    }
    unused:
    {
        KN_DEBUG("warning: dispatched to unused opcode");
        DISPATCH(OPC_LAND); // return
    }
    throwException:
    {
    }
    DEF_OPC(OPC_NOP) {
        DISPATCH(OPC_LAND);
    }
    DEF_OPC(OPC_LAND) {
        KN_DEBUG("OPC_LAND");
    }
    
    return GS_LAST_VAL;
    
    /*
    KonContinuation* firstCont = AllocContinuationWithType(knstate, KN_CONT_RETURN);
    firstCont->env = rootEnv;
    // set root level return
    KN_EnvDefine(knstate, rootEnv, "return", firstCont);
    KonTrampoline* bounce = KN_EvalSentences(knstate, sentences, rootEnv, firstCont);
    
    // update current code
    knstate->currCode = sentences;

    while (kon_bounce_type(bounce) != KN_TRAMPOLINE_LAND) {

        if (kon_bounce_type(bounce) == KN_TRAMPOLINE_RUN) {
            KonTrampoline* oldBounce = bounce;
            KN value = bounce->run.value;
            KonContinuation* cont = bounce->cont;
            bounce = KN_RunContinuation(knstate, cont, value);
        }
        else if (kon_bounce_type(bounce) == KN_TRAMPOLINE_BLOCK) {
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->cont;
            KonEnv* env = bounce->bounce.env;
            KN sentence = bounce->bounce.value;

            bounce = KN_EvalExpression(knstate, sentence, env, cont);
        }
        else if (kon_bounce_type(bounce) == KN_TRAMPOLINE_SUBJ) {
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->cont;
            KonEnv* env = bounce->bounce.env;
            KN subj = bounce->bounce.value;
            if (KN_IsPairList(subj)) {
                // the subj position is a list, should be evaluated
                // like the first sentence in this eg block {{ {zhangsan name} |upcase }}
                KN subjExpr = subj;
                bounce = KN_EvalExpression(knstate, subjExpr, env, cont);
            }
            else if (KN_IS_REFERENCE(subj)) {
                // lookup subject in env
                KN val = KN_EnvLookup(knstate, env, KN_SymbolToCstr(subj));
                assert(val != KN_UNDEF);
                bounce = KN_RunContinuation(knstate, cont, val);
            }
            else if (KN_IS_CELL(subj)) {
                // TODO !!! verify cell inner content(tag, list, vector, table )
                // whether have Quasiquote, Expand, Unquote, KN_SYM_VARIABLE node
                KN subjExpr = subj;
                bounce = KN_EvalExpression(knstate, subjExpr, env, cont);
            }
            // TODO quasiquote unquote, etc.
            
            else if (IsSelfEvaluated(subj)) {
                bounce = KN_RunContinuation(knstate, cont, subj);
            }
        }
        else if (kon_bounce_type(bounce) == KN_TRAMPOLINE_CLAUSE_LIST) {
            
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->cont;
            
            KN subj = cont->evalClauseList.subj;
            KonEnv* env = bounce->bounce.env;
            KN clauseArgList = bounce->bounce.value;
            

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
                //   eg: "zhangsan" . append "abc"
                // 2 the first word symbol arg after / maker, don't need eval
                if ((CAST_Kon(SyntaxMarker, firstArg)->type == KN_SYNTAX_MARKER_MSG_SIGNAL
                        || CAST_Kon(SyntaxMarker, firstArg)->type == KN_SYNTAX_MARKER_GET_SLOT
                    )
                    && KN_IS_WORD(KN_CADR(clauseArgList))
                ) {
                    KN firstAfterMarker = KN_CADR(clauseArgList);
                    ((KonSymbol*)firstAfterMarker)->type = KN_SYM_IDENTIFIER;
                }
                evaledArgList = KN_NIL;
                firstToEval = firstArg;
                restArgList = KN_CDR(clauseArgList);
                
                KonContinuation* k = AllocContinuationWithType(knstate, KN_CONT_EVAL_CLAUSE_ARGS);
                k->cont = cont;
                k->env = env;
                k->evalClauseArgs.subj = subj;
                k->evalClauseArgs.restArgList = restArgList;
                k->evalClauseArgs.evaledArgList = evaledArgList;

                bounce = AllocBounceWithType(knstate, KN_TRAMPOLINE_ARG_LIST);
                bounce->cont = k;
                bounce->bounce.env = env;
                bounce->bounce.value = firstToEval; // the first arg is % or . or |

            }
            // infix word, list, cell, convert it to data type
            else if (KN_IS_SYMBOL(firstArg)
                || KN_IS_PAIR(firstArg)
                || KN_IS_CELL(firstArg)
            ) {

                KonContinuation* k = AllocContinuationWithType(knstate, KN_CONT_EVAL_CLAUSE_ARGS);
                k->cont = cont;
                k->env = env;
                k->evalClauseArgs.subj = subj;
                k->evalClauseArgs.restArgList = KN_CDR(clauseArgList);
                k->evalClauseArgs.evaledArgList = KN_NIL;

                // eg: {obj clone}, convert clone to $clone
                if (KN_IS_WORD(firstArg)) {
                    ((KonSymbol*)firstArg)->type = KN_SYM_IDENTIFIER;
                }
                else if (KN_IS_PAIR(firstArg)) {
                    KonQuote* tmp = KN_ALLOC_TYPE_TAG(knstate, KonQuote, KN_T_QUOTE);
                    tmp->type = KN_QUOTE_LIST;
                    tmp->inner = firstArg;
                    firstArg = (KN)tmp;
                }
                else if (KN_IS_CELL(firstArg)) {
                    KonQuote* tmp = KN_ALLOC_TYPE_TAG(knstate, KonQuote, KN_T_QUOTE);
                    tmp->type = KN_QUOTE_CELL;
                    tmp->inner = firstArg;
                    firstArg = (KN)tmp;
                }
                bounce = AllocBounceWithType(knstate, KN_TRAMPOLINE_ARG_LIST);
                bounce->cont = k;
                bounce->bounce.env = env;
                bounce->bounce.value = firstArg; // the first arg is /abc
            }
            else {
                KonContinuation* k = AllocContinuationWithType(knstate, KN_CONT_EVAL_CLAUSE_ARGS);
                k->cont = cont;
                k->env = env;
                k->evalClauseArgs.subj = subj;
                k->evalClauseArgs.restArgList = KN_CDR(clauseArgList);
                k->evalClauseArgs.evaledArgList = KN_NIL;
                
                bounce = AllocBounceWithType(knstate, KN_TRAMPOLINE_ARG_LIST);
                bounce->cont = k;
                bounce->bounce.env = env;
                bounce->bounce.value = firstArg;
            }
        }

        else if (kon_bounce_type(bounce) == KN_TRAMPOLINE_ARG_LIST) {
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->cont;
            KonEnv* env = bounce->bounce.env;
            KN arg = bounce->bounce.value;
            // eager evaluation
            bounce = KN_EvalExpression(knstate, arg, env, cont);
        }
        else if (kon_bounce_type(bounce) == KN_TRAMPOLINE_QUASI_ITEM) {
            // eval each quasi list item
            KonTrampoline* oldBounce = bounce;
            KonContinuation* cont = bounce->cont;
            KonEnv* env = bounce->bounce.env;
            KN item = bounce->bounce.value;
            if (KN_IS_UNQUOTE(item)) {
                bounce = KN_EvalExpression(knstate, KN_UNBOX_UNQUOTE(item), env, cont);
            }
            else {
                bounce = AllocBounceWithType(knstate, KN_TRAMPOLINE_RUN);
                bounce->run.value = item;
                bounce->cont = cont;
            }
        }
        else {
            printf("unhandled bounce type\n");
            exit(1);
        }
    }

    return bounce->land.value;
    */
}