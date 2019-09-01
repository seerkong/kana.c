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
    kstate->WriteBarrierGen = KxList_Init();
    kstate->HeapPtrSegs = KN_CreateHeapPtrSegList(kstate);
    kstate->MarkTaskQueue = KxList_Init();
    kstate->SegmentMaxSizeVec = KxVector_InitWithSize(MAX_SEGMENT_CNT);

    unsigned long nextSize = FIRST_HEAP_PTR_SEG_SIZE;
    unsigned long maxObjCnt = 0;

    for (int i = 0; i < MAX_SEGMENT_CNT; i++) {
        maxObjCnt += nextSize;
        // KN_DEBUG("seg vec index %d, size %d\n", i, nextSize);
        KxVector_SetIndex(kstate->SegmentMaxSizeVec, i, KX_VEC_BOX_UINT(nextSize));
        
        // align to n *4
        int notAligned = nextSize * 1.618;
        nextSize = notAligned + 4 - (notAligned % 4);
    }
    // KN_DEBUG("MaxObjCntLimit %d\n", maxObjCnt);
    kstate->MaxObjCntLimit = maxObjCnt;
    kstate->GcThreshold = maxObjCnt - nextSize / 4;
    kstate->NeedGc = false;
}

void KN_ShowGcStatics(KonState* kstate)
{
    int barrierObjLength = KxList_Length(kstate->WriteBarrierGen);

    long long totalObjCnt = KN_CurrentObjCount(kstate);
    KN_DEBUG("HeapPtrSegs count : %d, totalObjCnt %lld, barrierObjLength %d\n", KxList_Length(kstate->HeapPtrSegs), totalObjCnt, barrierObjLength);
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
    int nextSize = KX_VEC_UNBOX_UINT(KxVector_AtIndex(kstate->SegmentMaxSizeVec, segIndex));
    KxVector* nextSeg = KxVector_InitWithCapacity(nextSize);
    KxVector_Push(nextSeg, ptr);
    KN_DEBUG("\n**create a new segment, seg index %d , new seg size %d\n", segIndex, nextSize);

    KxList_Push(heapPtrSegs, nextSeg);

    
    return 1;
}

long long KN_CurrentObjCount(KonState* kstate)
{
    KxListNode* iter = KxList_IterHead(kstate->HeapPtrSegs);
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
    long long limit = kstate->GcThreshold;
    
    bool needGc = (limit - totalObjCnt - requireSize) >= 0;
    if (!needGc) {
        KN_DEBUG("requireSize %d, current totalObjCnt %d, max limit %d\n", requireSize, totalObjCnt, limit);
        kstate->NeedGc = true;
    }
    
    return needGc;
}


void KN_PushWriteBarrierObjsToHeapPtrSeg(KonState* kstate)
{
    klist_val_t konPtr = KxList_Shift(kstate->WriteBarrierGen);

    while (konPtr != KX_LIST_UNDEF) {
        ((KonBase*)konPtr)->GcMarkColor = KN_GC_MARK_WHITE;
        // add to segment
        KN_PushToHeapPtrSeg(kstate, kstate->HeapPtrSegs, konPtr);

        konPtr = KxList_Shift(kstate->WriteBarrierGen);
    }
}

