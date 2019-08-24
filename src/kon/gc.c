#include "prefix.h"

#include "tbox/tbox.h"
#include "container/kx_hashtable.h"
#include "container/kx_vector.h"
#include "container/kx_list.h"
#include "gc.h"

#define FIRST_HEAP_PTR_SEG_SIZE 512
// #define FIRST_HEAP_PTR_SEG_SIZE 1024 * 8

// 7 ~= 22052
#define MAX_SEGMENT_CNT 7
// #define MAX_SEGMENT_CNT 2

KxList* KON_CreateHeapPtrSegList(KonState* kstate)
{
    KxVector* firstSeg = KxVector_InitWithCapacity(FIRST_HEAP_PTR_SEG_SIZE);
    KxList* segList = KxList_Init();
    KxList_Push(segList, firstSeg);
    return segList;
}

void KON_InitGc(KonState* kstate)
{
    kstate->WriteBarrierGen = KxList_Init();
    kstate->HeapPtrSegs = KON_CreateHeapPtrSegList(kstate);
    kstate->MarkTaskQueue = KxList_Init();
    kstate->SegmentMaxSizeVec = KxVector_InitWithSize(MAX_SEGMENT_CNT);

    unsigned long nextSize = FIRST_HEAP_PTR_SEG_SIZE;
    unsigned long maxObjCnt = 0;

    for (int i = 0; i < MAX_SEGMENT_CNT; i++) {
        maxObjCnt += nextSize;
        // KON_DEBUG("seg vec index %d, size %d\n", i, nextSize);
        KxVector_SetIndex(kstate->SegmentMaxSizeVec, i, KX_VEC_BOX_UINT(nextSize));
        
        // align to n *4
        int notAligned = nextSize * 1.618;
        nextSize = notAligned + 4 - (notAligned % 4);
    }
    // KON_DEBUG("MaxObjCntLimit %d\n", maxObjCnt);
    kstate->MaxObjCntLimit = maxObjCnt;
}

void KON_ShowGcStatics(KonState* kstate)
{
    int barrierObjLength = KxList_Length(kstate->WriteBarrierGen);

    long long totalObjCnt = KON_CurrentObjCount(kstate);
    KON_DEBUG("HeapPtrSegs count : %d, totalObjCnt %lld, barrierObjLength %d\n", KxList_Length(kstate->HeapPtrSegs), totalObjCnt, barrierObjLength);
}

void KON_DestroyGc(KonState* kstate)
{
    KON_ShowGcStatics(kstate);
}

int KON_PushToHeapPtrSeg(KonState* kstate, KxList* heapPtrSegs, KN ptr)
{
    KxListNode* iter = KxList_IterHead(heapPtrSegs);
    // KON_DEBUG("KxList_IterHead(heapPtrSegs) addr %x\n", iter);
    int segIndex = -1;
    while ((KN)iter != KON_NIL) {
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
        KON_DEBUG("error!!! reach gc ptr cnt limit, current seg cnt %d\n", KxList_Length(heapPtrSegs));
        exit(1);
        // return -1;
    }
    // create a new segment
    segIndex += 1;
    int nextSize = KX_VEC_UNBOX_UINT(KxVector_AtIndex(kstate->SegmentMaxSizeVec, segIndex));
    KxVector* nextSeg = KxVector_InitWithCapacity(nextSize);
    KxVector_Push(nextSeg, ptr);
    KON_DEBUG("\n**create a new segment, seg index %d , new seg size %d\n", segIndex, nextSize);

    KxList_Push(heapPtrSegs, nextSeg);

    
    return 1;
}

long long KON_CurrentObjCount(KonState* kstate)
{
    KxListNode* iter = KxList_IterHead(kstate->HeapPtrSegs);
    long long count = 0;
    while ((KN)iter != KON_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KxVector* segment = (KxVector*)KxList_IterVal(iter);
        count += KxVector_Length(segment);

        iter = next;
    }
    return count;
}

