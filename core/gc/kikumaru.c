#include "kikumaru.h"

void KN_MarkPhase(Kana* kana);
void KN_Mark(Kana* kana, KnList* taskQueue, char color);
void KN_SweepPhase();
void KN_ResetAndCopyPtrSegList(Kana* kana);

void KN_ShowGcStatics(Kana* kana);

void KN_RecordNewKonNode(Kana* kana, KN newVal);

void KN_MarkNode(struct _KonBase* item, KnList* markTaskQueue, char color);
void KN_DestroyNode(Kana* kana, struct _KonBase* item);

void KN_InitGc(Kana* kana)
{
    kana->largeConstAllocator = tb_large_allocator_init(tb_null, 0);
    kana->constAllocator = tb_default_allocator_init(kana->largeConstAllocator);
    kana->largeAllocator = tb_large_allocator_init(tb_null, 0);
    kana->dynamicAllocator = tb_default_allocator_init(kana->largeAllocator);
    
    if (!tb_init(tb_null, kana->dynamicAllocator)) {
        printf("tb_init failed\n");
        return;
    }


}


void KN_DestroyGc(Kana* kana)
{
    // exit allocator
    if (kana->constAllocator) {
        tb_allocator_exit(kana->constAllocator);
    }
    kana->constAllocator = tb_null;

    if (kana->dynamicAllocator) {
        tb_allocator_exit(kana->dynamicAllocator);
    }
    kana->dynamicAllocator = tb_null;

    if (kana->largeAllocator) {
        tb_allocator_exit(kana->largeAllocator);
    }
    kana->largeAllocator = tb_null;

    if (kana->largeConstAllocator) {
        tb_allocator_exit(kana->largeConstAllocator);
    }
    kana->largeConstAllocator = tb_null;

}

