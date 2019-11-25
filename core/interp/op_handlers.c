#include "op_handlers.h"

void ContHandler_Return(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp = { .code = OPC_LAND };
    GS_NEXT_OP = nextOp;
}

void ContHandler_Sentences(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp;
    KnList* pendingList = curCont->pendingJobs;
    KnList* finishedList = curCont->finishedJobs;

    // no need to store result of each sentence
    // KnList_Push(finishedList, GS_LAST_VAL.asU64);

    if (KnList_Length(pendingList) == 0) {
        nextOp.code = OPC_RUN_NEXT_CONT;
    }
    else {
        KN nextJob = (KN)KnList_Shift(pendingList);
        KN_DEBUG("next job");
        KN_PrintNodeToStdio(kana, nextJob);
        GS_NODE_TO_RUN = nextJob;
        nextOp.code = OPC_LOAD_CONT_RUN_NEXT_NODE;
    }
    GS_NEXT_OP = nextOp;
}


// run clauses until rest jobs is nil
void ContHandler_CellSentence(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp = { .code = OPC_NOP };
    GS_NEXT_OP = nextOp;
}

void ContHandler_CellClause(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp = { .code = OPC_NOP };
    GS_NEXT_OP = nextOp;
}

void ContHandler_ClauseCore(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp = { .code = OPC_NOP };
    GS_NEXT_OP = nextOp;
}

void ContHandler_ClauseArgs(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp;
    KnList* pendingList = curCont->pendingJobs;
    KnList* finishedList = curCont->finishedJobs;

    KnList_Push(finishedList, GS_LAST_VAL.asU64);

    if (KnList_Length(pendingList) == 0) {
        // finished list to KN arg list
        KN result = KN_NIL;
        KnListNode* iter = KnList_IterTail(finishedList);
        while ((int64_t)iter != KNBOX_NIL) {
            KnListNode* next = KnList_IterPrev(iter);

            KN arg = (KN)KnList_IterVal(iter);
            // KN_PrintNodeToStdio(kana, arg);
            result = KN_CONS(kana, arg, result);
            iter = next;
        }
        GS_LAST_VAL = result;
        nextOp.code = OPC_RUN_NEXT_CONT;
    }
    else {
        KN nextJob = (KN)KnList_Shift(pendingList);
        KN_DEBUG("next job");
        KN_PrintNodeToStdio(kana, nextJob);
        GS_NODE_TO_RUN = nextJob;
        nextOp.code = OPC_LOAD_CONT_RUN_NEXT_NODE;
    }
    GS_NEXT_OP = nextOp;
}


void OpHandler_HELLOWORLD(Kana* kana, KonContinuation* curCont)
{
    GS_LAST_VAL = KN_BOX_INT(5);
    printf("in OpHandler_HELLOWORLD\n");
    KnOp nextOp = { .code = OPC_LAND };
    GS_NEXT_OP = nextOp;
}

void OpHandler_EVAL_SENTENCES(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp;
    KonEnv* curEnv = curCont->env;
    if (GS_NODE_TO_RUN.asU64 == KNBOX_NIL) {
        nextOp.code = OPC_RUN_NEXT_CONT;
    }
    else {
        KonContinuation* newCont = AllocContinuationWithType(kana, KN_CONT_SENTENCES, curEnv, curCont);
        newCont->contHandler = ContHandler_Sentences;
        KnList* pendingList = newCont->pendingJobs;

        // add centences to pending queue
        KN iter = KN_CDR(GS_NODE_TO_RUN);

        while (iter.asU64 != KNBOX_NIL && KN_IS_PAIR(iter)) {
            KN item = KN_CAR(iter);
            KN next = KN_CDR(iter);
            KnList_Push(pendingList, item.asU64);
            iter = next;
        }
        GS_NEW_CONT = KON_2_KN(newCont);
        GS_NODE_TO_RUN = KN_CAR(GS_NODE_TO_RUN);
        nextOp.code = OPC_LOAD_CONT_RUN_NEXT_NODE;
    }
    GS_NEXT_OP = nextOp;
}


