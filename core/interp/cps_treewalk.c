#include "cps_treewalk.h"
#include "op_handlers.h"


KN KN_EvalFile(Kana* kana, char* filePath)
{
    KN_DEBUG("KN_EvalFile enter");

    KonReader* reader = KSON_ReaderInit(kana);
    if (!reader) {
        KN_DEBUG("KN_EvalFile init failed");
        exit(1);
    }

    KN result = KN_UNDEF;

    bool openRes = KSON_ReaderFromFile(reader, filePath);
    KN_DEBUG("---eval file %s\n", filePath);
    
    if (openRes) {
        KN root = KSON_Parse(reader);
        KN_ShowGcStatics(kana);
        KSON_ReaderCloseStream(reader);
        // 释放读取器 
        KSON_ReaderExit(reader);
        if (KN_IsPairList(root)) {
            KonEnv* rootEnv = KN_MakeRootEnv(kana);

        //     // DefineReservedDispatcher(kana, env);

        //     // KN result = KN_ExecAst(kana, root, kana->value.Context.rootEnv);
            KonEnv* processEnv = KN_MakeChildEnv(kana, rootEnv);
            result = KN_ExecAst(kana, root, processEnv);
            
            KN_DEBUG("eval sentences success");
            KN formated = KN_ToFormatString(kana, result, true, 0, "  ");
            KN_DEBUG("%s", KN_StringToCstr(formated));

            KN_ShowGcStatics(kana);
            KN_DEBUG("---------\n");
        }
        
    }
    else {
        KN_DEBUG("open stream failed");
    }
    

    return result;
}

bool IsSelfEvaluated(KN source)
{
    if (KN_IS_FIXNUM(source)
        || KN_IS_FLONUM(source)
        || KN_IS_STRING(source)
        || KN_IS_SYNTAX_MARKER(source)
        // $abc
        || (KN_IS_IDENTIFIER(source))
        // $'abc'
        || (KN_IS_SYMBOL(source) && KN_2_KON(source, Symbol)->type == KN_SYM_STRING)
        || KN_IS_QUOTE(source)
        || KN_IS_VECTOR(source)
        || KN_IS_TABLE(source)
        || KN_IS_MAP(source)
        || source.asU64 == KNBOX_TRUE
        || source.asU64 == KNBOX_FALSE
        || source.asU64 == KNBOX_NIL
        || source.asU64 == KNBOX_UKN
        || source.asU64 == KNBOX_UNDEF
    ) {
        return true;
    }
    else {
        return false;
    }
}

KN UnBoxAccessorValue(KN konValue)
{
    if (!KN_IS_ACCESSOR(konValue)) {
        return konValue;
    }
    KonAccessor* accessor = KN_2_KON(konValue, Accessor);
    if (accessor->isDir == true) {
        return KON_2_KN(accessor);
    }
    KN value = accessor->value;
    while (KN_IS_ACCESSOR(value)
        && !KN_2_KON(value, Accessor)->isDir
    ) {
        value = KN_2_KON(value, Accessor)->value;
    }
    return value;
}

