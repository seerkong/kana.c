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

    // no need to store result of each sentence
    // KxList_Push(finishedList, GS_LAST_VAL.asU64);

    if (KxList_Length(pendingList) == 0) {
        nextOp.code = OPC_RUN_NEXT_CONT;
    }
    else {
        KN nextJob = (KN)KxList_Shift(pendingList);
        printf("next job");
        KN_PrintNodeToStdio(knstate, nextJob);
        GS_NODE_TO_RUN = nextJob;
        nextOp.code = OPC_LOAD_CONT_RUN_NEXT_NODE;
    }
    return nextOp;
}

KN_OP ContHandler_ListSentence(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs)
{
    KN_OP nextOp;
    const int WAIT_VERB = 0;
    const int WAIT_ARGS = 1;

    const int CONT_STATE = 0;
    const int REG_FUNC = 1;
    const int REG_ARGS = 2;

    // get current state
    int state = KN_UNBOX_INT(curCont->memo[CONT_STATE]);
    printf("ContHandler_ListSentence, state %d\n", state);
    switch (state) {
        case WAIT_VERB: {
            printf("ContHandler_ListSentence, before box new state\n");
            // verb eval finished
            CS_MEMO(CONT_STATE) = KN_BOX_INT(WAIT_ARGS);
            CS_MEMO(REG_FUNC) = GS_LAST_VAL;
            printf("ContHandler_ListSentence, before shift args\n");
            KN args = (KN)KxList_Shift(curCont->pendingJobs);

            if (args.asU64 == KNBOX_NIL) {
                printf("ContHandler_ListSentence, no args\n");
                // no args, apply
                GS_PROCEDURE_FUNC = CS_MEMO(REG_FUNC);
                GS_PROCEDURE_ARGS = KN_NIL;
                GS_PROCEDURE_BLOCK = KN_NIL;

                nextOp.code = OPC_APPLY_PROCEDURE;

            }
            else {
                printf("ContHandler_ListSentence, spawn new continuation\n");
                KonContinuation* newCont = AllocContinuationWithType(knstate, KN_CONT_SENTENCES, curCont->env, curCont);
                newCont->contHandler = ContHandler_ClauseArgs;

                KxList* pendingList = newCont->pendingJobs;

                // add args to pending queue
                KN iter = KN_CDR(args);;
                
                while (iter.asU64 != KNBOX_NIL && KN_IS_PAIR(iter)) {
                    KN item = KN_CAR(iter);
                    KN next = KN_CDR(iter);
                    KxList_Push(pendingList, item.asU64);
                    iter = next;
                }

                printf("ContHandler_ListSentence, after make pending list\n");

                GS_NEW_CONT = KON_2_KN(newCont);
                GS_NODE_TO_RUN = KN_CAR(args);

                KN_PrintNodeToStdio(knstate, GS_NODE_TO_RUN);
                nextOp.code = OPC_LOAD_CONT_RUN_NEXT_NODE;
                break;
            }
        }
        case WAIT_ARGS: {
            // args eval finished
            GS_PROCEDURE_FUNC = CS_MEMO(REG_FUNC);
            GS_PROCEDURE_ARGS = GS_LAST_VAL;
            GS_PROCEDURE_BLOCK = KN_NIL;

            nextOp.code = OPC_APPLY_PROCEDURE;
        }
    }
    printf("ContHandler_ListSentence, before return\n");

    return nextOp;
}

// run clauses until rest jobs is nil
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
    // save 
    return nextOp;
}

KN_OP ContHandler_ClauseArgs(KonState* knstate, KonContinuation* curCont, KN* globalKonRegs)
{
    KN_OP nextOp;
    KxList* pendingList = curCont->pendingJobs;
    KxList* finishedList = curCont->finishedJobs;

    KxList_Push(finishedList, GS_LAST_VAL.asU64);

    if (KxList_Length(pendingList) == 0) {
        // finished list to KN arg list
        KN result = KN_NIL;
        KxListNode* iter = KxList_IterTail(finishedList);
        while ((int64_t)iter != KNBOX_NIL) {
            KxListNode* next = KxList_IterPrev(iter);

            KN arg = (KN)KxList_IterVal(iter);
            // KN_PrintNodeToStdio(knstate, arg);
            result = KN_CONS(knstate, arg, result);
            iter = next;
        }
        GS_LAST_VAL = result;
        nextOp.code = OPC_RUN_NEXT_CONT;
    }
    else {
        KN nextJob = (KN)KxList_Shift(pendingList);
        printf("next job");
        KN_PrintNodeToStdio(knstate, nextJob);
        GS_NODE_TO_RUN = nextJob;
        nextOp.code = OPC_LOAD_CONT_RUN_NEXT_NODE;
    }
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
    KonContinuation* newCont = AllocContinuationWithType(knstate, KN_CONT_SENTENCES, curEnv, curCont);
    newCont->contHandler = ContHandler_ListSentence;

    KxList* pendingList = newCont->pendingJobs;
    KxList* finishedList = newCont->pendingJobs;

    newCont->memo[0] = KN_BOX_INT(0);
    KxList_Push(pendingList, KN_CDR(GS_NODE_TO_RUN).asU64);

    GS_NEW_CONT = KON_2_KN(newCont);
    GS_NODE_TO_RUN = KN_CAR(GS_NODE_TO_RUN);
    nextOp->code = OPC_LOAD_CONT_RUN_NEXT_NODE;



    // // create two continuation
    // KonContinuation* argsCont = AllocContinuationWithType(knstate, KN_CONT_SENTENCES, curEnv, curCont);
    // argsCont->contHandler = ContHandler_ClauseArgs;
    // KonContinuation* verbCont = AllocContinuationWithType(knstate, KN_CONT_SENTENCES, curEnv, argsCont);
    // verbCont->contHandler = ContHandler_ClauseCore;

    // KxList* pendingList = argsCont->pendingJobs;

    // // add centences to pending queue
    // KN iter = KN_CDR(GS_NODE_TO_RUN);

    // while (iter.asU64 != KNBOX_NIL && KN_IS_PAIR(iter)) {
    //     KN item = KN_CAR(iter);
    //     KN next = KN_CDR(iter);
    //     KxList_Push(pendingList, item.asU64);
    //     iter = next;
    // }

    // GS_NEW_CONT = KON_2_KN(verbCont);
    // GS_NODE_TO_RUN = KN_CAR(GS_NODE_TO_RUN);
    // nextOp->code = OPC_LOAD_CONT_RUN_NEXT_NODE;
}
void OpHandler_EVAL_CELL_SENTENCE(KonState* knstate, KonEnv* curEnv, KonContinuation* curCont, KN* globalKonRegs, KN_OP* nextOp)
{

}