bool KON_HasEnoughSegSpace(KonState* kstate, int requireSize)
{
    if (requireSize == 0) {
        return true;
    }
    long long totalObjCnt = KON_CurrentObjCount(kstate);
    long long limit = kstate->MaxObjCntLimit;
    // KON_DEBUG("requireSize %d, current totalObjCnt %d, max limit %d\n", requireSize, totalObjCnt, limit);
    return (limit - totalObjCnt - requireSize) >= 0;
}


void KON_PushWriteBarrierObjsToHeapPtrSeg(KonState* kstate)
{
    klist_val_t konPtr = KxList_Shift(kstate->WriteBarrierGen);

    while (konPtr != KX_LIST_UNDEF) {
        ((KonBase*)konPtr)->GcMarkColor = KON_GC_MARK_WHITE;
        // add to segment
        KON_PushToHeapPtrSeg(kstate, kstate->HeapPtrSegs, konPtr);

        konPtr = KxList_Shift(kstate->WriteBarrierGen);
    }
}

void KON_SwitchContinuation(KonState* kstate, KonContinuation* cont)
{
#if KON_DISABLE_GC
    return;
#endif
    kstate->CurrCont = cont;
    
    // write barrier list size
    int barrierObjLength = KxList_Length(kstate->WriteBarrierGen);
    bool firstTry = KON_HasEnoughSegSpace(kstate, barrierObjLength);
    if (!firstTry) {
        long long totalObjCnt = KON_CurrentObjCount(kstate);
        long long limit = kstate->MaxObjCntLimit;
        KON_DEBUG("requireSize %d, current totalObjCnt %lld, max limit %lld\n", barrierObjLength, totalObjCnt, limit);

        KON_Gc(kstate);
        bool secondTry = KON_HasEnoughSegSpace(kstate, barrierObjLength);

        if (!secondTry) {
            KON_DEBUG("gc failed, reach ptr count limit\n");
            KON_ShowGcStatics(kstate);
            exit(1);
        }
    }
    
    KON_PushWriteBarrierObjsToHeapPtrSeg(kstate);
}

void KON_RecordNewKonNode(KonState* kstate, KN newVal)
{
#if KON_DISABLE_GC
    return;
#endif
    // add the pointers created between two continuation switch
    // to a temp list
    ((KonBase*)newVal)->GcMarkColor = KON_GC_MARK_GRAY;
    KxList_Push(kstate->WriteBarrierGen, newVal);
}

void KON_Gc(KonState* kstate)
{
    KON_DEBUG("\n**trigger gc\n");
    KON_MarkPhase(kstate);
    KON_SweepPhase(kstate);
}

void KON_MarkPhase(KonState* kstate)
{
    if (kstate->CurrCode != NULL) {
        KxList_Push(kstate->MarkTaskQueue, kstate->CurrCode);
    }

    KxListNode* iter = KxList_IterHead(kstate->WriteBarrierGen);
    while ((KN)iter != KON_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KN ptr = KxList_IterVal(iter);

        // add to MarkTaskQueue
        KxList_Push(kstate->MarkTaskQueue, ptr);

        iter = next;
    }

    if (kstate->CurrCont != NULL) {
        KxList_Push(kstate->MarkTaskQueue, kstate->CurrCont);
    }

    KON_Mark(kstate, kstate->MarkTaskQueue, KON_GC_MARK_BLACK);

    // FIXME: this is a workaround
    // to avoid free AST nodes
    // reserve code pointers
    // KxList* astTaskQueue = KxList_Init();
    // KxList_Push(astTaskQueue, kstate->CurrCode);
    // KON_Mark(kstate, astTaskQueue, KON_GC_MARK_BLACK);
    // KxList_Destroy(astTaskQueue);
}

// 1. shift a KonValue
// 2. mark this value to black
// 3. add all children to this queue
void KON_Mark(KonState* kstate, KxList* taskQueue, char color)
{
    while (KxList_Length(kstate->MarkTaskQueue) > 0) {
        KonBase* konPtr = KxList_Shift(kstate->MarkTaskQueue);
 
        KON_MarkNode(konPtr, taskQueue, color);
        
    }
}

