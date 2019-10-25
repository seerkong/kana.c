#include "kikumaru.h"

void KN_MarkPhase(KonState* knState);
void KN_Mark(KonState* knState, KxList* taskQueue, char color);
void KN_SweepPhase();
void KN_ResetAndCopyPtrSegList(KonState* knState);

void KN_ShowGcStatics(KonState* knState);

void KN_RecordNewKonNode(KonState* knState, KN newVal);

void KN_MarkNode(struct _KonBase* item, KxList* markTaskQueue, char color);
void KN_DestroyNode(KonState* knState, struct _KonBase* item);

void KN_InitGc(KonState* knState)
{
    knState->largeConstAllocator = tb_large_allocator_init(tb_null, 0);
    knState->constAllocator = tb_default_allocator_init(knState->largeConstAllocator);
    knState->largeAllocator = tb_large_allocator_init(tb_null, 0);
    knState->dynamicAllocator = tb_default_allocator_init(knState->largeAllocator);
    
    if (!tb_init(tb_null, knState->dynamicAllocator)) {
        printf("tb_init failed\n");
        return;
    }


}


void KN_DestroyGc(KonState* knState)
{
    // exit allocator
    if (knState->constAllocator) {
        tb_allocator_exit(knState->constAllocator);
    }
    knState->constAllocator = tb_null;

    if (knState->dynamicAllocator) {
        tb_allocator_exit(knState->dynamicAllocator);
    }
    knState->dynamicAllocator = tb_null;

    if (knState->largeAllocator) {
        tb_allocator_exit(knState->largeAllocator);
    }
    knState->largeAllocator = tb_null;

    if (knState->largeConstAllocator) {
        tb_allocator_exit(knState->largeConstAllocator);
    }
    knState->largeConstAllocator = tb_null;

}

KN KN_NewConstMemObj(KonState* kstate, size_t size, kon_uint_t tag)
{
    KN res = (KN)tb_allocator_malloc0(kstate->constAllocator, size);

    // add to heap ptr store
    KN_RecordNewKonNode(kstate, res);

    if (res.asU64) {
        KN_OBJ_PTR_TYPE(res) = tag;
        // set dispatcher id
        if (tag == KN_T_FIXNUM || tag == KN_T_FLONUM || tag == KN_T_BIGNUM) {
            KN_FIELD(res, Base, msgDispatcherId) = KN_T_NUMBER;
        }
        else if (tag == KN_T_NIL || tag == KN_T_PAIR) {
            KN_FIELD(res, Base, msgDispatcherId) = KN_T_PAIRLIST;
        }
        else {
            KN_FIELD(res, Base, msgDispatcherId) = tag;
        }
        
    }

    return res;
}

KN KN_NewDynamicMemObj(KonState* kstate, size_t size, kon_uint_t tag)
{
    KN res = (KN)tb_allocator_malloc0(kstate->dynamicAllocator, size);

    // add to heap ptr store
    KN_RecordNewKonNode(kstate, res);

    if (res.asU64) {
        KN_OBJ_PTR_TYPE(res) = tag;
        // set dispatcher id
        if (tag == KN_T_FIXNUM || tag == KN_T_FLONUM || tag == KN_T_BIGNUM) {
            KN_FIELD(res, Base, msgDispatcherId) = KN_T_NUMBER;
        }
        else if (tag == KN_T_NIL || tag == KN_T_PAIR) {
            KN_FIELD(res, Base, msgDispatcherId) = KN_T_PAIRLIST;
        }
        else {
            KN_FIELD(res, Base, msgDispatcherId) = tag;
        }
        
    }

    return res;
}

void KN_Gc(KonState* knState)
{
    KN_DEBUG("\n**trigger gc, before gc, statics:\n");
    KN_ShowGcStatics(knState);
    KN_MarkPhase(knState);
    KN_SweepPhase(knState);
}


void KN_MarkPhase(KonState* knState)
{

}
void KN_Mark(KonState* knState, KxList* taskQueue, char color)
{

}
void KN_SweepPhase()
{

}

