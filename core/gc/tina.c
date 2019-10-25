#include "tina.h"


#define FIRST_HEAP_PTR_SEG_SIZE 512
// #define FIRST_HEAP_PTR_SEG_SIZE 1024 * 8

// 7 ~= 22052
#define MAX_SEGMENT_CNT 7
// #define MAX_SEGMENT_CNT 2

void KN_MarkPhase(KonState* knState);
void KN_Mark(KonState* knState, KxList* taskQueue, char color);
void KN_SweepPhase();
void KN_ResetAndCopyPtrSegList(KonState* knState);
void KN_MarkNode(struct _KonBase* item, KxList* markTaskQueue, char color);
void KN_DestroyNode(KonState* knState, struct _KonBase* item);



KxList* KN_CreateHeapPtrSegList(KonState* knState)
{
    KxVector* firstSeg = KxVector_InitWithCapacity(FIRST_HEAP_PTR_SEG_SIZE);
    KxList* segList = KxList_Init();
    KxList_Push(segList, firstSeg);
    return segList;
}

void KN_InitGc(KonState* knState)
{

    knState->largeAllocator = tb_large_allocator_init(tb_null, 0);
    knState->allocator = tb_default_allocator_init(knState->largeAllocator);
    
    if (!tb_init(tb_null, knState->allocator)) {
        printf("tb_init failed\n");
        return;
    }

    knState->writeBarrierGen = KxList_Init();
    knState->heapPtrSegs = KN_CreateHeapPtrSegList(knState);
    knState->markTaskQueue = KxList_Init();
    knState->segmentMaxSizeVec = KxVector_InitWithSize(MAX_SEGMENT_CNT);

    unsigned long nextSize = FIRST_HEAP_PTR_SEG_SIZE;
    unsigned long maxObjCnt = 0;

    for (int i = 0; i < MAX_SEGMENT_CNT; i++) {
        maxObjCnt += nextSize;
        // KN_DEBUG("seg vec index %d, size %d\n", i, nextSize);
        KxVector_SetIndex(knState->segmentMaxSizeVec, i, KX_VEC_BOX_UINT(nextSize));
        
        // align to n *4
        int notAligned = nextSize * 1.618;
        nextSize = notAligned + 4 - (notAligned % 4);
    }
    // KN_DEBUG("MaxObjCntLimit %d\n", maxObjCnt);
    knState->maxObjCntLimit = maxObjCnt;
    knState->gcThreshold = maxObjCnt - nextSize / 4;
    knState->needGc = false;
}


void KN_DestroyGc(KonState* knState)
{
    // exit allocator
    if (knState->allocator) {
        tb_allocator_exit(knState->allocator);
    }
    knState->allocator = tb_null;

    if (knState->largeAllocator) {
        tb_allocator_exit(knState->largeAllocator);
    }
    knState->largeAllocator = tb_null;


    KN_ShowGcStatics(knState);
}