void KN_SwitchContinuation(KonState* kstate, KonContinuation* cont)
{
#if KN_DISABLE_GC
    return;
#endif
    kstate->CurrCont = cont;
    
    // write barrier list size
    int barrierObjLength = KxList_Length(kstate->WriteBarrierGen);
    bool firstTry = KN_HasEnoughSegSpace(kstate, barrierObjLength);
    if (!firstTry) {
        long long totalObjCnt = KN_CurrentObjCount(kstate);
        long long limit = kstate->MaxObjCntLimit;
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
    ((KonBase*)newVal)->GcMarkColor = KN_GC_MARK_GRAY;
    KxList_Push(kstate->WriteBarrierGen, newVal);
}

// safepoint:
// after function finished
// after a loop body finished
void KN_EnterGcSafepoint(KonState* kstate)
{
    if (kstate->NeedGc) {
        KN_DEBUG("KN_EnterGcSafepoint");
        KN_Gc(kstate);
        kstate->NeedGc = false;
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
    if (kstate->CurrCode != NULL) {
        KxList_Push(kstate->MarkTaskQueue, kstate->CurrCode);
    }

    // pointers which are not set to pointer addr segments should be reserved
    KxListNode* iter = KxList_IterHead(kstate->WriteBarrierGen);
    while ((KN)iter != KN_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KN ptr = KxList_IterVal(iter);

        // add to MarkTaskQueue
        KxList_Push(kstate->MarkTaskQueue, ptr);

        iter = next;
    }

    // msg dispatchers should be reserved
    int vecLen = KxVector_Length(kstate->MsgDispatchers);
    for (int i = 0; i < vecLen; i++) {
        KN dispatcherPtr = (KN)KxVector_AtIndex(kstate->MsgDispatchers, i);
        KxList_Push(kstate->MarkTaskQueue, dispatcherPtr);
    }

    if (kstate->CurrCont != NULL) {
        KxList_Push(kstate->MarkTaskQueue, kstate->CurrCont);
    }

    KN_Mark(kstate, kstate->MarkTaskQueue, KN_GC_MARK_BLACK);

    // FIXME: this is a workaround
    // to avoid free AST nodes
    // reserve code pointers
    // KxList* astTaskQueue = KxList_Init();
    // KxList_Push(astTaskQueue, kstate->CurrCode);
    // KN_Mark(kstate, astTaskQueue, KN_GC_MARK_BLACK);
    // KxList_Destroy(astTaskQueue);
}

// 1. shift a KonValue
// 2. mark this value to black
// 3. add all children to this queue
void KN_Mark(KonState* kstate, KxList* taskQueue, char color)
{
    while (KxList_Length(kstate->MarkTaskQueue) > 0) {
        KonBase* konPtr = KxList_Shift(taskQueue);
        KN_MarkNode(konPtr, taskQueue, color);
        
    }
}

void KN_SweepPhase(KonState* kstate)
{

    // mark kstate->WriteBarrierGen objs to gray
    KxListNode* iter = KxList_IterHead(kstate->WriteBarrierGen);
    while ((KN)iter != KN_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KonBase* ptr = (KonBase*)KxList_IterVal(iter);

        ptr->GcMarkColor = KN_GC_MARK_GRAY;

        iter = next;
    }
    
    // mark kstate->HeapPtrSegs objs to white
    // and copy to a new segment

    KN_ResetAndCopyPtrSegList(kstate);
}


void KN_ResetAndCopyPtrSegList(KonState* kstate)
{
    KN_DEBUG("KN_ResetAndCopyPtrSegList\n");
    // shink the segment list
    KxList* newPtrSegList = KN_CreateHeapPtrSegList(kstate);
    
    KxListNode* iter = KxList_IterHead(kstate->HeapPtrSegs);
    
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
            if (konPtr->GcMarkColor == KN_GC_MARK_WHITE) {
                whiteCnt += 1;
                KN_DestroyNode(kstate, konPtr);
                continue;
            }
            else if (konPtr->GcMarkColor == KN_GC_MARK_GRAY) {
                grayCnt += 1;
            }
            else if (konPtr->GcMarkColor == KN_GC_MARK_RED) {
                redCnt += 1;
            }
            else if (konPtr->GcMarkColor == KN_GC_MARK_BLACK) {
                blackCnt += 1;
            }
            // reset black node to white
            konPtr->GcMarkColor = KN_GC_MARK_WHITE;
            KN_PushToHeapPtrSeg(kstate, newPtrSegList, (KN)konPtr);
        }

        KxVector_Destroy(segment);
        KxList_DelNode(kstate->HeapPtrSegs, iter);

        iter = next;
    }
    
    KN_DEBUG("ptr color cnt : white %ld, gray %ld, red %ld, black %ld\n", whiteCnt, grayCnt, redCnt, blackCnt);
    
    // free old kstate->HeapPtrSegs
    KxList_Destroy(kstate->HeapPtrSegs);

    kstate->HeapPtrSegs = newPtrSegList;
}