void KN_ShowGcStatics(KonState* knState)
{
    // int barrierObjLength = KxList_Length(knState->writeBarrierGen);

    // long long totalObjCnt = KN_CurrentObjCount(knState);
    // KN_DEBUG("HeapPtrSegs count : %d, totalObjCnt %lld, barrierObjLength %d\n", KxList_Length(knState->heapPtrSegs), totalObjCnt, barrierObjLength);
}


void KN_RecordNewKonNode(KonState* knState, KN newVal)
{
// #if KN_DISABLE_GC
//     return;
// #endif
//     // add the pointers created between two continuation switch
//     // to a temp list
//     KN_FIELD(newVal, Base, gcMarkColor) = KN_GC_MARK_GRAY;
//     KxList_Push(knState->writeBarrierGen, newVal.asU64);
}

void KN_MarkNode(KonBase* node, KxList* markTaskQueue, char color)
{
    if (node == NULL || !KN_IS_POINTER(KON_2_KN(node))) {
        return;
    }
    
    // skip, if finished mark or just do marking stuffs
    if (node->gcMarkColor == KN_GC_MARK_BLACK
        || node->gcMarkColor == KN_GC_MARK_RED
    ) {
        return;
    }
    
    
    switch (node->tag) {
        case KN_T_STATE: {
            break;
        }
        case KN_T_FLONUM: {
            break;
        }
        case KN_T_BIGNUM: {
            break;
        }
        case KN_T_PAIR: {
            KonPair* pair = (KonPair*)node;
            KxList_Push(markTaskQueue, pair->prev.asU64);
            KxList_Push(markTaskQueue, pair->next.asU64);
            KxList_Push(markTaskQueue, pair->body.asU64);
            break;
        }
        case KN_T_SYMBOL: {
            break;
        }
        case KN_T_SYNTAX_MARKER: {
            break;
        }
        case KN_T_BYTES: {
            break;
        }
        case KN_T_STRING: {
            break;
        }
        case KN_T_VECTOR: {
            KxVector* vec = ((KonVector*)node)->vector;
            for (int i = 0; i < KxVector_Length(vec); i++) {
                KxList_Push(markTaskQueue, KxVector_AtIndex(vec, i));
            }
            break;
        }
        case KN_T_TABLE: {
            KxHashTable* table = ((KonTable*)node)->table;
            KxHashTableIter iter = KxHashTable_IterHead(table);
            while (iter != KNBOX_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(table, iter);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(table, iter));
                iter = next;
            }
            break;
        }
        case KN_T_MAP: {
            KxHashTable* map = ((KonMap*)node)->map;
            KxHashTableIter iter = KxHashTable_IterHead(map);
            while (iter != KNBOX_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(map, iter);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(map, iter));
                iter = next;
            }
            break;
        }
        case KN_T_CELL: {
            KonCell* cell = (KonCell*)node;
            KxList_Push(markTaskQueue, cell->core.asU64);
            KxList_Push(markTaskQueue, cell->map);
            KxList_Push(markTaskQueue, cell->table);
            KxList_Push(markTaskQueue, cell->list);
            if (cell->next != KNBOX_NIL) {
                KxList_Push(markTaskQueue, cell->next);
            }
            break;
        }
        case KN_T_PARAM: {
            KxHashTable* table = ((KonParam*)node)->table;;
            KxHashTableIter iter = KxHashTable_IterHead(table);
            while (iter != KNBOX_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(table, iter);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(table, iter));
                iter = next;
            }
            break;
        }
        case KN_T_BLOCK: {
            KonBlock* pair = (KonBlock*)node;
            KxList_Push(markTaskQueue, pair->prev.asU64);
            KxList_Push(markTaskQueue, pair->next.asU64);
            KxList_Push(markTaskQueue, pair->body.asU64);
            break;
        }
        case KN_T_QUOTE: {
            KonQuote* quote = (KonQuote*)node;
            KxList_Push(markTaskQueue, quote->inner.asU64);
            break;
        }
        case KN_T_QUASIQUOTE: {
            KonQuasiquote* quasi = (KonQuasiquote*)node;
            KxList_Push(markTaskQueue, quasi->inner.asU64);
            break;
        }
        case KN_T_UNQUOTE: {
            KonUnquote* unquote = (KonUnquote*)node;
            KxList_Push(markTaskQueue, unquote->inner.asU64);
            break;
        }
        case KN_T_ENV: {
            // BIND TABLE
            KonEnv* env = (KonEnv*)node;
            KxHashTable* tableBindings = env->bindings;
            KxHashTableIter iterBindings = KxHashTable_IterHead(tableBindings);
            while (iterBindings != KNBOX_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(tableBindings, iterBindings);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(tableBindings, iterBindings));
                iterBindings = next;
            }
            
            KxHashTable* tableDispatchers = env->bindings;
            KxHashTableIter iterDispatchers = KxHashTable_IterHead(tableDispatchers);
            while (iterDispatchers != KNBOX_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(tableDispatchers, iterDispatchers);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(tableDispatchers, iterDispatchers));
                iterDispatchers = next;
            }

            KxList_Push(markTaskQueue, env->parent);
            break;
        }
        case KN_T_ACCESSOR: {
            KonAccessor* slot = (KonAccessor*)node;
            if (slot->isDir) {
                KxHashTable* dirItems = slot->dir;
                KxHashTableIter iterDirItems = KxHashTable_IterHead(dirItems);
                while (iterDirItems != KNBOX_NIL) {
                    KxHashTableIter next = KxHashTable_IterNext(dirItems, iterDirItems);
                    KxList_Push(markTaskQueue, KxHashTable_IterGetVal(dirItems, iterDirItems));
                    iterDirItems = next;
                }
            }
            else {
                KxList_Push(markTaskQueue, slot->value.asU64);
            }
            break;
        }
        case KN_T_MSG_DISPATCHER: {
            KonMsgDispatcher* dispatcher = (KonMsgDispatcher*)node;
            KxList_Push(markTaskQueue, dispatcher->onSymbol);
            KxList_Push(markTaskQueue, dispatcher->onSyntaxMarker);
            KxList_Push(markTaskQueue, dispatcher->onMethodCall);
            KxList_Push(markTaskQueue, dispatcher->onVisitList);
            KxList_Push(markTaskQueue, dispatcher->onVisitVector);
            KxList_Push(markTaskQueue, dispatcher->onVisitTable);
            KxList_Push(markTaskQueue, dispatcher->onVisitCell);
            KxList_Push(markTaskQueue, dispatcher->onOtherType);
            break;
        }
        case KN_T_CONTINUATION: {
            KonContinuation* cont = (KonContinuation*)node;

            if (cont->type == KN_CONT_NATIVE_CALLBACK) {
                KxHashTable* table = cont->native.memoTable;
                KxHashTableIter iter = KxHashTable_IterHead(table);
                while (iter != KNBOX_NIL) {
                    KxHashTableIter next = KxHashTable_IterNext(table, iter);
                    KxList_Push(markTaskQueue, KxHashTable_IterGetVal(table, iter));
                    iter = next;
                }
            }
            else if (cont->type == KN_CONT_EVAL_CLAUSE_LIST) {
                KxList_Push(markTaskQueue, cont->evalClauseList.subj.asU64);
                KxList_Push(markTaskQueue, cont->evalClauseList.restClauses.asU64);
            }
            else if (cont->type == KN_CONT_EVAL_CLAUSE_ARGS) {
                KxList_Push(markTaskQueue, cont->evalClauseArgs.subj.asU64);
                KxList_Push(markTaskQueue, cont->evalClauseArgs.restArgList.asU64);
                KxList_Push(markTaskQueue, cont->evalClauseArgs.evaledArgList.asU64);
            }
            else {
                KxList_Push(markTaskQueue, cont->restJobs.asU64);
            }

            KxList_Push(markTaskQueue, cont->env);
            KxList_Push(markTaskQueue, cont->cont);
            break;
        }
        case KN_T_PROCEDURE: {
            KonProcedure* proc = (KonProcedure*)node;
            if (proc->type != KN_NATIVE_FUNC && proc->type != KN_NATIVE_OBJ_METHOD) {
                KxList_Push(markTaskQueue, proc->composite.argList.asU64);
                KxList_Push(markTaskQueue, proc->composite.body.asU64);
                KxList_Push(markTaskQueue, proc->composite.lexicalEnv);
                KxList_Push(markTaskQueue, proc->composite.captureList.asU64);
            }
            break;
        }
        case KN_T_CPOINTER: {
            break;
        }
        case KN_T_EXCEPTION: {
            break;
        }
        default: {
        }
    }

    node->gcMarkColor = color;
}


