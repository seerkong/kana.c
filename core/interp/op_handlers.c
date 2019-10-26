#include "op_handlers.h"

void OpHandler_NONE(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp)
{
}

void OpHandler_HELLOWORLD(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp)
{
    GS_LAST_VAL = KN_BOX_INT(5);
    printf("in OpHandler_HELLOWORLD\n");
    nextOp->code = OPC_NOP;
}
