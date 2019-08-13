/**
 * KxList
 * Copyright (c) 2019 Kong WeiXian
 *
 */


#ifndef KxList__H
#define KxList__H 1
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
typedef struct _KxList KxList;
typedef struct _KxListNode KxListNode;
typedef struct _KxListNode* KxListIter;

#if 0
typedef void* XN;
#else
typedef volatile union _Kon* XN;
#endif

#define KX_LIST_UNDEF 0x0e
#define KX_LIST_NIL 0x1e
#define KX_LIST_UKN 0x2e

struct _KxList {
    uint32_t Length;  // how many items stored
    KxListNode* Head;
    KxListNode* Tail;
};

struct _KxListNode {
    KxListNode* Prev;
    KxListNode* Next;
    XN Val;
};

KxList* KxList_Init();

int KxList_Destroy(KxList* self);

int KxList_Clear(KxList* self);

// number of elements
uint32_t KxList_Length(KxList* self);

// get val by index number
XN KxList_ValAt(KxList* self, int index);

XN KxList_Head(KxList* self);

XN KxList_Tail(KxList* self);

// push and pop value should not be nil

// add value to tail
int KxList_Push(KxList* self, XN value);
// get tail and remove
XN KxList_Pop(KxList* self);
// add a list to tail
int KxList_Append(KxList* self, KxList* other);


// add value to head
int KxList_Unshift(KxList* self, XN value);
// get head and remove
XN KxList_Shift(KxList* self);
// add a list at head
int KxList_Prepend(KxList* self, KxList* other);


int KxList_InsertBefore(KxList* self, KxListNode* node, XN value);
int KxList_InsertAfter(KxList* self, KxListNode* node, XN value);
int KxList_InsertAt(KxList* self, int index, XN value);


// del by index, if out of range, do nothing
int KxList_DelNode(KxList* self, KxListNode* node);
// del at index
int KxList_DelAt(KxList* self, int index);


////
// iterator
KxListNode* KxList_IterHead(KxList* self);

KxListNode* KxList_IterTail(KxList* self);

bool KxList_IterHasNext(KxListNode* iter);

KxListNode* KxList_IterNext(KxListNode* iter);

XN KxList_IterVal(KxListNode* iter);



#ifdef __cplusplus
}
#endif

#endif