KnOp ApplyProcedureArguments(Kana* kana, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* curCont)
{
    KnOp nextOp;
    // TODO assert if is a procedure
    if (proc->type == KN_NATIVE_FUNC
        || proc->type == KN_NATIVE_OBJ_METHOD
    ) {
        KonNativeFuncRef funcRef = proc->nativeFuncRef;
        GS_LAST_VAL = KN_ApplyArgsToNativeFunc(kana, proc, argList);
        nextOp.code = OPC_RUN_NEXT_CONT;
    }
    // else if (proc->type == KN_COMPOSITE_LAMBDA) {
    //     return KN_ApplyCompositeLambda(kana, proc, argList, env, cont);
    // }
    // else if (proc->type == KN_COMPOSITE_FUNC) {
    //     return KN_ApplyCompositeFunc(kana, proc, argList, env, cont);
    // }
    // NOTE! the arguments are quoted before, unbox here
    // else if (proc->type == KN_COMPOSITE_MACRO_LAMBDA) {
    //     KN unboxed = KN_UNBOX_QUOTE(KN_CAR(argList));
    //     argList = unboxed;
    //     return KN_ApplyCompositeLambda(kana, proc, argList, env, cont);
    // }
    // else if (proc->type == KN_COMPOSITE_MACRO_FUNC) {
    //     KN unboxed = KN_UNBOX_QUOTE(KN_CAR(argList));
    //     argList = unboxed;
    //     return KN_ApplyCompositeFunc(kana, proc, argList, env, cont);
    // }
    // else if (proc->type == KN_COMPOSITE_OBJ_METHOD) {
    //     // treat as plain procedure when apply arg list
    //     // the first item in arg list is the object
    //     return KN_ApplyCompositeLambda(kana, proc, argList, env, cont);
    // }
    // else {
    //     // TODO throw exception
    // }
    return nextOp;
}