// free children and free node
void KN_DestroyNode(KonState* knState, KonBase* node)
{
    if (node == NULL || !KN_IS_POINTER(KON_2_KN(node))) {
        return;
    }

    switch (node->tag) {
        case KN_T_STATE: {
            break;
        }
        case KN_T_FLONUM: {
            break;
        }
        case KN_T_BIGNUM: {
            break;
        }
        case KN_T_PAIR: {
            break;
        }
        case KN_T_SYMBOL: {
            KonSymbol* sym = (KonSymbol*)node;
            if (sym->data != NULL) {
                tb_free(sym->data);
                sym->data = NULL;
            }
            break;
        }
        case KN_T_SYNTAX_MARKER: {
            break;
        }
        case KN_T_BYTES: {
            break;
        }
        case KN_T_STRING: {
            KonString* str = (KonString*)node;
            if (str->string != NULL) {
                KxStringBuffer_Destroy(str->string);
                str->string = NULL;
            }
            break;
        }
        case KN_T_VECTOR: {
            KonVector* vector = (KonVector*)node;
            if (vector->vector != NULL) {
                KxVector_Destroy(vector->vector);
                vector->vector = NULL;
            }
            break;
        }
        case KN_T_TABLE: {
            KonTable* table = (KonTable*)node;
            if (table->table != NULL) {
                KxHashTable_Destroy(table->table);
                table->table = NULL;
            }
            break;
        }
        case KN_T_MAP: {
            KxHashTable* map = (KonMap*)node;
            if (map != NULL) {
                KxHashTable_Destroy(map);
                map = NULL;
            }
            break;
        }
        case KN_T_CELL: {
            KonCell* cell = (KonCell*)node;
            break;
        }
        case KN_T_PARAM: {
            KonParam* table = (KonParam*)node;
            if (table->table != NULL) {
                KxHashTable_Destroy(table->table);
                table->table = NULL;
            }
            break;
        }
        case KN_T_BLOCK: {
        }
        case KN_T_QUOTE: {
            break;
        }
        case KN_T_QUASIQUOTE: {
            break;
        }
        case KN_T_UNQUOTE: {
            break;
        }
        case KN_T_ENV: {
            
            KonEnv* env = (KonEnv*)node;
            // don't clear root gc
            if (env->parent == KNBOX_NIL) {
                break;
                
            }
            if (env->bindings != NULL) {
                KxHashTable_Destroy(env->bindings);
                env->bindings = NULL;
            }
            break;
        }
        case KN_T_ACCESSOR: {
            KonAccessor* slot = (KonAccessor*)node;
            if (slot->isDir && slot->dir != NULL) {
                KxHashTable_Destroy(slot->dir);
                slot->dir = NULL;
            }
            break;
        }
        case KN_T_MSG_DISPATCHER: {
            KonMsgDispatcher* dispatcher = (KonMsgDispatcher*)node;
            break;
        }
        case KN_T_CONTINUATION: {
            KonContinuation* cont = (KonContinuation*)node;
            if (cont->type == KN_CONT_NATIVE_CALLBACK && cont->native.memoTable) {
                KxHashTable* table = cont->native.memoTable;
                KN_DEBUG("destroy memo table, table addr %x", table);
                KxHashTable_PrintKeys(table);
                KxHashTable_Destroy(table);
                cont->native.memoTable = NULL;
            }

            break;
        }
        case KN_T_PROCEDURE: {
            break;
        }
        case KN_T_CPOINTER: {
            break;
        }
        case KN_T_EXCEPTION: {
            break;
        }
        default: {
        }
    }
    tb_free(node);
    node = NULL;
}
