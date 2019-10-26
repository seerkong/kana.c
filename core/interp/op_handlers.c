#include "op_handlers.h"

KN_OP ContHandler_Return(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs)
{
    KN_OP nextOp;
    nextOp.code = OPC_LAND;
    return nextOp;
}

KN_OP ContHandler_Sentences(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs)
{
    KN_OP nextOp;
    KxList* pendingList = curCont->pendingJobs;
    KxList* finishedList = curCont->finishedJobs;

    KxList_Push(finishedList, GS_LAST_VAL.asU64);

    if (KxList_Length(pendingList) == 0) {
        nextOp.code = OPC_RUN_NEXT_CONT;
    }
    else {
        KN nextJob = (KN)KxList_Shift(pendingList);
        GS_NODE_TO_RUN = nextJob;
        nextOp.code = OPC_LOAD_CONT_RUN_NEXT_NODE;
    }
    return nextOp;
}

KN_OP ContHandler_ListSentence(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs)
{
    KN_OP nextOp;
    return nextOp;
}

KN_OP ContHandler_CellSentence(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs)
{
    KN_OP nextOp;
    return nextOp;
}

KN_OP ContHandler_CellClause(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs)
{
    KN_OP nextOp;
    return nextOp;
}

KN_OP ContHandler_ClauseCore(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs)
{
    KN_OP nextOp;
    return nextOp;
}

KN_OP ContHandler_ClauseArgs(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs)
{
    KN_OP nextOp;
    return nextOp;
}




void OpHandler_NONE(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp)
{
}

void OpHandler_HELLOWORLD(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp)
{
    GS_LAST_VAL = KN_BOX_INT(5);
    printf("in OpHandler_HELLOWORLD\n");
    nextOp->code = OPC_NOP;
}

void OpHandler_EVAL_SENTENCES(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp)
{
    if (GS_NODE_TO_RUN.asU64 == KNBOX_NIL) {
        nextOp->code = OPC_RUN_NEXT_CONT;
    }
    else {
        KonContinuation* newCont = AllocContinuationWithType(knstate, KN_CONT_SENTENCES, curEnv, curCont);
        newCont->contHandler = ContHandler_Sentences;
        KxList* pendingList = newCont->pendingJobs;

        // add centences to pending queue
        KN iter = KN_CDR(GS_NODE_TO_RUN);

        while (iter.asU64 != KNBOX_NIL && KN_IS_PAIR(iter)) {
            KN item = KN_CAR(iter);
            KN next = KN_CDR(iter);
            KxList_Push(pendingList, item.asU64);
            iter = next;
        }
        GS_NEW_CONT = KON_2_KN(newCont);
        GS_NODE_TO_RUN = KN_CAR(GS_NODE_TO_RUN);
        nextOp->code = OPC_LOAD_CONT_RUN_NEXT_NODE;
    }
}
void OpHandler_EVAL_LIST_SENTENCE(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp)
{

}
void OpHandler_EVAL_CELL_SENTENCE(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp)
{

}