void KON_SweepPhase(KonState* kstate)
{

    // mark kstate->WriteBarrierGen objs to gray
    KxListNode* iter = KxList_IterHead(kstate->WriteBarrierGen);
    while ((KN)iter != KON_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KonBase* ptr = (KonBase*)KxList_IterVal(iter);

        ptr->GcMarkColor = KON_GC_MARK_GRAY;

        iter = next;
    }
    
    // mark kstate->HeapPtrSegs objs to white
    // and copy to a new segment

    KON_ResetAndCopyPtrSegList(kstate);
}


void KON_ResetAndCopyPtrSegList(KonState* kstate)
{
    KON_DEBUG("KON_ResetAndCopyPtrSegList\n");
    // shink the segment list
    KxList* newPtrSegList = KON_CreateHeapPtrSegList(kstate);
    
    KxListNode* iter = KxList_IterHead(kstate->HeapPtrSegs);
    
    unsigned long whiteCnt = 0;
    unsigned long grayCnt = 0;
    unsigned long redCnt = 0;
    unsigned long blackCnt = 0;
    
    while ((KN)iter != KON_NIL) {
        KxListNode* next = KxList_IterNext(iter);

        KxVector* segment = (KxVector*)KxList_IterVal(iter);

        int vecLen = KxVector_Length(segment);
        for (int i = 0; i < vecLen; i++) {
            KonBase* konPtr = (KonBase*)KxVector_AtIndex(segment, i);
            if (konPtr->GcMarkColor == KON_GC_MARK_WHITE) {
                whiteCnt += 1;
                KON_DestroyNode(kstate, konPtr);
                continue;
            }
            else if (konPtr->GcMarkColor == KON_GC_MARK_GRAY) {
                grayCnt += 1;
            }
            else if (konPtr->GcMarkColor == KON_GC_MARK_RED) {
                redCnt += 1;
            }
            else if (konPtr->GcMarkColor == KON_GC_MARK_BLACK) {
                blackCnt += 1;
            }
            // reset black node to white
            konPtr->GcMarkColor = KON_GC_MARK_WHITE;
            KON_PushToHeapPtrSeg(kstate, newPtrSegList, (KN)konPtr);
        }

        KxVector_Destroy(segment);
        KxList_DelNode(kstate->HeapPtrSegs, iter);

        iter = next;
    }
    
    KON_DEBUG("ptr color cnt : white %ld, gray %ld, red %ld, black %ld\n", whiteCnt, grayCnt, redCnt, blackCnt);
    
    // free old kstate->HeapPtrSegs
    KxList_Destroy(kstate->HeapPtrSegs);

    kstate->HeapPtrSegs = newPtrSegList;
}


