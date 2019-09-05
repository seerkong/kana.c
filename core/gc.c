#include "prefix.h"

#include "../lib/tbox/tbox.h"
#include "container/kx_hashtable.h"
#include "container/kx_vector.h"
#include "container/kx_list.h"
#include "gc.h"

#define FIRST_HEAP_PTR_SEG_SIZE 512
// #define FIRST_HEAP_PTR_SEG_SIZE 1024 * 8

// 7 ~= 22052
#define MAX_SEGMENT_CNT 7
// #define MAX_SEGMENT_CNT 2

KxList* KN_CreateHeapPtrSegList(KonState* kstate)
{
    KxVector* firstSeg = KxVector_InitWithCapacity(FIRST_HEAP_PTR_SEG_SIZE);
    KxList* segList = KxList_Init();
    KxList_Push(segList, firstSeg);
    return segList;
}

void KN_InitGc(KonState* kstate)
{
    kstate->writeBarrierGen = KxList_Init();
    kstate->heapPtrSegs = KN_CreateHeapPtrSegList(kstate);
    kstate->markTaskQueue = KxList_Init();
    kstate->segmentMaxSizeVec = KxVector_InitWithSize(MAX_SEGMENT_CNT);

    unsigned long nextSize = FIRST_HEAP_PTR_SEG_SIZE;
    unsigned long maxObjCnt = 0;

    for (int i = 0; i < MAX_SEGMENT_CNT; i++) {
        maxObjCnt += nextSize;
        // KN_DEBUG("seg vec index %d, size %d\n", i, nextSize);
        KxVector_SetIndex(kstate->segmentMaxSizeVec, i, KX_VEC_BOX_UINT(nextSize));
        
        // align to n *4
        int notAligned = nextSize * 1.618;
        nextSize = notAligned + 4 - (notAligned % 4);
    }
    // KN_DEBUG("MaxObjCntLimit %d\n", maxObjCnt);
    kstate->maxObjCntLimit = maxObjCnt;
    kstate->gcThreshold = maxObjCnt - nextSize / 4;
    kstate->needGc = false;
}

void KN_ShowGcStatics(KonState* kstate)
{
    int barrierObjLength = KxList_Length(kstate->writeBarrierGen);

    long long totalObjCnt = KN_CurrentObjCount(kstate);
    KN_DEBUG("HeapPtrSegs count : %d, totalObjCnt %lld, barrierObjLength %d\n", KxList_Length(kstate->heapPtrSegs), totalObjCnt, barrierObjLength);
}

void KN_DestroyGc(KonState* kstate)
{
    KN_ShowGcStatics(kstate);
}

