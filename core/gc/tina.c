#include "tina.h"


#define FIRST_HEAP_PTR_SEG_SIZE 512
// #define FIRST_HEAP_PTR_SEG_SIZE 1024 * 8

// 7 ~= 22052
#define MAX_SEGMENT_CNT 7
// #define MAX_SEGMENT_CNT 2

void KN_MarkPhase(Kana* kana);
void KN_Mark(Kana* kana, KnList* taskQueue, char color);
void KN_SweepPhase();
void KN_ResetAndCopyPtrSegList(Kana* kana);
void KN_MarkNode(struct _KonBase* item, KnList* markTaskQueue, char color);
void KN_DestroyNode(Kana* kana, struct _KonBase* item);



KnList* KN_CreateHeapPtrSegList(Kana* kana)
{
    KnVector* firstSeg = KnVector_InitWithCapacity(FIRST_HEAP_PTR_SEG_SIZE);
    KnList* segList = KnList_Init();
    KnList_Push(segList, firstSeg);
    return segList;
}

void KN_InitGc(Kana* kana)
{

    kana->largeAllocator = tb_large_allocator_init(tb_null, 0);
    kana->allocator = tb_default_allocator_init(kana->largeAllocator);
    
    if (!tb_init(tb_null, kana->allocator)) {
        printf("tb_init failed\n");
        return;
    }

    kana->writeBarrierGen = KnList_Init();
    kana->heapPtrSegs = KN_CreateHeapPtrSegList(kana);
    kana->markTaskQueue = KnList_Init();
    kana->segmentMaxSizeVec = KnVector_InitWithSize(MAX_SEGMENT_CNT);

    unsigned long nextSize = FIRST_HEAP_PTR_SEG_SIZE;
    unsigned long maxObjCnt = 0;

    for (int i = 0; i < MAX_SEGMENT_CNT; i++) {
        maxObjCnt += nextSize;
        // KN_DEBUG("seg vec index %d, size %d\n", i, nextSize);
        KnVector_SetIndex(kana->segmentMaxSizeVec, i, KN_VEC_BOX_UINT(nextSize));
        
        // align to n *4
        int notAligned = nextSize * 1.618;
        nextSize = notAligned + 4 - (notAligned % 4);
    }
    // KN_DEBUG("MaxObjCntLimit %d\n", maxObjCnt);
    kana->maxObjCntLimit = maxObjCnt;
    kana->gcThreshold = maxObjCnt - nextSize / 4;
    kana->needGc = false;
}


void KN_DestroyGc(Kana* kana)
{
    // exit allocator
    if (kana->allocator) {
        tb_allocator_exit(kana->allocator);
    }
    kana->allocator = tb_null;

    if (kana->largeAllocator) {
        tb_allocator_exit(kana->largeAllocator);
    }
    kana->largeAllocator = tb_null;


    KN_ShowGcStatics(kana);
}