void KON_MarkNode(KonBase* node, KxList* markTaskQueue, char color)
{
    if (node == NULL || !KON_IS_POINTER(node)) {
        return;
    }
    
    // skip, if finished mark or just do marking stuffs
    if (node->GcMarkColor == KON_GC_MARK_BLACK
        || node->GcMarkColor == KON_GC_MARK_RED
    ) {
        return;
    }
    
    
    switch (node->Tag) {
        case KON_T_STATE: {
            break;
        }
        case KON_T_FLONUM: {
            break;
        }
        case KON_T_BIGNUM: {
            break;
        }
        case KON_T_PAIR: {
            KonPair* pair = (KonPair*)node;
            KxList_Push(markTaskQueue, pair->Prev);
            KxList_Push(markTaskQueue, pair->Prev);
            KxList_Push(markTaskQueue, pair->Next);
            KxList_Push(markTaskQueue, pair->Body);
            break;
        }
        case KON_T_SYMBOL: {
            break;
        }
        case KON_T_SYNTAX_MARKER: {
            break;
        }
        case KON_T_BYTES: {
            break;
        }
        case KON_T_STRING: {
            break;
        }
        case KON_T_VECTOR: {
            KxVector* vec = CAST_Kon(Vector, node)->Vector;
            for (int i = 0; i < KxVector_Length(vec); i++) {
                KxList_Push(markTaskQueue, KxVector_AtIndex(vec, i));
            }
            break;
        }
        case KON_T_TABLE: {
            KxHashTable* table = CAST_Kon(Table, node)->Table;
            KxHashTableIter iter = KxHashTable_IterHead(table);
            while ((KN)iter != KON_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(table, iter);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(table, iter));
                iter = next;
            }
            break;
        }
        case KON_T_CELL: {
            KonCell* cell = (KonCell*)node;
            KxList_Push(markTaskQueue, cell->Core);
            KxList_Push(markTaskQueue, cell->Vector);
            KxList_Push(markTaskQueue, cell->Table);
            KxList_Push(markTaskQueue, cell->List);
            break;
        }
        case KON_T_QUOTE: {
            KxList_Push(markTaskQueue, CAST_Kon(Quote, node)->Inner);
            break;
        }
        case KON_T_QUASIQUOTE: {
            KxList_Push(markTaskQueue, CAST_Kon(Quasiquote, node)->Inner);
            break;
        }
        case KON_T_EXPAND: {
            KxList_Push(markTaskQueue, CAST_Kon(Expand, node)->Inner);
            break;
        }
        case KON_T_UNQUOTE: {
            KxList_Push(markTaskQueue, CAST_Kon(Unquote, node)->Inner);
            break;
        }
        case KON_T_ENV: {
            // BIND TABLE
            KonEnv* env = (KonEnv*)node;
            KxHashTable* tableBindings = env->Bindings;
            KxHashTableIter iterBindings = KxHashTable_IterHead(tableBindings);
            while ((KN)iterBindings != KON_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(tableBindings, iterBindings);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(tableBindings, iterBindings));
                iterBindings = next;
            }
            
            KxHashTable* tableDispatchers = env->Bindings;
            KxHashTableIter iterDispatchers = KxHashTable_IterHead(tableDispatchers);
            while ((KN)iterDispatchers != KON_NIL) {
                KxHashTableIter next = KxHashTable_IterNext(tableDispatchers, iterDispatchers);
                KxList_Push(markTaskQueue, KxHashTable_IterGetVal(tableDispatchers, iterDispatchers));
                iterDispatchers = next;
            }

            KxList_Push(markTaskQueue, env->Parent);
            break;
        }
        case KON_T_ACCESSOR: {
            KonAccessor* slot = (KonAccessor*)node;
            if (slot->IsDir) {
                KxList_Push(markTaskQueue, slot->Dir);
            }
            else {
                KxList_Push(markTaskQueue, slot->Value);
            }
            break;
        }
        case KON_T_MSG_DISPATCHER: {
            // TODO
            break;
        }
        case KON_T_CONTINUATION: {
            KonContinuation* cont = (KonContinuation*)node;

            if (cont->Type == KON_CONT_NATIVE_CALLBACK) {
                KxHashTable* table = cont->Native.MemoTable;
                KxHashTableIter iter = KxHashTable_IterHead(table);
                while ((KN)iter != KON_NIL) {
                    KxHashTableIter next = KxHashTable_IterNext(table, iter);
                    KxList_Push(markTaskQueue, KxHashTable_IterGetVal(table, iter));
                    iter = next;
                }
            }
            else if (cont->Type == KON_CONT_EVAL_CLAUSE_LIST) {
                KxList_Push(markTaskQueue, cont->EvalClauseList.Subj);
                KxList_Push(markTaskQueue, cont->EvalClauseList.RestClauses);
            }
            else if (cont->Type == KON_CONT_EVAL_CLAUSE_ARGS) {
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
        case KON_T_PROCEDURE: {
            KonProcedure* proc = (KonProcedure*)node;
            if (proc->Type != KON_NATIVE_FUNC && proc->Type != KON_NATIVE_OBJ_METHOD) {
                KxList_Push(markTaskQueue, proc->Composite.ArgList);
                KxList_Push(markTaskQueue, proc->Composite.Body);
                KxList_Push(markTaskQueue, proc->Composite.LexicalEnv);
            }
            break;
        }
        case KON_T_CPOINTER: {
            break;
        }
        case KON_T_EXCEPTION: {
            break;
        }
        default: {
        }
    }

    node->GcMarkColor = color;
}


