#ifndef kn_interp_op_handlers_h
#define kn_interp_op_handlers_h 1

#include "prefix.h"
#include "env.h"
#include "opcodes.h"

/* Two levels of macros are needed to correctly produce the label
 * from the OPC_xxx macro passed into DEF_OPC as cpp doesn't 
 * prescan when concatenating with ##...
 *
 * On gcc <= 2.95, we also get a space inserted before the :
 * e.g DEF_OPC(OPC_NULL) -> opc0 : - the ##: is a hack to fix
 * this, but this generates warnings on >= 2.96...
 */
#if (__GNUC__ == 2) && (__GNUC_MINOR__ <= 95)
#define label(x)         \
opc##x##:
#else
#define label(x)         \
opc##x:
#endif

// #define DEF_OPC(opcode, BODY)            \
//     label(opcode)                 \
//         BODY
#define DEF_OPC(opcode)  \
            label(opcode)

#define DISPATCH(opcode)     \
    goto *opLabels[opcode];

// usage:
// CASE_OP(ADD, (kana, env, &regLastVal, codeBlock[pc]))
#define CASE_OP(name, args) case OPC_##name: {       \
    opHandlers[OPC_##name]args;                      \
    pc++;                                           \
    break;                                          \
}

#define GS_LAST_VAL     (kana->knRegs[0])
#define GS_NODE_TO_RUN    (kana->knRegs[1])
#define GS_NEW_CONT    (kana->knRegs[2])
#define GS_FRAME    (kana->knRegs[3])
#define GS_CELL_SUBJ    (kana->knRegs[4])
#define GS_PATH_CHAIN    (kana->knRegs[5])  // a::b a\b\c a.inner.core a<i><j>
#define GS_PROCEDURE_FUNC    (kana->knRegs[6])
#define GS_PROCEDURE_ARGS    (kana->knRegs[7])
#define GS_PROCEDURE_BLOCK    (kana->knRegs[8])

KnOp ContHandler_Return(Kana* kana, KonContinuation* curCont);
KnOp ContHandler_Sentences(Kana* kana, KonContinuation* curCont);
KnOp ContHandler_ListSentence(Kana* kana, KonContinuation* curCont);
KnOp ContHandler_CellSentence(Kana* kana, KonContinuation* curCont);
KnOp ContHandler_CellClause(Kana* kana, KonContinuation* curCont);
KnOp ContHandler_ClauseCore(Kana* kana, KonContinuation* curCont);
KnOp ContHandler_ClauseArgs(Kana* kana, KonContinuation* curCont);

KnOp OpHandler_HELLOWORLD(Kana* kana, KonContinuation* curCont);

KnOp OpHandler_EVAL_SENTENCES(Kana* kana, KonContinuation* curCont);
KnOp OpHandler_EVAL_LIST_SENTENCE(Kana* kana, KonContinuation* curCont);
KnOp OpHandler_EVAL_CELL_SENTENCE(Kana* kana, KonContinuation* curCont);


#endif