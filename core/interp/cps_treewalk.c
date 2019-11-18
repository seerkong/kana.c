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

            // DefineReservedDispatcher(kana, env);

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

void ApplyProcedureArguments(Kana* kana, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* curCont)
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
    GS_NEXT_OP = nextOp;
}

void ContHandler_QuitBounceMode(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp = { .code = OPC_LAND };
    GS_NEXT_OP = nextOp;
}

KN KN_ExecAst(Kana* kana, KN sourceCodeAst, KonEnv* rootEnv)
{
    // TODO add step count when debug
    KN formatedCode = KN_ToFormatString(kana, sourceCodeAst, true, 0, "  ");
    KN_DEBUG("%s", KN_StringToCstr(formatedCode));

    KonEnv* curEnv = rootEnv;
    KonContinuation* curCont = AllocContinuationWithType(kana, KN_CONT_RETURN, curEnv, NULL);
    curCont->contHandler = ContHandler_Return;

    int dummyPc = 0;    // used in bounce mode

    KN* astNodePool = (KN*)calloc(10, sizeof(KN));
    astNodePool[0] = sourceCodeAst;

    KnOp* NEXT_OP_REF = &(kana->NEXT_OP);
    
    // bytecode to enter bounce mode
    KnOp initCodeSeg[] = {
        {.code = OPC_SET_KN_REG_FROM_AST, .two = {.a = REG_NODE_TO_RUN, .b = 0} },
        {.code = OPC_SET_NEXT_OP_CODE, .one = {.a = OPC_EVAL_SENTENCES} },
        {.code = OPC_DISPATCH_OP_CODE, .one = {.a = OPC_ENTER_BOUNCE_MODE} },
    };


    KnFrame* rootFrame = (KnFrame*)tb_allocator_malloc0(kana->dynamicAllocator, sizeof(KnFrame));
    rootFrame->prev = NULL;
    rootFrame->curCont = curCont;
    rootFrame->curEnv = curEnv;
    rootFrame->codeSegment = initCodeSeg;
    rootFrame->pc = 0;
    rootFrame->ir = rootFrame->codeSegment;

    // init global register
    GS_FRAME = rootFrame;
    GS_PC = &(GS_FRAME->pc);
    GS_IR = &(GS_FRAME->ir);
    GS_CS = GS_FRAME->codeSegment;

    GS_NODE_TO_RUN = sourceCodeAst;
    GS_LAST_VAL = KN_UKN;


    static const void* opLabels[] = {
        // used in bounce-trampoline mode
        &&opc0, &&opc1, &&opc2, &&opc3, &&opc4, &&opc5, &&opc6, &&opc7, &&opc8, &&opc9, &&opc10,
        &&opc11, &&opc12, &&opc13, &&opc14, &&opc15, &&opc16, &&opc17, &&opc18, &&opc19, &&opc20,
        &&opc21, &&opc22, &&opc23, &&opc24, &&opc25, &&opc26, &&opc27, &&opc28, &&opc29, &&opc30,
        
        // used in kon-array mode
        &&opc31, &&opc32, &&opc33, &&opc34, &&opc35, &&opc36, &&opc37, &&opc38, &&opc39, &&opc40,
        &&opc41, &&opc42, &&opc43, &&opc44, &&opc45, &&opc46, &&opc47, &&opc48, &&opc49, &&opc50,
        &&opc51, &&opc52, &&opc53, &&opc54, &&opc55, &&opc56, &&opc57, &&opc58, &&opc59, &&opc60,

        // used in bytecode mode
        &&opc61, &&opc62, &&opc63, &&opc64, &&opc65, &&opc66, &&opc67, //&&opc68, &&opc69, &&opc70,
        // &&opc71, &&opc72, &&opc73, &&opc74, &&opc75, &&opc76, &&opc77, &&opc78, &&opc79, &&opc80,
        // &&opc81, &&opc82, &&opc83, &&opc84, &&opc85, &&opc86, &&opc87, &&opc88, &&opc89, &&opc90,
        // &&opc91, &&opc92, &&opc93, &&opc94, &&opc95, &&opc96, &&opc97, &&opc98, &&opc99, &&opc100,
        // &&opc101, &&opc102, &&opc103, &&opc104, &&opc105, &&opc106, &&opc107, &&opc108, &&opc109,
        // &&opc110, &&opc111, &&opc112, &&opc113, &&opc114, &&opc115, &&opc116, &&opc117, &&opc118,
        // &&opc119, &&opc120, &&opc121, &&opc122, &&opc123, &&opc124, &&opc125, &&opc126, &&opc127,
        // &&unused,
    };

    // KnOp initOp = { .code = OPC_EVAL_SENTENCES };
    // GS_NEXT_OP = initOp;
    
    // start in in bounce-trampoline interprete mode
    // DISPATCH(OPC_ENTER_BOUNCE_MODE);
    // DISPATCH(OPC_HELLOWORLD);

    
    
    DISPATCH((*GS_IR)->code);

    DEF_OPC(OPC_HELLOWORLD)
    {
        KN_DEBUG("HELLO OP EXECUTED");
        GS_IR = NEXT_OP_REF;
        OpHandler_HELLOWORLD(kana, curCont);
        DISPATCH((*GS_IR)->code);
    }

    DEF_OPC(OPC_ENTER_BOUNCE_MODE)
    {
        GS_PC = &dummyPc;
        GS_IR = &NEXT_OP_REF;
        KonContinuation* nextCont = AllocContinuationWithType(kana, KN_CONT_QUIT_BOUNCE_MODE, curEnv, curCont);
        nextCont->contHandler = ContHandler_QuitBounceMode;

        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }

    DEF_OPC(OPC_ENTER_CODE_SEG_MODE)
    {
        GS_PC = &(GS_FRAME->pc);
        GS_IR = &(GS_FRAME->ir);
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }

    DEF_OPC(OPC_EVAL_EXPR_BY_TYPE)
    {
        KN_DEBUG("OPC_EVAL_EXPR_BY_TYPE");
        if (IsSelfEvaluated(GS_NODE_TO_RUN)) {
            GS_LAST_VAL = GS_NODE_TO_RUN;
            GS_NEXT_OP.code = OPC_RUN_CURRENT_CONT;
        }
        else if (KN_IS_CELL(GS_NODE_TO_RUN)) {
            // create a new cont
            GS_NEXT_OP.code = OPC_EVAL_CELL_SENTENCE;
        }
        else if (KN_IsPairList(GS_NODE_TO_RUN)) {
            // create a new cont
            GS_NEXT_OP.code = OPC_EVAL_LIST_SENTENCE;
        }
        else if (KN_IS_REFERENCE(GS_NODE_TO_RUN)) {
            // a code block like { a }
            // TODO asert should be a SYM_IDENTIFIER
            // env lookup this val
            GS_NEXT_OP.code = OPC_ENV_LOOKUP;
        }
        else if (KN_IS_ACCESSOR(GS_NODE_TO_RUN)) {
            // unbox
            KN val = UnBoxAccessorValue(GS_NODE_TO_RUN);
            GS_LAST_VAL = val;
            GS_NEXT_OP.code = OPC_RUN_CURRENT_CONT;
        }
        // TODO
        // else if (KN_IS_QUASI_PAIR(GS_NODE_TO_RUN)) {
        // }
        else {
            KN_ERROR("unhandled expression type");
            exit(1);
        }
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_EVAL_SENTENCES)
    {
        KN_DEBUG("OPC_EVAL_SENTENCES");
        // receive a list, treat it as a code block
        OpHandler_EVAL_SENTENCES(kana, curCont);
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_EVAL_LIST_SENTENCE)
    {
        KN_DEBUG("OPC_EVAL_LIST_SENTENCE");
        // receive a list, treat it as a expression
        OpHandler_EVAL_LIST_SENTENCE(kana, curCont);
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_EVAL_CELL_SENTENCE)
    {
        // receive a cell, treat it as a expression
        OpHandler_EVAL_CELL_SENTENCE(kana, curCont);
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_EVAL_CELL_CLAUSE)
    {
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_EVAL_CLAUSE_CORE)
    {
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_EVAL_CLAUSE_ARGS)
    {
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    
    DEF_OPC(OPC_LOAD_CONT_RUN_NEXT_NODE)
    {
        KN_DEBUG("OPC_LOAD_CONT_RUN_NEXT_NODE");
        // append cont chain
        curCont = KN_2_KON(GS_NEW_CONT, Continuation);
        curEnv = curCont->env;
        GS_NEXT_OP.code = OPC_EVAL_EXPR_BY_TYPE;
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_RUN_NEXT_CONT)
    {
        KN_DEBUG("OPC_RUN_NEXT_CONT");
        // pop cont chain
        curCont = curCont->next;
        curEnv = curCont->env;
        KonContFuncRef contHander = curCont->contHandler;
        contHander(kana, curCont);
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_RUN_CURRENT_CONT)
    {
        // no need to modify cont chain
        KonContFuncRef contHander = curCont->contHandler;
        contHander(kana, curCont);

        KN_DEBUG("OPC_RUN_CURRENT_CONT next code %d", GS_NEXT_OP.code);

        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }

    DEF_OPC(OPC_ENV_LOOKUP)
    {
        KN_DEBUG("OPC_ENV_LOOKUP");
        KN val = KN_EnvLookup(kana, curEnv, KN_SymbolToCstr(GS_NODE_TO_RUN));
        assert(val.asU64 != KNBOX_UNDEF);
        GS_LAST_VAL = val;
        GS_NEXT_OP.code = OPC_RUN_CURRENT_CONT;
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_APPLY_PROCEDURE)
    {
        ApplyProcedureArguments(
            kana, KN_2_KON(GS_PROCEDURE_FUNC, Procedure), 
            GS_PROCEDURE_ARGS,
            curEnv, curCont->next
        );

        KN_DEBUG("OPC_APPLY_PROCEDURE next code %d", GS_NEXT_OP.code);
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }

    opc17: opc18: opc19: opc20:
    opc21: opc22: opc23: opc24: opc25: opc26: opc27: opc28: opc29: opc30:
    opc31: opc32: opc33: opc34: opc35: opc36: opc37: opc38: opc39: opc40:
    opc41: opc42: opc43: opc44: opc45: opc46: opc47: opc48: opc49: opc50:
    opc51: opc52: opc53: opc54: opc55: opc56: opc57: opc58: opc59: opc60:
    opc68: opc69: opc70:
    opc71: opc72: opc73: opc74: opc75: opc76: opc77: opc78: opc79: opc80:
    {}

    DEF_OPC(OPC_SET_KN_REG_FROM_AST)
    {
        int astNodeId = (*GS_IR)->two.b;
        GS_NODE_TO_RUN = astNodePool[astNodeId];
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_SET_NEXT_OP_CODE)
    {
        GS_NEXT_OP.code = (*GS_IR)->one.a;
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_SET_NEXT_OP_ONE)
    {
        GS_NEXT_OP.one.a = (*GS_IR)->one.a;
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_SET_NEXT_OP_TWO)
    {
        GS_NEXT_OP.two.a = (*GS_IR)->two.a;
        GS_NEXT_OP.two.b = (*GS_IR)->two.b;
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_SET_NEXT_OP_THREE)
    {
        GS_NEXT_OP.three.a = (*GS_IR)->three.a;
        GS_NEXT_OP.three.b = (*GS_IR)->three.b;
        GS_NEXT_OP.three.c = (*GS_IR)->three.c;
        DISPATCH_NEXT_OP(GS_FRAME, GS_PC, GS_IR);
    }
    DEF_OPC(OPC_DISPATCH_OP_CODE)
    {
        int jumpCode = (*GS_IR)->one.a;
        DISPATCH(jumpCode);
    }
    DEF_OPC(OPC_END_CODE_SEG)
    {
        KN_DEBUG("OPC_END_CODE_SEG");
        DISPATCH(OPC_LAND);
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