KN KN_NewConstMemObj(Kana* kana, size_t size, kon_uint_t tag)
{
    KN res = (KN)tb_allocator_malloc0(kana->constAllocator, size);

    // add to heap ptr store
    KN_RecordNewKonNode(kana, res);

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

KN KN_NewDynamicMemObj(Kana* kana, size_t size, kon_uint_t tag)
{
    KN res = (KN)tb_allocator_malloc0(kana->dynamicAllocator, size);

    // add to heap ptr store
    KN_RecordNewKonNode(kana, res);

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

void KN_Gc(Kana* kana)
{
    KN_DEBUG("\n**trigger gc, before gc, statics:\n");
    KN_ShowGcStatics(kana);
    KN_MarkPhase(kana);
    KN_SweepPhase(kana);
}


void KN_MarkPhase(Kana* kana)
{

}
void KN_Mark(Kana* kana, KnList* taskQueue, char color)
{

}
void KN_SweepPhase()
{

}

void KN_ShowGcStatics(Kana* kana)
{
    // int barrierObjLength = KnList_Length(kana->writeBarrierGen);

    // long long totalObjCnt = KN_CurrentObjCount(kana);
    // KN_DEBUG("HeapPtrSegs count : %d, totalObjCnt %lld, barrierObjLength %d\n", KnList_Length(kana->heapPtrSegs), totalObjCnt, barrierObjLength);
}


void KN_RecordNewKonNode(Kana* kana, KN newVal)
{
// #if KN_DISABLE_GC
//     return;
// #endif
//     // add the pointers created between two continuation switch
//     // to a temp list
//     KN_FIELD(newVal, Base, gcMarkColor) = KN_GC_MARK_GRAY;
//     KnList_Push(kana->writeBarrierGen, newVal.asU64);
}

void KN_MarkNode(KonBase* node, KnList* markTaskQueue, char color)
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
            KnList_Push(markTaskQueue, pair->prev.asU64);
            KnList_Push(markTaskQueue, pair->next.asU64);
            KnList_Push(markTaskQueue, pair->body.asU64);
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
            KnVector* vec = ((KonVector*)node)->vector;
            for (int i = 0; i < KnVector_Length(vec); i++) {
                KnList_Push(markTaskQueue, KnVector_AtIndex(vec, i));
            }
            break;
        }
        case KN_T_TABLE: {
            KnHashTable* table = ((KonTable*)node)->table;
            KnHashTableIter iter = KnHashTable_IterHead(table);
            while (iter != KNBOX_NIL) {
                KnHashTableIter next = KnHashTable_IterNext(table, iter);
                KnList_Push(markTaskQueue, KnHashTable_IterGetVal(table, iter));
                iter = next;
            }
            break;
        }
        case KN_T_MAP: {
            KnHashTable* map = ((KonMap*)node)->map;
            KnHashTableIter iter = KnHashTable_IterHead(map);
            while (iter != KNBOX_NIL) {
                KnHashTableIter next = KnHashTable_IterNext(map, iter);
                KnList_Push(markTaskQueue, KnHashTable_IterGetVal(map, iter));
                iter = next;
            }
            break;
        }
        case KN_T_CELL: {
            KonCell* cell = (KonCell*)node;
            KnList_Push(markTaskQueue, cell->core.asU64);
            KnList_Push(markTaskQueue, cell->map);
            KnList_Push(markTaskQueue, cell->table);
            KnList_Push(markTaskQueue, cell->list);
            if (cell->next != KNBOX_NIL) {
                KnList_Push(markTaskQueue, cell->next);
            }
            break;
        }
        case KN_T_PARAM: {
            KnHashTable* table = ((KonParam*)node)->table;;
            KnHashTableIter iter = KnHashTable_IterHead(table);
            while (iter != KNBOX_NIL) {
                KnHashTableIter next = KnHashTable_IterNext(table, iter);
                KnList_Push(markTaskQueue, KnHashTable_IterGetVal(table, iter));
                iter = next;
            }
            break;
        }
        case KN_T_BLOCK: {
            KonBlock* pair = (KonBlock*)node;
            KnList_Push(markTaskQueue, pair->prev.asU64);
            KnList_Push(markTaskQueue, pair->next.asU64);
            KnList_Push(markTaskQueue, pair->body.asU64);
            break;
        }
        case KN_T_QUOTE: {
            KonQuote* quote = (KonQuote*)node;
            KnList_Push(markTaskQueue, quote->inner.asU64);
            break;
        }
        case KN_T_QUASIQUOTE: {
            KonQuasiquote* quasi = (KonQuasiquote*)node;
            KnList_Push(markTaskQueue, quasi->inner.asU64);
            break;
        }
        case KN_T_UNQUOTE: {
            KonUnquote* unquote = (KonUnquote*)node;
            KnList_Push(markTaskQueue, unquote->inner.asU64);
            break;
        }
        case KN_T_ENV: {
            // BIND TABLE
            KonEnv* env = (KonEnv*)node;
            KnHashTable* tableBindings = env->bindings;
            KnHashTableIter iterBindings = KnHashTable_IterHead(tableBindings);
            while (iterBindings != KNBOX_NIL) {
                KnHashTableIter next = KnHashTable_IterNext(tableBindings, iterBindings);
                KnList_Push(markTaskQueue, KnHashTable_IterGetVal(tableBindings, iterBindings));
                iterBindings = next;
            }
            
            KnHashTable* tableDispatchers = env->bindings;
            KnHashTableIter iterDispatchers = KnHashTable_IterHead(tableDispatchers);
            while (iterDispatchers != KNBOX_NIL) {
                KnHashTableIter next = KnHashTable_IterNext(tableDispatchers, iterDispatchers);
                KnList_Push(markTaskQueue, KnHashTable_IterGetVal(tableDispatchers, iterDispatchers));
                iterDispatchers = next;
            }

            KnList_Push(markTaskQueue, env->parent);
            break;
        }
        case KN_T_ACCESSOR: {
            KonAccessor* slot = (KonAccessor*)node;
            if (slot->isDir) {
                KnHashTable* dirItems = slot->dir;
                KnHashTableIter iterDirItems = KnHashTable_IterHead(dirItems);
                while (iterDirItems != KNBOX_NIL) {
                    KnHashTableIter next = KnHashTable_IterNext(dirItems, iterDirItems);
                    KnList_Push(markTaskQueue, KnHashTable_IterGetVal(dirItems, iterDirItems));
                    iterDirItems = next;
                }
            }
            else {
                KnList_Push(markTaskQueue, slot->value.asU64);
            }
            break;
        }
        case KN_T_MSG_DISPATCHER: {
            KonMsgDispatcher* dispatcher = (KonMsgDispatcher*)node;
            KnList_Push(markTaskQueue, dispatcher->onSymbol);
            KnList_Push(markTaskQueue, dispatcher->onSyntaxMarker);
            KnList_Push(markTaskQueue, dispatcher->onMethodCall);
            KnList_Push(markTaskQueue, dispatcher->onVisitList);
            KnList_Push(markTaskQueue, dispatcher->onVisitVector);
            KnList_Push(markTaskQueue, dispatcher->onVisitTable);
            KnList_Push(markTaskQueue, dispatcher->onVisitCell);
            KnList_Push(markTaskQueue, dispatcher->onOtherType);
            break;
        }
        case KN_T_CONTINUATION: {
            // TODO
            // KonContinuation* cont = (KonContinuation*)node;

            // KnList_Push(markTaskQueue, cont->env);
            // KnList_Push(markTaskQueue, cont->cont);
            break;
        }
        case KN_T_PROCEDURE: {
            KonProcedure* proc = (KonProcedure*)node;
            if (proc->type != KN_NATIVE_FUNC && proc->type != KN_NATIVE_OBJ_METHOD) {
                KnList_Push(markTaskQueue, proc->composite.argList.asU64);
                KnList_Push(markTaskQueue, proc->composite.body.asU64);
                KnList_Push(markTaskQueue, proc->composite.lexicalEnv);
                KnList_Push(markTaskQueue, proc->composite.captureList.asU64);
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
void KN_DestroyNode(Kana* kana, KonBase* node)
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
                KnVector_Destroy(vector->vector);
                vector->vector = NULL;
            }
            break;
        }
        case KN_T_TABLE: {
            KonTable* table = (KonTable*)node;
            if (table->table != NULL) {
                KnHashTable_Destroy(table->table);
                table->table = NULL;
            }
            break;
        }
        case KN_T_MAP: {
            KnHashTable* map = (KonMap*)node;
            if (map != NULL) {
                KnHashTable_Destroy(map);
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
                KnHashTable_Destroy(table->table);
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
                KnHashTable_Destroy(env->bindings);
                env->bindings = NULL;
            }
            break;
        }
        case KN_T_ACCESSOR: {
            KonAccessor* slot = (KonAccessor*)node;
            if (slot->isDir && slot->dir != NULL) {
                KnHashTable_Destroy(slot->dir);
                slot->dir = NULL;
            }
            break;
        }
        case KN_T_MSG_DISPATCHER: {
            KonMsgDispatcher* dispatcher = (KonMsgDispatcher*)node;
            break;
        }
        case KN_T_CONTINUATION: {
            // TODO
            // KonContinuation* cont = (KonContinuation*)node;
            // if (cont->type == KN_CONT_NATIVE_CALLBACK && cont->native.memoTable) {
            //     KnHashTable* table = cont->native.memoTable;
            //     KN_DEBUG("destroy memo table, table addr %x", table);
            //     KnHashTable_PrintKeys(table);
            //     KnHashTable_Destroy(table);
            //     cont->native.memoTable = NULL;
            // }

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
