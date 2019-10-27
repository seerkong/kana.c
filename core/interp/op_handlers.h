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
// CASE_OP(ADD, (knState, env, &regLastVal, codeBlock[pc]))
#define CASE_OP(name, args) case OPC_##name: {       \
    opHandlers[OPC_##name]args;                      \
    pc++;                                           \
    break;                                          \
}

// TODO move ast cursor
#define HANDLE_OP(name, args)   {opHandlers[OPC_##name]args;}

// TODO knstate global states
// #define GS_OP          ((KN_OP)globalRegs32[0])
#define GS_LAST_VAL     (globalKonRegs[0])
#define GS_NODE_TO_RUN    (globalKonRegs[1])
#define GS_NEW_CONT    (globalKonRegs[2])
#define GS_CELL_SUBJ    (globalKonRegs[3])
#define GS_PROCEDURE_FUNC    (globalKonRegs[4])
#define GS_PROCEDURE_ARGS    (globalKonRegs[5])
#define GS_PROCEDURE_BLOCK    (globalKonRegs[6])

// TODO continuation states
#define CS_PENDING      (curCont->pendingJobs)
#define CS_FINISHED     (curCont->finishedJobs)
// #define CS_MEMO1     (curCont->memo1)
// #define CS_MEMO2     (curCont->memo2)
// #define CS_MEMO3     (curCont->memo3)
#define CS_MEMO(n)     (curCont->memo[n])

typedef void (*OpHandlerRef)(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp);

KN_OP ContHandler_Return(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs);
KN_OP ContHandler_Sentences(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs);
KN_OP ContHandler_ListSentence(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs);
KN_OP ContHandler_CellSentence(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs);
KN_OP ContHandler_CellClause(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs);
KN_OP ContHandler_ClauseCore(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs);
KN_OP ContHandler_ClauseArgs(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs);

void OpHandler_NONE(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp);
void OpHandler_HELLOWORLD(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp);

void OpHandler_EVAL_SENTENCES(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp);
void OpHandler_EVAL_LIST_SENTENCE(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp);
void OpHandler_EVAL_CELL_SENTENCE(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp);


#endif