KN KN_ExecAst(Kana* kana, KN sourceCodeAst, KonEnv* rootEnv)
{
    KonEnv* curEnv = rootEnv;
    KonContinuation* curCont = AllocContinuationWithType(kana, KN_CONT_RETURN, curEnv, NULL);
    curCont->contHandler = ContHandler_Return;

    // KnOp* nextOp = (KnOp*)tb_nalloc0(1, sizeof(KnOp));
    // nextOp->code = OPC_EVAL_SENTENCES;

    KnOp nextOp = { .code = OPC_EVAL_SENTENCES };

    GS_LAST_VAL = KN_UKN;
    GS_NODE_TO_RUN = sourceCodeAst;

    // TODO add step count when debug
    KN formatedCode = KN_ToFormatString(kana, GS_NODE_TO_RUN, true, 0, "  ");
    KN_DEBUG("%s", KN_StringToCstr(formatedCode));

    static const void* opLabels[] = {
        &&opc0, &&opc1, &&opc2, &&opc3, &&opc4, &&opc5, &&opc6, &&opc7, &&opc8, &&opc9, &&opc10,
        &&opc11, &&opc12, &&opc13, &&opc14, //&&opc15, &&opc16, &&opc17, &&opc18, &&opc19, &&opc20,
        // &&opc21, &&opc22, &&opc23, &&opc24, &&opc25, &&opc26, &&opc27, &&opc28, &&opc29, &&opc30,
        // &&opc31, &&opc32, &&opc33, &&opc34, &&opc35, &&opc36, &&opc37, &&opc38, &&opc39, &&opc40,
        // &&unused,
    };
    
    DISPATCH(nextOp.code);

    DEF_OPC(OPC_HELLOWORLD)
    {
        KN_DEBUG("HELLO OP EXECUTED");
        nextOp = OpHandler_HELLOWORLD(kana, curCont);
        DISPATCH(nextOp.code);
    }

    DEF_OPC(OPC_EVAL_EXPR_BY_TYPE)
    {
        KN_DEBUG("OPC_EVAL_EXPR_BY_TYPE");
        if (IsSelfEvaluated(GS_NODE_TO_RUN)) {
            GS_LAST_VAL = GS_NODE_TO_RUN;
            nextOp.code = OPC_RUN_CURRENT_CONT;
        }
        else if (KN_IS_CELL(GS_NODE_TO_RUN)) {
            // create a new cont
            nextOp.code = OPC_EVAL_CELL_SENTENCE;
        }
        else if (KN_IsPairList(GS_NODE_TO_RUN)) {
            // create a new cont
            nextOp.code = OPC_EVAL_LIST_SENTENCE;
        }
        else if (KN_IS_REFERENCE(GS_NODE_TO_RUN)) {
            // a code block like { a }
            // TODO asert should be a SYM_IDENTIFIER
            // env lookup this val
            nextOp.code = OPC_ENV_LOOKUP;
        }
        else if (KN_IS_ACCESSOR(GS_NODE_TO_RUN)) {
            // unbox
            KN val = UnBoxAccessorValue(GS_NODE_TO_RUN);
            GS_LAST_VAL = val;
            nextOp.code = OPC_RUN_CURRENT_CONT;
        }
        // TODO
        // else if (KN_IS_QUASI_PAIR(GS_NODE_TO_RUN)) {
        // }
        else {
            KN_ERROR("unhandled expression type");
            exit(1);
        }
        DISPATCH(nextOp.code);
    }
    DEF_OPC(OPC_EVAL_SENTENCES)
    {
        KN_DEBUG("OPC_EVAL_SENTENCES");
        // receive a list, treat it as a code block
        nextOp = OpHandler_EVAL_SENTENCES(kana, curCont);
        DISPATCH(nextOp.code);
    }
    DEF_OPC(OPC_EVAL_LIST_SENTENCE)
    {
        KN_DEBUG("OPC_EVAL_LIST_SENTENCE");
        // receive a list, treat it as a expression
        nextOp = OpHandler_EVAL_LIST_SENTENCE(kana, curCont);
        DISPATCH(nextOp.code);
    }
    DEF_OPC(OPC_EVAL_CELL_SENTENCE)
    {
        // receive a cell, treat it as a expression
        nextOp = OpHandler_EVAL_CELL_SENTENCE(kana, curCont);
        DISPATCH(nextOp.code);
    }
    DEF_OPC(OPC_EVAL_CELL_CLAUSE)
    {
        DISPATCH(nextOp.code);
    }
    DEF_OPC(OPC_EVAL_CLAUSE_CORE)
    {
        DISPATCH(nextOp.code);
    }
    DEF_OPC(OPC_EVAL_CLAUSE_ARGS)
    {
        DISPATCH(nextOp.code);
    }
    
    DEF_OPC(OPC_LOAD_CONT_RUN_NEXT_NODE)
    {
        KN_DEBUG("OPC_LOAD_CONT_RUN_NEXT_NODE");
        // append cont chain
        curCont = KN_2_KON(GS_NEW_CONT, Continuation);
        curEnv = curCont->env;
        nextOp.code = OPC_EVAL_EXPR_BY_TYPE;
        DISPATCH(nextOp.code);
    }
    DEF_OPC(OPC_RUN_NEXT_CONT)
    {
        KN_DEBUG("OPC_RUN_NEXT_CONT");
        // pop cont chain
        curCont = curCont->next;
        curEnv = curCont->env;
        KonContFuncRef contHander = curCont->contHandler;
        nextOp = contHander(kana, curCont);
        DISPATCH(nextOp.code);
    }
    DEF_OPC(OPC_RUN_CURRENT_CONT)
    {
        // no need to modify cont chain
        KonContFuncRef contHander = curCont->contHandler;
        nextOp = contHander(kana, curCont);

        KN_DEBUG("OPC_RUN_CURRENT_CONT next code %d", nextOp.code);

        DISPATCH(nextOp.code);
    }

    DEF_OPC(OPC_ENV_LOOKUP)
    {
        KN_DEBUG("OPC_ENV_LOOKUP");
        KN val = KN_EnvLookup(kana, curEnv, KN_SymbolToCstr(GS_NODE_TO_RUN));
        assert(val.asU64 != KNBOX_UNDEF);
        GS_LAST_VAL = val;
        nextOp.code = OPC_RUN_CURRENT_CONT;
        DISPATCH(nextOp.code);
    }
    DEF_OPC(OPC_APPLY_PROCEDURE)
    {
        nextOp = ApplyProcedureArguments(
            kana, KN_2_KON(GS_PROCEDURE_FUNC, Procedure), 
            GS_PROCEDURE_ARGS,
            curEnv, curCont->next
        );

        KN_DEBUG("OPC_APPLY_PROCEDURE next code %d", nextOp.code);
        DISPATCH(nextOp.code);
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
}