// free children and free node
void KON_DestroyNode(KonState* kstate, KonBase* node)
{
    if (node == NULL || !KON_IS_POINTER(node)) {
        return;
    }

    switch (node->Tag) {
        case KON_T_STATE: {
            break;
        }
        case KON_T_FLONUM: {
            break;
        }
        case KON_T_BIGNUM: {
            break;
        }
        case KON_T_PAIR: {
            break;
        }
        case KON_T_SYMBOL: {
            KonSymbol* sym = CAST_Kon(Symbol, node);
            if (sym->Data != NULL) {
                tb_free(sym->Data);
                sym->Data = NULL;
            }
            break;
        }
        case KON_T_SYNTAX_MARKER: {
            break;
        }
        case KON_T_BYTES: {
            break;
        }
        case KON_T_STRING: {
            KonString* str = CAST_Kon(String, node);
            if (str->String != NULL) {
                KxStringBuffer_Destroy(str->String);
                str->String = NULL;
            }
            break;
        }
        case KON_T_VECTOR: {
            KonVector* vector = CAST_Kon(Vector, node);
            if (vector->Vector != NULL) {
                KxVector_Destroy(vector->Vector);
                vector->Vector = NULL;
            }
            break;
        }
        case KON_T_TABLE: {
            KonTable* table = CAST_Kon(Table, node);
            if (table->Table != NULL) {
                KxHashTable_Destroy(table->Table);
                table->Table = NULL;
            }
            break;
        }
        case KON_T_CELL: {
            break;
        }
        case KON_T_QUOTE: {
            break;
        }
        case KON_T_QUASIQUOTE: {
            break;
        }
        case KON_T_EXPAND: {
            break;
        }
        case KON_T_UNQUOTE: {
            break;
        }
        case KON_T_ENV: {
            
            KonEnv* env = CAST_Kon(Env, node);
            // don't clear root gc
            if ((KN)env->Parent == KON_NIL) {
                break;
                
            }
            if (env->Bindings != NULL) {
                KxHashTable_Destroy(env->Bindings);
                env->Bindings = NULL;
            }
            if (env->MsgDispatchers != NULL) {
                KxHashTable_Destroy(env->MsgDispatchers);
                env->MsgDispatchers = NULL;
            }
            
            break;
        }
        case KON_T_ACCESSOR: {
            KonAccessor* slot = (KonAccessor*)node;
            if (slot->IsDir && slot->Dir != NULL) {
                KxHashTable_Destroy(slot->Dir);
                slot->Dir = NULL;
            }
            break;
        }
        case KON_T_MSG_DISPATCHER: {
            KonMsgDispatcher* dispatcher = (KonMsgDispatcher*)node;
            break;
        }
        case KON_T_CONTINUATION: {
            KonContinuation* cont = (KonContinuation*)node;
            if (cont->Type == KON_CONT_NATIVE_CALLBACK && cont->Native.MemoTable) {
                KxHashTable* table = cont->Native.MemoTable;
                KON_DEBUG("destroy memo table, table addr %x", table);
                KxHashTable_PrintKeys(table);
                KxHashTable_Destroy(table);
                cont->Native.MemoTable = NULL;
            }

            break;
        }
        case KON_T_PROCEDURE: {
            break;
        }
        case KON_T_CPOINTER: {
            break;
        }
        case KON_T_EXCEPTION: {
            break;
        }
        default: {
        }
    }
    tb_free(node);
    node = NULL;
}