void KN_MarkNode(KonBase* node, KxList* markTaskQueue, char color)
{
    if (node == NULL || !KN_IS_POINTER(node)) {
        return;
    }
    
    // skip, if finished mark or just do marking stuffs
    if (node->GcMarkColor == KN_GC_MARK_BLACK
        || node->GcMarkColor == KN_GC_MARK_RED
    ) {
        return;
    }
    
    
    switch (node->Tag) {
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
            KxList_Push(markTaskQueue, pair->Prev);
            KxList_Push(markTaskQueue, pair->Next);
            KxList_Push(markTaskQueue, pair->Body);
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
            KxVector* vec = CAST_Kon(Vector, node)->Vector;
            for (int i = 0; i < KxVector_Length(vec); i++) {
                KxList_Push(markTaskQueue, KxVector_AtIndex(vec, i));
            }
            break;
        }
        case KN_T_TABLE: {
            KxHashTable* table = CAST_Kon(Table, node)->Table;
            KxHashTableIter iter = KxHashTable_IterHead(table);
            while ((KN)iter != KN_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(table, iter);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(table, iter));
                iter = next;
            }
            break;
        }
        case KN_T_MAP: {
            KxHashTable* map = CAST_Kon(Map, node)->Map;
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
            KxList_Push(markTaskQueue, cell->Core);
            KxList_Push(markTaskQueue, cell->Map);
            KxList_Push(markTaskQueue, cell->Table);
            KxList_Push(markTaskQueue, cell->List);
            if (cell->Next != KN_NIL) {
                KxList_Push(markTaskQueue, cell->Next);
            }
            break;
        }
        case KN_T_PARAM: {
            KxHashTable* table = CAST_Kon(Param, node)->Table;
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
            KxList_Push(markTaskQueue, pair->Prev);
            KxList_Push(markTaskQueue, pair->Next);
            KxList_Push(markTaskQueue, pair->Body);
            break;
        }
        case KN_T_QUOTE: {
            KxList_Push(markTaskQueue, CAST_Kon(Quote, node)->Inner);
            break;
        }
        case KN_T_QUASIQUOTE: {
            KxList_Push(markTaskQueue, CAST_Kon(Quasiquote, node)->Inner);
            break;
        }
        case KN_T_EXPAND: {
            KxList_Push(markTaskQueue, CAST_Kon(Expand, node)->Inner);
            break;
        }
        case KN_T_UNQUOTE: {
            KxList_Push(markTaskQueue, CAST_Kon(Unquote, node)->Inner);
            break;
        }
        case KN_T_ENV: {
            // BIND TABLE
            KonEnv* env = (KonEnv*)node;
            KxHashTable* tableBindings = env->Bindings;
            KxHashTableIter iterBindings = KxHashTable_IterHead(tableBindings);
            while ((KN)iterBindings != KN_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(tableBindings, iterBindings);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(tableBindings, iterBindings));
                iterBindings = next;
            }
            
            KxHashTable* tableDispatchers = env->Bindings;
            KxHashTableIter iterDispatchers = KxHashTable_IterHead(tableDispatchers);
            while ((KN)iterDispatchers != KN_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(tableDispatchers, iterDispatchers);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(tableDispatchers, iterDispatchers));
                iterDispatchers = next;
            }

            KxList_Push(markTaskQueue, env->Parent);
            break;
        }
        case KN_T_ACCESSOR: {
            KonAccessor* slot = (KonAccessor*)node;
            if (slot->IsDir) {
                KxHashTable* dirItems = slot->Dir;
                KxHashTableIter iterDirItems = KxHashTable_IterHead(dirItems);
                while ((KN)iterDirItems != KN_NIL) {
                    KxHashTableIter next = KxHashTable_IterNext(dirItems, iterDirItems);
                    KxList_Push(markTaskQueue, KxHashTable_IterGetVal(dirItems, iterDirItems));
                    iterDirItems = next;
                }
            }
            else {
                KxList_Push(markTaskQueue, slot->Value);
            }
            break;
        }
        case KN_T_MSG_DISPATCHER: {
            KonMsgDispatcher* dispatcher = (KonMsgDispatcher*)node;
            KxList_Push(markTaskQueue, dispatcher->OnSymbol);
            KxList_Push(markTaskQueue, dispatcher->OnSyntaxMarker);
            KxList_Push(markTaskQueue, dispatcher->OnMethodCall);
            KxList_Push(markTaskQueue, dispatcher->OnVisitList);
            KxList_Push(markTaskQueue, dispatcher->OnVisitVector);
            KxList_Push(markTaskQueue, dispatcher->OnVisitTable);
            KxList_Push(markTaskQueue, dispatcher->OnVisitCell);
            KxList_Push(markTaskQueue, dispatcher->OnOtherType);
            break;
        }
        case KN_T_CONTINUATION: {
            KonContinuation* cont = (KonContinuation*)node;

            if (cont->Type == KN_CONT_NATIVE_CALLBACK) {
                KxHashTable* table = cont->Native.MemoTable;
                KxHashTableIter iter = KxHashTable_IterHead(table);
                while ((KN)iter != KN_NIL) {
                    KxHashTableIter next = KxHashTable_IterNext(table, iter);
                    KxList_Push(markTaskQueue, KxHashTable_IterGetVal(table, iter));
                    iter = next;
                }
            }
            else if (cont->Type == KN_CONT_EVAL_CLAUSE_LIST) {
                KxList_Push(markTaskQueue, cont->EvalClauseList.Subj);
                KxList_Push(markTaskQueue, cont->EvalClauseList.RestClauses);
            }
            else if (cont->Type == KN_CONT_EVAL_CLAUSE_ARGS) {
                KxList_Push(markTaskQueue, cont->EvalClauseArgs.Subj);
                KxList_Push(markTaskQueue, cont->EvalClauseArgs.RestArgList);
                KxList_Push(markTaskQueue, cont->EvalClauseArgs.EvaledArgList);
            }
            else {
                KxList_Push(markTaskQueue, cont->RestJobs);
            }

            KxList_Push(markTaskQueue, cont->Env);
            KxList_Push(markTaskQueue, cont->Cont);
            break;
        }
        case KN_T_PROCEDURE: {
            KonProcedure* proc = (KonProcedure*)node;
            if (proc->Type != KN_NATIVE_FUNC && proc->Type != KN_NATIVE_OBJ_METHOD) {
                KxList_Push(markTaskQueue, proc->Composite.ArgList);
                KxList_Push(markTaskQueue, proc->Composite.Body);
                KxList_Push(markTaskQueue, proc->Composite.LexicalEnv);
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

    node->GcMarkColor = color;
}


// free children and free node
void KN_DestroyNode(KonState* kstate, KonBase* node)
{
    if (node == NULL || !KN_IS_POINTER(node)) {
        return;
    }

    switch (node->Tag) {
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
            if (sym->Data != NULL) {
                tb_free(sym->Data);
                sym->Data = NULL;
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
            if (str->String != NULL) {
                KxStringBuffer_Destroy(str->String);
                str->String = NULL;
            }
            break;
        }
        case KN_T_VECTOR: {
            KonVector* vector = CAST_Kon(Vector, node);
            if (vector->Vector != NULL) {
                KxVector_Destroy(vector->Vector);
                vector->Vector = NULL;
            }
            break;
        }
        case KN_T_TABLE: {
            KonTable* table = CAST_Kon(Table, node);
            if (table->Table != NULL) {
                KxHashTable_Destroy(table->Table);
                table->Table = NULL;
            }
            break;
        }
        case KN_T_MAP: {
            KxHashTable* map = CAST_Kon(Map, node)->Map;
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
            if (table->Table != NULL) {
                KxHashTable_Destroy(table->Table);
                table->Table = NULL;
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
            if ((KN)env->Parent == KN_NIL) {
                break;
                
            }
            if (env->Bindings != NULL) {
                KxHashTable_Destroy(env->Bindings);
                env->Bindings = NULL;
            }
            break;
        }
        case KN_T_ACCESSOR: {
            KonAccessor* slot = (KonAccessor*)node;
            if (slot->IsDir && slot->Dir != NULL) {
                KxHashTable_Destroy(slot->Dir);
                slot->Dir = NULL;
            }
            break;
        }
        case KN_T_MSG_DISPATCHER: {
            KonMsgDispatcher* dispatcher = (KonMsgDispatcher*)node;
            break;
        }
        case KN_T_CONTINUATION: {
            KonContinuation* cont = (KonContinuation*)node;
            if (cont->Type == KN_CONT_NATIVE_CALLBACK && cont->Native.MemoTable) {
                KxHashTable* table = cont->Native.MemoTable;
                KN_DEBUG("destroy memo table, table addr %x", table);
                KxHashTable_PrintKeys(table);
                KxHashTable_Destroy(table);
                cont->Native.MemoTable = NULL;
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
