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

// a pointer or a fix number
typedef long int klist_val_t;

#define KX_LIST_UNDEF 0x0eL
#define KX_LIST_NIL 0x1eL
#define KX_LIST_UKN 0x2eL
#define KX_LIST_FIXNUM_BITS 1
#define KX_LIST_FIXNUM_TAG 1
#define KX_LIST_BOX_UINT(n)    ((klist_val_t) ((((klist_val_t)(n))*(klist_val_t)((klist_val_t)1<<KX_LIST_FIXNUM_BITS)) | KX_LIST_FIXNUM_TAG))
#define KX_LIST_UNBOX_UINT(n)   (((klist_val_t)((klist_val_t)(n) & ~KX_LIST_FIXNUM_TAG))/(klist_val_t)((klist_val_t)1<<KX_LIST_FIXNUM_BITS))

struct _KxList {
    uint32_t Length;  // how many items stored
    KxListNode* Head;
    KxListNode* Tail;
};

struct _KxListNode {
    KxListNode* Prev;
    KxListNode* Next;
    klist_val_t Val;
};

KxList* KxList_Init();

int KxList_Destroy(KxList* self);

int KxList_Clear(KxList* self);

// number of elements
uint32_t KxList_Length(KxList* self);

// get val by index number
klist_val_t KxList_ValAt(KxList* self, int index);

klist_val_t KxList_Head(KxList* self);

klist_val_t KxList_Tail(KxList* self);

// push and pop value should not be nil

// add value to tail
int KxList_Push(KxList* self, klist_val_t value);
// get tail and remove
klist_val_t KxList_Pop(KxList* self);
// add a list to tail
int KxList_Append(KxList* self, KxList* other);


// add value to head
int KxList_Unshift(KxList* self, klist_val_t value);
// get head and remove
klist_val_t KxList_Shift(KxList* self);
// add a list at head
int KxList_Prepend(KxList* self, KxList* other);


int KxList_InsertBefore(KxList* self, KxListNode* node, klist_val_t value);
int KxList_InsertAfter(KxList* self, KxListNode* node, klist_val_t value);
int KxList_InsertAt(KxList* self, int index, klist_val_t value);


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

klist_val_t KxList_IterVal(KxListNode* iter);



#ifdef __cplusplus
}
#endif

#endif