KN KN_NewDynamicMemObj(Kana* kana, size_t size, kon_uint_t tag)
{
    KN res = (KN)tb_allocator_malloc0(kana->allocator, size);

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

void KN_ShowGcStatics(Kana* kana)
{
    int barrierObjLength = KnList_Length(kana->writeBarrierGen);

    long long totalObjCnt = KN_CurrentObjCount(kana);
    KN_DEBUG("HeapPtrSegs count : %d, totalObjCnt %lld, barrierObjLength %d\n", KnList_Length(kana->heapPtrSegs), totalObjCnt, barrierObjLength);
}


int KN_PushToHeapPtrSeg(Kana* kana, KnList* heapPtrSegs, KN ptr)
{
    KnListNode* iter = KnList_IterHead(heapPtrSegs);
    // KN_DEBUG("KnList_IterHead(heapPtrSegs) addr %x\n", iter);
    int segIndex = -1;
    while (iter != KNBOX_NIL) {
        segIndex += 1;
        KnListNode* next = KnList_IterNext(iter);

        KnVector* segment = (KnVector*)KnList_IterVal(iter);
        if (KnVector_SpaceLeft(segment) > 0) {
            KnVector_Push(segment, ptr.asU64);
            return 1;
        }
        
        iter = next;
    }

    // all segments are full
    if (KnList_Length(heapPtrSegs) >= MAX_SEGMENT_CNT) {
        KN_DEBUG("error!!! reach gc ptr cnt limit, current seg cnt %d\n", KnList_Length(heapPtrSegs));
        exit(1);
        // return -1;
    }
    // create a new segment
    segIndex += 1;
    int nextSize = KN_VEC_UNBOX_UINT(KnVector_AtIndex(kana->segmentMaxSizeVec, segIndex));
    KnVector* nextSeg = KnVector_InitWithCapacity(nextSize);
    KnVector_Push(nextSeg, ptr.asU64);
    KN_DEBUG("\n**create a new segment, seg index %d , new seg size %d\n", segIndex, nextSize);

    KnList_Push(heapPtrSegs, nextSeg);

    
    return 1;
}

long long KN_CurrentObjCount(Kana* kana)
{
    KnListNode* iter = KnList_IterHead(kana->heapPtrSegs);
    long long count = 0;
    while (iter != KNBOX_NIL) {
        KnListNode* next = KnList_IterNext(iter);

        KnVector* segment = (KnVector*)KnList_IterVal(iter);
        count += KnVector_Length(segment);

        iter = next;
    }
    return count;
}

bool KN_HasEnoughSegSpace(Kana* kana, int requireSize)
{
    if (requireSize == 0) {
        return true;
    }
    long long totalObjCnt = KN_CurrentObjCount(kana);
    long long limit = kana->gcThreshold;
    
    bool needGc = (limit - totalObjCnt - requireSize) >= 0;
    if (!needGc) {
        KN_DEBUG("requireSize %d, current totalObjCnt %d, max limit %d\n", requireSize, totalObjCnt, limit);
        kana->needGc = true;
    }
    
    return needGc;
}


void KN_PushWriteBarrierObjsToHeapPtrSeg(Kana* kana)
{
    klist_val_t konPtr = KnList_Shift(kana->writeBarrierGen);

    while (konPtr != KN_LIST_UNDEF) {
        ((KonBase*)konPtr)->gcMarkColor = KN_GC_MARK_WHITE;
        // add to segment
        KN_PushToHeapPtrSeg(kana, kana->heapPtrSegs, (KN)konPtr);

        konPtr = KnList_Shift(kana->writeBarrierGen);
    }
}

void KN_SwitchContinuation(Kana* kana, KonContinuation* cont)
{
#if KN_DISABLE_GC
    return;
#endif
    kana->currCont = cont;
    
    // write barrier list size
    int barrierObjLength = KnList_Length(kana->writeBarrierGen);
    bool firstTry = KN_HasEnoughSegSpace(kana, barrierObjLength);
    if (!firstTry) {
        long long totalObjCnt = KN_CurrentObjCount(kana);
        long long limit = kana->maxObjCntLimit;
        KN_DEBUG("no EnoughSegSpace requireSize %d, current totalObjCnt %lld, max limit %lld\n", barrierObjLength, totalObjCnt, limit);

        // don't trigger gc at this time

        // KN_Gc(kana);
        // bool secondTry = KN_HasEnoughSegSpace(kana, barrierObjLength);

        // if (!secondTry) {
        //     KN_DEBUG("gc failed, reach ptr count limit\n");
        //     KN_ShowGcStatics(kana);
        //     exit(1);
        // }
    }
    
    KN_PushWriteBarrierObjsToHeapPtrSeg(kana);
}

void KN_RecordNewKonNode(Kana* kana, KN newVal)
{
#if KN_DISABLE_GC
    return;
#endif
    // add the pointers created between two continuation switch
    // to a temp list
    KN_FIELD(newVal, Base, gcMarkColor) = KN_GC_MARK_GRAY;
    KnList_Push(kana->writeBarrierGen, newVal.asU64);
}

// safepoint:
// after function finished
// after a loop body finished
void KN_EnterGcSafepoint(Kana* kana)
{
    if (kana->needGc) {
        KN_DEBUG("KN_EnterGcSafepoint");
        KN_Gc(kana);
        kana->needGc = false;
    }
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
    // code pointers should be reserved
    if (kana->currCode.asU64 != NULL) {
        KnList_Push(kana->markTaskQueue, kana->currCode.asU64);
    }

    // pointers which are not set to pointer addr segments should be reserved
    KnListNode* iter = KnList_IterHead(kana->writeBarrierGen);
    while (iter != KNBOX_NIL) {
        KnListNode* next = KnList_IterNext(iter);

        KN ptr = (KN)KnList_IterVal(iter);

        // add to MarkTaskQueue
        KnList_Push(kana->markTaskQueue, ptr.asU64);

        iter = next;
    }

    // msg dispatchers should be reserved
    int vecLen = KnVector_Length(kana->msgDispatchers);
    for (int i = 0; i < vecLen; i++) {
        KN dispatcherPtr = (KN)KnVector_AtIndex(kana->msgDispatchers, i);
        KnList_Push(kana->markTaskQueue, dispatcherPtr.asU64);
    }

    if (kana->currCont != NULL) {
        KnList_Push(kana->markTaskQueue, kana->currCont);
    }

    KN_Mark(kana, kana->markTaskQueue, KN_GC_MARK_BLACK);

    // FIXME: this is a workaround
    // to avoid free AST nodes
    // reserve code pointers
    // KnList* astTaskQueue = KnList_Init();
    // KnList_Push(astTaskQueue, kana->currCode);
    // KN_Mark(kana, astTaskQueue, KN_GC_MARK_BLACK);
    // KnList_Destroy(astTaskQueue);
}

// 1. shift a KonValue
// 2. mark this value to black
// 3. add all children to this queue
void KN_Mark(Kana* kana, KnList* taskQueue, char color)
{
    while (KnList_Length(kana->markTaskQueue) > 0) {
        KonBase* konPtr = KnList_Shift(taskQueue);
        KN_MarkNode(konPtr, taskQueue, color);
        
    }
}

void KN_SweepPhase(Kana* kana)
{

    // mark kana->writeBarrierGen objs to gray
    KnListNode* iter = KnList_IterHead(kana->writeBarrierGen);
    while (iter != KNBOX_NIL) {
        KnListNode* next = KnList_IterNext(iter);

        KonBase* ptr = (KonBase*)KnList_IterVal(iter);

        ptr->gcMarkColor = KN_GC_MARK_GRAY;

        iter = next;
    }
    
    // mark kana->heapPtrSegs objs to white
    // and copy to a new segment

    KN_ResetAndCopyPtrSegList(kana);
}


void KN_ResetAndCopyPtrSegList(Kana* kana)
{
    KN_DEBUG("KN_ResetAndCopyPtrSegList\n");
    // shink the segment list
    KnList* newPtrSegList = KN_CreateHeapPtrSegList(kana);
    
    KnListNode* iter = KnList_IterHead(kana->heapPtrSegs);
    
    unsigned long whiteCnt = 0;
    unsigned long grayCnt = 0;
    unsigned long redCnt = 0;
    unsigned long blackCnt = 0;
    
    while (iter != KNBOX_NIL) {
        KnListNode* next = KnList_IterNext(iter);

        KnVector* segment = (KnVector*)KnList_IterVal(iter);

        int vecLen = KnVector_Length(segment);
        for (int i = 0; i < vecLen; i++) {
            KonBase* konPtr = (KonBase*)KnVector_AtIndex(segment, i);
            if (konPtr->gcMarkColor == KN_GC_MARK_WHITE) {
                whiteCnt += 1;
                KN_DestroyNode(kana, konPtr);
                continue;
            }
            else if (konPtr->gcMarkColor == KN_GC_MARK_GRAY) {
                grayCnt += 1;
            }
            else if (konPtr->gcMarkColor == KN_GC_MARK_RED) {
                redCnt += 1;
            }
            else if (konPtr->gcMarkColor == KN_GC_MARK_BLACK) {
                blackCnt += 1;
            }
            // reset black node to white
            konPtr->gcMarkColor = KN_GC_MARK_WHITE;
            KN_PushToHeapPtrSeg(kana, newPtrSegList, KON_2_KN(konPtr));
        }

        KnVector_Destroy(segment);
        KnList_DelNode(kana->heapPtrSegs, iter);

        iter = next;
    }
    
    KN_DEBUG("ptr color cnt : white %ld, gray %ld, red %ld, black %ld\n", whiteCnt, grayCnt, redCnt, blackCnt);
    
    // free old kana->heapPtrSegs
    KnList_Destroy(kana->heapPtrSegs);

    kana->heapPtrSegs = newPtrSegList;
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
            KonContinuation* cont = (KonContinuation*)node;

            if (cont->type == KN_CONT_NATIVE_CALLBACK) {
                KnHashTable* table = cont->native.memoTable;
                KnHashTableIter iter = KnHashTable_IterHead(table);
                while (iter != KNBOX_NIL) {
                    KnHashTableIter next = KnHashTable_IterNext(table, iter);
                    KnList_Push(markTaskQueue, KnHashTable_IterGetVal(table, iter));
                    iter = next;
                }
            }
            else if (cont->type == KN_CONT_EVAL_CLAUSE_LIST) {
                KnList_Push(markTaskQueue, cont->evalClauseList.subj.asU64);
                KnList_Push(markTaskQueue, cont->evalClauseList.restClauses.asU64);
            }
            else if (cont->type == KN_CONT_EVAL_CLAUSE_ARGS) {
                KnList_Push(markTaskQueue, cont->evalClauseArgs.subj.asU64);
                KnList_Push(markTaskQueue, cont->evalClauseArgs.restArgList.asU64);
                KnList_Push(markTaskQueue, cont->evalClauseArgs.evaledArgList.asU64);
            }
            else {
                KnList_Push(markTaskQueue, cont->restJobs.asU64);
            }

            KnList_Push(markTaskQueue, cont->env);
            KnList_Push(markTaskQueue, cont->cont);
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
            KonContinuation* cont = (KonContinuation*)node;
            if (cont->type == KN_CONT_NATIVE_CALLBACK && cont->native.memoTable) {
                KnHashTable* table = cont->native.memoTable;
                KN_DEBUG("destroy memo table, table addr %x", table);
                KnHashTable_PrintKeys(table);
                KnHashTable_Destroy(table);
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
