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
        KonContinuation* newCont = AllocContinuationWithType(kana, KN_CONT_NATIVE_HANDLER, curEnv, curCont);
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


enum ContStatusListSentence {
    CONT_STATUS_LIST_SENTENCE_WAIT_VERB = 0,
    CONT_STATUS_LIST_SENTENCE_WAIT_ARGS
};

void ContHandler_ListSentence(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp;
    const int REG_FUNC = 1;
    const int REG_ARGS = 2;

    // get current state
    int state = curCont->status;
    KN_DEBUG("ContHandler_ListSentence, state %d\n", state);
    switch (state) {
        case CONT_STATUS_LIST_SENTENCE_WAIT_VERB: {
            KN_DEBUG("ContHandler_ListSentence, before box new state\n");
            // verb eval finished
            curCont->status = CONT_STATUS_LIST_SENTENCE_WAIT_ARGS;
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
                KonContinuation* newCont = AllocContinuationWithType(kana, KN_CONT_NATIVE_HANDLER, curCont->env, curCont);
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
        case CONT_STATUS_LIST_SENTENCE_WAIT_ARGS: {
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
    KonContinuation* newCont = AllocContinuationWithType(kana, KN_CONT_NATIVE_HANDLER, curEnv, curCont);
    newCont->contHandler = ContHandler_ListSentence;

    KnList* pendingList = newCont->pendingJobs;
    KnList* finishedList = newCont->finishedJobs;

    KnList_Push(pendingList, KN_CDR(GS_NODE_TO_RUN).asU64);

    GS_NEW_CONT = KON_2_KN(newCont);
    GS_NODE_TO_RUN = KN_CAR(GS_NODE_TO_RUN);
    nextOp.code = OPC_LOAD_CONT_RUN_NEXT_NODE;

    GS_NEXT_OP = nextOp;
}

bool KN_IsKeyword(KN word) {
    if (!(KN_CHECK_TAG(word, KN_T_SYMBOL))) {
        return false;
    }
    int type = KN_FIELD(word, Symbol, type);
    if (type != KN_SYM_WORD) {
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


void ContHandler_ClauseCore(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp = { .code = OPC_NOP };
    GS_NEXT_OP = nextOp;
}

enum ContStatusCellClause {
    CONT_STATUS_CELL_CLAUSE_PREPARE_SUBJ = 0,
    CONT_STATUS_CELL_CLAUSE_SUBJ_EVALED_NEXT_TABLE,
    CONT_STATUS_CELL_CLAUSE_TABLE_EVALED_NEXT_MAP,
    CONT_STATUS_CELL_CLAUSE_ALL_EVALED
};

void ContHandler_CellClause(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp;

    // memo
    const int REG_SUBJ = 1;
    const int REG_TABLE = 2;
    
    // get current state
    int state = curCont->status;
    KN_DEBUG("ContHandler_CellClause, state %d\n", state);
    switch (state) {
        case CONT_STATUS_CELL_CLAUSE_SUBJ_EVALED_NEXT_TABLE: {
            KN firstCell = (KN)KnList_Head(curCont->pendingJobs);
            KonTable* table = KN_FIELD(firstCell, Cell, table);
            curCont->status = CONT_STATUS_CELL_CLAUSE_TABLE_EVALED_NEXT_MAP;
            if (table != KNBOX_UNDEF && KnHashTable_Length(table->table) > 0) {
                KnHashTable* clauseCellTable = table->table;
                KN_DEBUG("ContHandler_CellClause, spawn new continuation to eval arg table\n");
                KonContinuation* evalTableCont = AllocContinuationWithType(kana, KN_CONT_NATIVE_HANDLER, curCont->env, curCont);
                evalTableCont->contHandler = ContHandler_ClauseArgs;

                KnList* pendingList = evalTableCont->pendingJobs;

                KnHashTableIter iter = KnHashTable_IterHead(clauseCellTable);
                while (iter != KNBOX_NIL) {
                    KnHashTableIter next = KnHashTable_IterNext(clauseCellTable, iter);

                    KnList_Push(pendingList, KnHashTable_IterGetVal(clauseCellTable, iter));
                    iter = next;
                }
                GS_NEW_CONT = KON_2_KN(evalTableCont);
                GS_NODE_TO_RUN = (KN)KnList_Shift(evalTableCont->pendingJobs);

                KN_DEBUG("ContHandler_CellSentence, fisrt item of arg table\n");
                KN_PrintNodeToStdio(kana, GS_NODE_TO_RUN);
                nextOp.code = OPC_LOAD_CONT_RUN_NEXT_NODE;
                
                
                break;
            }
            else {
                curCont->memo[REG_TABLE] = KN_UNDEF;
                
            }
        }
            
        case CONT_STATUS_CELL_CLAUSE_TABLE_EVALED_NEXT_MAP: {
            curCont->memo[REG_TABLE] = GS_LAST_VAL;
            
            // TODO eval map attributes
            
            curCont->status = CONT_STATUS_CELL_CLAUSE_ALL_EVALED;
        }
        case CONT_STATUS_CELL_CLAUSE_ALL_EVALED: {
            if (curCont->memo[REG_TABLE].asU64 != KNBOX_UNDEF) {
                // TODO find subject handler
                
                GS_PROCEDURE_FUNC = curCont->memo[REG_SUBJ];
                // TODO actually memo[REG_TABLE] is a list
                GS_PROCEDURE_ARGS = curCont->memo[REG_TABLE];
                GS_PROCEDURE_BLOCK = KN_NIL;

                nextOp.code = OPC_APPLY_PROCEDURE;
            }
            else {
                // TODO
                KN_DEBUG("state ALL_EVALED, balabala labalaba\n");
                exit(1);
            }
        }
    }
    
    GS_NEXT_OP = nextOp;
}

enum ContStatusCellSentence {
    CONT_STATUS_CELL_SENTENCE_WAIT_SUBJ = 0,
    CONT_STATUS_CELL_SENTENCE_DO_CLAUSE,
};

// run clauses until rest jobs is nil
void ContHandler_CellSentence(Kana* kana, KonContinuation* curCont)
{
    KnOp nextOp;

    const int REG_SUBJ = 1;

    // get current state
    int state = curCont->status;
    KN_DEBUG("ContHandler_CellSentence, state %d\n", state);
    switch (state) {
        case CONT_STATUS_CELL_SENTENCE_WAIT_SUBJ: {
            KN_DEBUG("ContHandler_CellSentence, before box new state\n");
            // verb eval finished
            curCont->status = CONT_STATUS_CELL_SENTENCE_DO_CLAUSE;
            curCont->memo[REG_SUBJ] = GS_LAST_VAL;
            KN_DEBUG("ContHandler_CellSentence, before shift args\n");
            int clauseRestLen = KnList_Length(curCont->pendingJobs);
            if (clauseRestLen == 0) {
                KN_DEBUG("ContHandler_CellSentence, WAIT_SUBJ all clauses finished\n");
                nextOp.code = OPC_RUN_NEXT_CONT;
                break;
            }
        }
        case CONT_STATUS_CELL_SENTENCE_DO_CLAUSE: {
            int clauseRestLen = KnList_Length(curCont->pendingJobs);
            if (clauseRestLen == 0) {
                KN_DEBUG("ContHandler_CellSentence, PREPARE_CLAUSE all clauses finished\n");
                nextOp.code = OPC_RUN_NEXT_CONT;
                break;
            }

            // 1 if is a keyword or a marcro
            KN firstCell = (KN)KnList_Shift(curCont->pendingJobs);
            KN subj = KN_DCR(firstCell);
            // is a keyword or a prefix marcro?
            if (subj.asU64 != KNBOX_UNDEF && (KN_IsKeyword(subj) || KN_IS_SYM_MARCRO(subj))) {
                // TODO dispatch keyword/marcro handler
                return;
            }

            // 2 normal message
            
            KonContinuation* newCont = AllocContinuationWithType(kana, KN_CONT_NATIVE_HANDLER, curCont->env, curCont);
            newCont->contHandler = ContHandler_CellClause;
            newCont->status = CONT_STATUS_CELL_CLAUSE_SUBJ_EVALED_NEXT_TABLE;   // eval table
            newCont->memo[1] = curCont->memo[REG_SUBJ];

            KnList* pendingList = newCont->pendingJobs;

            KnList_Push(pendingList, firstCell.asU64);
            
            GS_NEW_CONT = KON_2_KN(newCont);
            
            nextOp.code = OPC_LOAD_CONT;

            break;
        }
    }
    KN_DEBUG("ContHandler_CellSentence, before return\n");

    GS_NEXT_OP = nextOp;
}

void OpHandler_EVAL_CELL_SENTENCE(Kana* kana, KonContinuation* curCont)
{
    // get subject
    KN subject = KN_DCR(GS_NODE_TO_RUN);
    // is a keyword or a prefix marcro?
    if (KN_IsKeyword(subject) || KN_IS_SYM_MARCRO(subject)) {
        // TODO dispatch keyword/marcro handler
        return;
    }

    KnOp nextOp;
    KonEnv* curEnv = curCont->env;
    KonContinuation* newCont = AllocContinuationWithType(kana, KN_CONT_NATIVE_HANDLER, curEnv, curCont);
    newCont->contHandler = ContHandler_CellSentence;

    KnList* pendingList = newCont->pendingJobs;
    KnList* finishedList = newCont->finishedJobs;

    // 1 save last val to memo
    newCont->memo[1] = GS_NODE_TO_RUN;

    // 2 split clauses
    // 2.1 the first cell
    KonCell* firstCell = KN_2_KON(GS_NODE_TO_RUN, Cell);
    
    if (firstCell->list != KNBOX_UNDEF || firstCell->table != KNBOX_UNDEF
        || firstCell->vector != KNBOX_UNDEF || firstCell->map != KNBOX_UNDEF
    ) {
        KonCell* firstClause = KN_NEW_DYNAMIC_OBJ(kana, KonCell, KN_T_CELL);
        firstClause->list = firstCell->list;
        firstClause->table = firstCell->table;
        firstClause->map = firstCell->map;
        firstClause->vector = firstCell->vector;
        firstClause->core = KN_UNDEF;
        firstClause->next = KNBOX_NIL;
        KnList_Push(pendingList, firstClause);
    }
    // 2.2 next cells
    KonCell* iterCell = firstCell->next;
    while (iterCell != KNBOX_NIL) {
        KonCell* next = iterCell->next;
        KN iterCellSubj = iterCell->core;
        // is a keyword or a prefix marcro?
        if (KN_IsKeyword(iterCellSubj) || KN_IS_SYM_MARCRO(iterCellSubj)) {
            // TODO get clause type and length
            // for (int i = 0; i < n; i++) {

            // }
            // continue;
        }
        KonCell* clause = KN_NEW_DYNAMIC_OBJ(kana, KonCell, KN_T_CELL);
        clause->list = iterCell->list;
        clause->table = iterCell->table;
        clause->map = iterCell->map;
        clause->vector = iterCell->vector;
        clause->core = KN_UNDEF;
        clause->next = KNBOX_NIL;
        KnList_Push(pendingList, clause);
        iterCell = next;
    }


    GS_NEW_CONT = KON_2_KN(newCont);
    GS_NODE_TO_RUN = subject;
    nextOp.code = OPC_LOAD_CONT_RUN_NEXT_NODE;
    GS_NEXT_OP = nextOp;
}