KN KN_NewDynamicMemObj(KonState* kstate, size_t size, kon_uint_t tag)
{
    KN res = (KN)tb_allocator_malloc0(kstate->allocator, size);

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

void KN_ShowGcStatics(KonState* knState)
{
    int barrierObjLength = KxList_Length(knState->writeBarrierGen);

    long long totalObjCnt = KN_CurrentObjCount(knState);
    KN_DEBUG("HeapPtrSegs count : %d, totalObjCnt %lld, barrierObjLength %d\n", KxList_Length(knState->heapPtrSegs), totalObjCnt, barrierObjLength);
}


int KN_PushToHeapPtrSeg(KonState* knState, KxList* heapPtrSegs, KN ptr)
{
    KxListNode* iter = KxList_IterHead(heapPtrSegs);
    // KN_DEBUG("KxList_IterHead(heapPtrSegs) addr %x\n", iter);
    int segIndex = -1;
    while (iter != KNBOX_NIL) {
        segIndex += 1;
        KxListNode* next = KxList_IterNext(iter);

        KxVector* segment = (KxVector*)KxList_IterVal(iter);
        if (KxVector_SpaceLeft(segment) > 0) {
            KxVector_Push(segment, ptr.asU64);
            return 1;
        }
        
        iter = next;
    }

    // all segments are full
    if (KxList_Length(heapPtrSegs) >= MAX_SEGMENT_CNT) {
        KN_DEBUG("error!!! reach gc ptr cnt limit, current seg cnt %d\n", KxList_Length(heapPtrSegs));
        exit(1);
        // return -1;
    }
    // create a new segment
    segIndex += 1;
    int nextSize = KX_VEC_UNBOX_UINT(KxVector_AtIndex(knState->segmentMaxSizeVec, segIndex));
    KxVector* nextSeg = KxVector_InitWithCapacity(nextSize);
    KxVector_Push(nextSeg, ptr.asU64);
    KN_DEBUG("\n**create a new segment, seg index %d , new seg size %d\n", segIndex, nextSize);

    KxList_Push(heapPtrSegs, nextSeg);

    
    return 1;
}

long long KN_CurrentObjCount(KonState* knState)
{
    KxListNode* iter = KxList_IterHead(knState->heapPtrSegs);
    long long count = 0;
    while (iter != KNBOX_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KxVector* segment = (KxVector*)KxList_IterVal(iter);
        count += KxVector_Length(segment);

        iter = next;
    }
    return count;
}

bool KN_HasEnoughSegSpace(KonState* knState, int requireSize)
{
    if (requireSize == 0) {
        return true;
    }
    long long totalObjCnt = KN_CurrentObjCount(knState);
    long long limit = knState->gcThreshold;
    
    bool needGc = (limit - totalObjCnt - requireSize) >= 0;
    if (!needGc) {
        KN_DEBUG("requireSize %d, current totalObjCnt %d, max limit %d\n", requireSize, totalObjCnt, limit);
        knState->needGc = true;
    }
    
    return needGc;
}


void KN_PushWriteBarrierObjsToHeapPtrSeg(KonState* knState)
{
    klist_val_t konPtr = KxList_Shift(knState->writeBarrierGen);

    while (konPtr != KX_LIST_UNDEF) {
        ((KonBase*)konPtr)->gcMarkColor = KN_GC_MARK_WHITE;
        // add to segment
        KN_PushToHeapPtrSeg(knState, knState->heapPtrSegs, (KN)konPtr);

        konPtr = KxList_Shift(knState->writeBarrierGen);
    }
}

void KN_SwitchContinuation(KonState* knState, KonContinuation* cont)
{
#if KN_DISABLE_GC
    return;
#endif
    knState->currCont = cont;
    
    // write barrier list size
    int barrierObjLength = KxList_Length(knState->writeBarrierGen);
    bool firstTry = KN_HasEnoughSegSpace(knState, barrierObjLength);
    if (!firstTry) {
        long long totalObjCnt = KN_CurrentObjCount(knState);
        long long limit = knState->maxObjCntLimit;
        KN_DEBUG("no EnoughSegSpace requireSize %d, current totalObjCnt %lld, max limit %lld\n", barrierObjLength, totalObjCnt, limit);

        // don't trigger gc at this time

        // KN_Gc(knState);
        // bool secondTry = KN_HasEnoughSegSpace(knState, barrierObjLength);

        // if (!secondTry) {
        //     KN_DEBUG("gc failed, reach ptr count limit\n");
        //     KN_ShowGcStatics(knState);
        //     exit(1);
        // }
    }
    
    KN_PushWriteBarrierObjsToHeapPtrSeg(knState);
}

void KN_RecordNewKonNode(KonState* knState, KN newVal)
{
#if KN_DISABLE_GC
    return;
#endif
    // add the pointers created between two continuation switch
    // to a temp list
    KN_FIELD(newVal, Base, gcMarkColor) = KN_GC_MARK_GRAY;
    KxList_Push(knState->writeBarrierGen, newVal.asU64);
}

// safepoint:
// after function finished
// after a loop body finished
void KN_EnterGcSafepoint(KonState* knState)
{
    if (knState->needGc) {
        KN_DEBUG("KN_EnterGcSafepoint");
        KN_Gc(knState);
        knState->needGc = false;
    }
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
    // code pointers should be reserved
    if (knState->currCode.asU64 != NULL) {
        KxList_Push(knState->markTaskQueue, knState->currCode.asU64);
    }

    // pointers which are not set to pointer addr segments should be reserved
    KxListNode* iter = KxList_IterHead(knState->writeBarrierGen);
    while (iter != KNBOX_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KN ptr = (KN)KxList_IterVal(iter);

        // add to MarkTaskQueue
        KxList_Push(knState->markTaskQueue, ptr.asU64);

        iter = next;
    }

    // msg dispatchers should be reserved
    int vecLen = KxVector_Length(knState->msgDispatchers);
    for (int i = 0; i < vecLen; i++) {
        KN dispatcherPtr = (KN)KxVector_AtIndex(knState->msgDispatchers, i);
        KxList_Push(knState->markTaskQueue, dispatcherPtr.asU64);
    }

    if (knState->currCont != NULL) {
        KxList_Push(knState->markTaskQueue, knState->currCont);
    }

    KN_Mark(knState, knState->markTaskQueue, KN_GC_MARK_BLACK);

    // FIXME: this is a workaround
    // to avoid free AST nodes
    // reserve code pointers
    // KxList* astTaskQueue = KxList_Init();
    // KxList_Push(astTaskQueue, knState->currCode);
    // KN_Mark(knState, astTaskQueue, KN_GC_MARK_BLACK);
    // KxList_Destroy(astTaskQueue);
}

// 1. shift a KonValue
// 2. mark this value to black
// 3. add all children to this queue
void KN_Mark(KonState* knState, KxList* taskQueue, char color)
{
    while (KxList_Length(knState->markTaskQueue) > 0) {
        KonBase* konPtr = KxList_Shift(taskQueue);
        KN_MarkNode(konPtr, taskQueue, color);
        
    }
}

void KN_SweepPhase(KonState* knState)
{

    // mark knState->writeBarrierGen objs to gray
    KxListNode* iter = KxList_IterHead(knState->writeBarrierGen);
    while (iter != KNBOX_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KonBase* ptr = (KonBase*)KxList_IterVal(iter);

        ptr->gcMarkColor = KN_GC_MARK_GRAY;

        iter = next;
    }
    
    // mark knState->heapPtrSegs objs to white
    // and copy to a new segment

    KN_ResetAndCopyPtrSegList(knState);
}


void KN_ResetAndCopyPtrSegList(KonState* knState)
{
    KN_DEBUG("KN_ResetAndCopyPtrSegList\n");
    // shink the segment list
    KxList* newPtrSegList = KN_CreateHeapPtrSegList(knState);
    
    KxListNode* iter = KxList_IterHead(knState->heapPtrSegs);
    
    unsigned long whiteCnt = 0;
    unsigned long grayCnt = 0;
    unsigned long redCnt = 0;
    unsigned long blackCnt = 0;
    
    while (iter != KNBOX_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KxVector* segment = (KxVector*)KxList_IterVal(iter);

        int vecLen = KxVector_Length(segment);
        for (int i = 0; i < vecLen; i++) {
            KonBase* konPtr = (KonBase*)KxVector_AtIndex(segment, i);
            if (konPtr->gcMarkColor == KN_GC_MARK_WHITE) {
                whiteCnt += 1;
                KN_DestroyNode(knState, konPtr);
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
            KN_PushToHeapPtrSeg(knState, newPtrSegList, KON_2_KN(konPtr));
        }

        KxVector_Destroy(segment);
        KxList_DelNode(knState->heapPtrSegs, iter);

        iter = next;
    }
    
    KN_DEBUG("ptr color cnt : white %ld, gray %ld, red %ld, black %ld\n", whiteCnt, grayCnt, redCnt, blackCnt);
    
    // free old knState->heapPtrSegs
    KxList_Destroy(knState->heapPtrSegs);

    knState->heapPtrSegs = newPtrSegList;
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