int KN_PushToHeapPtrSeg(KonState* kstate, KxList* heapPtrSegs, KN ptr)
{
    KxListNode* iter = KxList_IterHead(heapPtrSegs);
    // KN_DEBUG("KxList_IterHead(heapPtrSegs) addr %x\n", iter);
    int segIndex = -1;
    while ((KN)iter != KN_NIL) {
        segIndex += 1;
        KxListNode* next = KxList_IterNext(iter);

        KxVector* segment = (KxVector*)KxList_IterVal(iter);
        if (KxVector_SpaceLeft(segment) > 0) {
            KxVector_Push(segment, ptr);
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
    int nextSize = KX_VEC_UNBOX_UINT(KxVector_AtIndex(kstate->segmentMaxSizeVec, segIndex));
    KxVector* nextSeg = KxVector_InitWithCapacity(nextSize);
    KxVector_Push(nextSeg, ptr);
    KN_DEBUG("\n**create a new segment, seg index %d , new seg size %d\n", segIndex, nextSize);

    KxList_Push(heapPtrSegs, nextSeg);

    
    return 1;
}

long long KN_CurrentObjCount(KonState* kstate)
{
    KxListNode* iter = KxList_IterHead(kstate->heapPtrSegs);
    long long count = 0;
    while ((KN)iter != KN_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KxVector* segment = (KxVector*)KxList_IterVal(iter);
        count += KxVector_Length(segment);

        iter = next;
    }
    return count;
}

bool KN_HasEnoughSegSpace(KonState* kstate, int requireSize)
{
    if (requireSize == 0) {
        return true;
    }
    long long totalObjCnt = KN_CurrentObjCount(kstate);
    long long limit = kstate->gcThreshold;
    
    bool needGc = (limit - totalObjCnt - requireSize) >= 0;
    if (!needGc) {
        KN_DEBUG("requireSize %d, current totalObjCnt %d, max limit %d\n", requireSize, totalObjCnt, limit);
        kstate->needGc = true;
    }
    
    return needGc;
}


void KN_PushWriteBarrierObjsToHeapPtrSeg(KonState* kstate)
{
    klist_val_t konPtr = KxList_Shift(kstate->writeBarrierGen);

    while (konPtr != KX_LIST_UNDEF) {
        ((KonBase*)konPtr)->gcMarkColor = KN_GC_MARK_WHITE;
        // add to segment
        KN_PushToHeapPtrSeg(kstate, kstate->heapPtrSegs, konPtr);

        konPtr = KxList_Shift(kstate->writeBarrierGen);
    }
}

void KN_SwitchContinuation(KonState* kstate, KonContinuation* cont)
{
#if KN_DISABLE_GC
    return;
#endif
    kstate->currCont = cont;
    
    // write barrier list size
    int barrierObjLength = KxList_Length(kstate->writeBarrierGen);
    bool firstTry = KN_HasEnoughSegSpace(kstate, barrierObjLength);
    if (!firstTry) {
        long long totalObjCnt = KN_CurrentObjCount(kstate);
        long long limit = kstate->maxObjCntLimit;
        KN_DEBUG("no EnoughSegSpace requireSize %d, current totalObjCnt %lld, max limit %lld\n", barrierObjLength, totalObjCnt, limit);

        // don't trigger gc at this time

        // KN_Gc(kstate);
        // bool secondTry = KN_HasEnoughSegSpace(kstate, barrierObjLength);

        // if (!secondTry) {
        //     KN_DEBUG("gc failed, reach ptr count limit\n");
        //     KN_ShowGcStatics(kstate);
        //     exit(1);
        // }
    }
    
    KN_PushWriteBarrierObjsToHeapPtrSeg(kstate);
}

void KN_RecordNewKonNode(KonState* kstate, KN newVal)
{
#if KN_DISABLE_GC
    return;
#endif
    // add the pointers created between two continuation switch
    // to a temp list
    ((KonBase*)newVal)->gcMarkColor = KN_GC_MARK_GRAY;
    KxList_Push(kstate->writeBarrierGen, newVal);
}

// safepoint:
// after function finished
// after a loop body finished
void KN_EnterGcSafepoint(KonState* kstate)
{
    if (kstate->needGc) {
        KN_DEBUG("KN_EnterGcSafepoint");
        KN_Gc(kstate);
        kstate->needGc = false;
    }
}

void KN_Gc(KonState* kstate)
{
    KN_DEBUG("\n**trigger gc, before gc, statics:\n");
    KN_ShowGcStatics(kstate);
    KN_MarkPhase(kstate);
    KN_SweepPhase(kstate);
}

void KN_MarkPhase(KonState* kstate)
{
    // code pointers should be reserved
    if (kstate->currCode != NULL) {
        KxList_Push(kstate->markTaskQueue, kstate->currCode);
    }

    // pointers which are not set to pointer addr segments should be reserved
    KxListNode* iter = KxList_IterHead(kstate->writeBarrierGen);
    while ((KN)iter != KN_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KN ptr = KxList_IterVal(iter);

        // add to MarkTaskQueue
        KxList_Push(kstate->markTaskQueue, ptr);

        iter = next;
    }

    // msg dispatchers should be reserved
    int vecLen = KxVector_Length(kstate->msgDispatchers);
    for (int i = 0; i < vecLen; i++) {
        KN dispatcherPtr = (KN)KxVector_AtIndex(kstate->msgDispatchers, i);
        KxList_Push(kstate->markTaskQueue, dispatcherPtr);
    }

    if (kstate->currCont != NULL) {
        KxList_Push(kstate->markTaskQueue, kstate->currCont);
    }

    KN_Mark(kstate, kstate->markTaskQueue, KN_GC_MARK_BLACK);

    // FIXME: this is a workaround
    // to avoid free AST nodes
    // reserve code pointers
    // KxList* astTaskQueue = KxList_Init();
    // KxList_Push(astTaskQueue, kstate->currCode);
    // KN_Mark(kstate, astTaskQueue, KN_GC_MARK_BLACK);
    // KxList_Destroy(astTaskQueue);
}

// 1. shift a KonValue
// 2. mark this value to black
// 3. add all children to this queue
void KN_Mark(KonState* kstate, KxList* taskQueue, char color)
{
    while (KxList_Length(kstate->markTaskQueue) > 0) {
        KonBase* konPtr = KxList_Shift(taskQueue);
        KN_MarkNode(konPtr, taskQueue, color);
        
    }
}

void KN_SweepPhase(KonState* kstate)
{

    // mark kstate->writeBarrierGen objs to gray
    KxListNode* iter = KxList_IterHead(kstate->writeBarrierGen);
    while ((KN)iter != KN_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KonBase* ptr = (KonBase*)KxList_IterVal(iter);

        ptr->gcMarkColor = KN_GC_MARK_GRAY;

        iter = next;
    }
    
    // mark kstate->heapPtrSegs objs to white
    // and copy to a new segment

    KN_ResetAndCopyPtrSegList(kstate);
}


void KN_ResetAndCopyPtrSegList(KonState* kstate)
{
    KN_DEBUG("KN_ResetAndCopyPtrSegList\n");
    // shink the segment list
    KxList* newPtrSegList = KN_CreateHeapPtrSegList(kstate);
    
    KxListNode* iter = KxList_IterHead(kstate->heapPtrSegs);
    
    unsigned long whiteCnt = 0;
    unsigned long grayCnt = 0;
    unsigned long redCnt = 0;
    unsigned long blackCnt = 0;
    
    while ((KN)iter != KN_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KxVector* segment = (KxVector*)KxList_IterVal(iter);

        int vecLen = KxVector_Length(segment);
        for (int i = 0; i < vecLen; i++) {
            KonBase* konPtr = (KonBase*)KxVector_AtIndex(segment, i);
            if (konPtr->gcMarkColor == KN_GC_MARK_WHITE) {
                whiteCnt += 1;
                KN_DestroyNode(kstate, konPtr);
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
            KN_PushToHeapPtrSeg(kstate, newPtrSegList, (KN)konPtr);
        }

        KxVector_Destroy(segment);
        KxList_DelNode(kstate->heapPtrSegs, iter);

        iter = next;
    }
    
    KN_DEBUG("ptr color cnt : white %ld, gray %ld, red %ld, black %ld\n", whiteCnt, grayCnt, redCnt, blackCnt);
    
    // free old kstate->heapPtrSegs
    KxList_Destroy(kstate->heapPtrSegs);

    kstate->heapPtrSegs = newPtrSegList;
}


void KN_MarkNode(KonBase* node, KxList* markTaskQueue, char color)
{
    if (node == NULL || !KN_IS_POINTER(node)) {
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
            KxList_Push(markTaskQueue, pair->prev);
            KxList_Push(markTaskQueue, pair->next);
            KxList_Push(markTaskQueue, pair->body);
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
            KxVector* vec = CAST_Kon(Vector, node)->vector;
            for (int i = 0; i < KxVector_Length(vec); i++) {
                KxList_Push(markTaskQueue, KxVector_AtIndex(vec, i));
            }
            break;
        }
        case KN_T_TABLE: {
            KxHashTable* table = CAST_Kon(Table, node)->table;
            KxHashTableIter iter = KxHashTable_IterHead(table);
            while ((KN)iter != KN_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(table, iter);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(table, iter));
                iter = next;
            }
            break;
        }
        case KN_T_MAP: {
            KxHashTable* map = CAST_Kon(Map, node)->map;
            KxHashTableIter iter = KxHashTable_IterHead(map);
            while ((KN)iter != KN_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(map, iter);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(map, iter));
                iter = next;
            }
            break;
        }
        case KN_T_CELL: {
            KonCell* cell = (KonCell*)node;
            KxList_Push(markTaskQueue, cell->core);
            KxList_Push(markTaskQueue, cell->map);
            KxList_Push(markTaskQueue, cell->table);
            KxList_Push(markTaskQueue, cell->list);
            if (cell->next != KN_NIL) {
                KxList_Push(markTaskQueue, cell->next);
            }
            break;
        }
        case KN_T_PARAM: {
            KxHashTable* table = CAST_Kon(Param, node)->table;
            KxHashTableIter iter = KxHashTable_IterHead(table);
            while ((KN)iter != KN_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(table, iter);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(table, iter));
                iter = next;
            }
            break;
        }
        case KN_T_BLOCK: {
            KonBlock* pair = (KonBlock*)node;
            KxList_Push(markTaskQueue, pair->prev);
            KxList_Push(markTaskQueue, pair->next);
            KxList_Push(markTaskQueue, pair->body);
            break;
        }
        case KN_T_QUOTE: {
            KxList_Push(markTaskQueue, CAST_Kon(Quote, node)->inner);
            break;
        }
        case KN_T_QUASIQUOTE: {
            KxList_Push(markTaskQueue, CAST_Kon(Quasiquote, node)->inner);
            break;
        }
        case KN_T_EXPAND: {
            KxList_Push(markTaskQueue, CAST_Kon(Expand, node)->inner);
            break;
        }
        case KN_T_UNQUOTE: {
            KxList_Push(markTaskQueue, CAST_Kon(Unquote, node)->inner);
            break;
        }
        case KN_T_ENV: {
            // BIND TABLE
            KonEnv* env = (KonEnv*)node;
            KxHashTable* tableBindings = env->bindings;
            KxHashTableIter iterBindings = KxHashTable_IterHead(tableBindings);
            while ((KN)iterBindings != KN_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(tableBindings, iterBindings);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(tableBindings, iterBindings));
                iterBindings = next;
            }
            
            KxHashTable* tableDispatchers = env->bindings;
            KxHashTableIter iterDispatchers = KxHashTable_IterHead(tableDispatchers);
            while ((KN)iterDispatchers != KN_NIL) {
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
                while ((KN)iterDirItems != KN_NIL) {
                    KxHashTableIter next = KxHashTable_IterNext(dirItems, iterDirItems);
                    KxList_Push(markTaskQueue, KxHashTable_IterGetVal(dirItems, iterDirItems));
                    iterDirItems = next;
                }
            }
            else {
                KxList_Push(markTaskQueue, slot->value);
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
                while ((KN)iter != KN_NIL) {
                    KxHashTableIter next = KxHashTable_IterNext(table, iter);
                    KxList_Push(markTaskQueue, KxHashTable_IterGetVal(table, iter));
                    iter = next;
                }
            }
            else if (cont->type == KN_CONT_EVAL_CLAUSE_LIST) {
                KxList_Push(markTaskQueue, cont->evalClauseList.subj);
                KxList_Push(markTaskQueue, cont->evalClauseList.restClauses);
            }
            else if (cont->type == KN_CONT_EVAL_CLAUSE_ARGS) {
                KxList_Push(markTaskQueue, cont->evalClauseArgs.subj);
                KxList_Push(markTaskQueue, cont->evalClauseArgs.restArgList);
                KxList_Push(markTaskQueue, cont->evalClauseArgs.evaledArgList);
            }
            else {
                KxList_Push(markTaskQueue, cont->restJobs);
            }

            KxList_Push(markTaskQueue, cont->env);
            KxList_Push(markTaskQueue, cont->cont);
            break;
        }
        case KN_T_PROCEDURE: {
            KonProcedure* proc = (KonProcedure*)node;
            if (proc->type != KN_NATIVE_FUNC && proc->type != KN_NATIVE_OBJ_METHOD) {
                KxList_Push(markTaskQueue, proc->composite.argList);
                KxList_Push(markTaskQueue, proc->composite.body);
                KxList_Push(markTaskQueue, proc->composite.lexicalEnv);
                KxList_Push(markTaskQueue, proc->composite.captureList);
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
void KN_DestroyNode(KonState* kstate, KonBase* node)
{
    if (node == NULL || !KN_IS_POINTER(node)) {
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
            KonSymbol* sym = CAST_Kon(Symbol, node);
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
            KonString* str = CAST_Kon(String, node);
            if (str->string != NULL) {
                KxStringBuffer_Destroy(str->string);
                str->string = NULL;
            }
            break;
        }
        case KN_T_VECTOR: {
            KonVector* vector = CAST_Kon(Vector, node);
            if (vector->vector != NULL) {
                KxVector_Destroy(vector->vector);
                vector->vector = NULL;
            }
            break;
        }
        case KN_T_TABLE: {
            KonTable* table = CAST_Kon(Table, node);
            if (table->table != NULL) {
                KxHashTable_Destroy(table->table);
                table->table = NULL;
            }
            break;
        }
        case KN_T_MAP: {
            KxHashTable* map = CAST_Kon(Map, node)->map;
            if (map != NULL) {
                KxHashTable_Destroy(map);
                map = NULL;
            }
            break;
        }
        case KN_T_CELL: {
            KonCell* cell = CAST_Kon(Cell, node);
            break;
        }
        case KN_T_PARAM: {
            KonParam* table = CAST_Kon(Param, node);
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
        case KN_T_EXPAND: {
            break;
        }
        case KN_T_UNQUOTE: {
            break;
        }
        case KN_T_ENV: {
            
            KonEnv* env = CAST_Kon(Env, node);
            // don't clear root gc
            if ((KN)env->parent == KN_NIL) {
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