void ContHandler_ListSentence(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp;
    const int WAIT_VERB = 0;
    const int WAIT_ARGS = 1;

    const int CONT_STATE = 0;
    const int REG_FUNC = 1;
    const int REG_ARGS = 2;

    // get current state
    int state = KN_UNBOX_INT(curCont->memo[CONT_STATE]);
    KN_DEBUG("ContHandler_ListSentence, state %d\n", state);
    switch (state) {
        case WAIT_VERB: {
            KN_DEBUG("ContHandler_ListSentence, before box new state\n");
            // verb eval finished
            curCont->memo[CONT_STATE] = KN_BOX_INT(WAIT_ARGS);
            curCont->memo[REG_FUNC] = GS_LAST_VAL;
            KN_DEBUG("ContHandler_ListSentence, before shift args\n");
            KN args = (KN)KnList_Shift(curCont->pendingJobs);

            if (args.asU64 == KNBOX_NIL) {
                KN_DEBUG("ContHandler_ListSentence, no args\n");
                // no args, apply
                GS_PROCEDURE_FUNC = curCont->memo[REG_FUNC];
                GS_PROCEDURE_ARGS = KN_NIL;
                GS_PROCEDURE_BLOCK = KN_NIL;

                nextOp.code = OPC_APPLY_PROCEDURE;

            }
            else {
                KN_DEBUG("ContHandler_ListSentence, spawn new continuation\n");
                KonContinuation* newCont = AllocContinuationWithType(kana, KN_CONT_SENTENCES, curCont->env, curCont);
                newCont->contHandler = ContHandler_ClauseArgs;

                KnList* pendingList = newCont->pendingJobs;

                // add args to pending queue
                KN iter = KN_CDR(args);;
                
                while (iter.asU64 != KNBOX_NIL && KN_IS_PAIR(iter)) {
                    KN item = KN_CAR(iter);
                    KN next = KN_CDR(iter);
                    KnList_Push(pendingList, item.asU64);
                    iter = next;
                }

                KN_DEBUG("ContHandler_ListSentence, after make pending list\n");

                GS_NEW_CONT = KON_2_KN(newCont);
                GS_NODE_TO_RUN = KN_CAR(args);

                KN_PrintNodeToStdio(kana, GS_NODE_TO_RUN);
                nextOp.code = OPC_LOAD_CONT_RUN_NEXT_NODE;
                break;
            }
        }
        case WAIT_ARGS: {
            // args eval finished
            GS_PROCEDURE_FUNC = curCont->memo[REG_FUNC];
            GS_PROCEDURE_ARGS = GS_LAST_VAL;
            GS_PROCEDURE_BLOCK = KN_NIL;

            nextOp.code = OPC_APPLY_PROCEDURE;
        }
    }
    KN_DEBUG("ContHandler_ListSentence, before return\n");

    GS_NEXT_OP = nextOp;
}


void OpHandler_EVAL_LIST_SENTENCE(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp;
    KonEnv* curEnv = curCont->env;
    KonContinuation* newCont = AllocContinuationWithType(kana, KN_CONT_SENTENCES, curEnv, curCont);
    newCont->contHandler = ContHandler_ListSentence;

    KnList* pendingList = newCont->pendingJobs;
    KnList* finishedList = newCont->pendingJobs;

    newCont->memo[0] = KN_BOX_INT(0);
    KnList_Push(pendingList, KN_CDR(GS_NODE_TO_RUN).asU64);

    GS_NEW_CONT = KON_2_KN(newCont);
    GS_NODE_TO_RUN = KN_CAR(GS_NODE_TO_RUN);
    nextOp.code = OPC_LOAD_CONT_RUN_NEXT_NODE;



    // // create two continuation
    // KonContinuation* argsCont = AllocContinuationWithType(kana, KN_CONT_SENTENCES, curEnv, curCont);
    // argsCont->contHandler = ContHandler_ClauseArgs;
    // KonContinuation* verbCont = AllocContinuationWithType(kana, KN_CONT_SENTENCES, curEnv, argsCont);
    // verbCont->contHandler = ContHandler_ClauseCore;

    // KnList* pendingList = argsCont->pendingJobs;

    // // add centences to pending queue
    // KN iter = KN_CDR(GS_NODE_TO_RUN);

    // while (iter.asU64 != KNBOX_NIL && KN_IS_PAIR(iter)) {
    //     KN item = KN_CAR(iter);
    //     KN next = KN_CDR(iter);
    //     KnList_Push(pendingList, item.asU64);
    //     iter = next;
    // }

    // GS_NEW_CONT = KON_2_KN(verbCont);
    // GS_NODE_TO_RUN = KN_CAR(GS_NODE_TO_RUN);
    // nextOp->code = OPC_LOAD_CONT_RUN_NEXT_NODE;
    GS_NEXT_OP = nextOp;
}

void OpHandler_EVAL_CELL_SENTENCE(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp = { .code = OPC_NOP };
    KonEnv* curEnv = curCont->env;
    GS_NEXT_OP = nextOp;
}

