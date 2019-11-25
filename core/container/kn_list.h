/**
 * KnList
 * Copyright (c) 2019 Kong WeiXian
 *
 */


#ifndef KnList__H
#define KnList__H 1
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
typedef struct _KnList KnList;
typedef struct _KnListNode KnListNode;
typedef struct _KnListNode* KnListIter;

// a pointer or a fix number
typedef uint64_t klist_val_t;

#define KN_LIST_UNDEF 0x0eL
#define KN_LIST_NIL 0x06L
#define KN_LIST_UKN 0x07L
#define KN_LIST_FIXNUM_BITS 1
#define KN_LIST_FIXNUM_TAG 1
#define KN_LIST_BOX_UINT(n)    ((klist_val_t) ((((klist_val_t)(n))*(klist_val_t)((klist_val_t)1<<KN_LIST_FIXNUM_BITS)) | KN_LIST_FIXNUM_TAG))
#define KN_LIST_UNBOX_UINT(n)   (((klist_val_t)((klist_val_t)(n) & ~KN_LIST_FIXNUM_TAG))/(klist_val_t)((klist_val_t)1<<KN_LIST_FIXNUM_BITS))

struct _KnList {
    uint32_t length;  // how many items stored
    KnListNode* head;
    KnListNode* tail;
};

struct _KnListNode {
    KnListNode* prev;
    KnListNode* next;
    klist_val_t val;
};

KnList* KnList_Init();

int KnList_Destroy(KnList* self);

int KnList_Clear(KnList* self);

// number of elements
uint32_t KnList_Length(KnList* self);

// get val by index number
klist_val_t KnList_ValAt(KnList* self, int index);

klist_val_t KnList_Head(KnList* self);

klist_val_t KnList_Tail(KnList* self);

// push and pop value should not be nil

// add value to tail
int KnList_Push(KnList* self, klist_val_t value);
// get tail and remove
klist_val_t KnList_Pop(KnList* self);
// add a list to tail
int KnList_Append(KnList* self, KnList* other);


// add value to head
int KnList_Unshift(KnList* self, klist_val_t value);
// get head and remove
klist_val_t KnList_Shift(KnList* self);
// add a list at head
int KnList_Prepend(KnList* self, KnList* other);


int KnList_InsertBefore(KnList* self, KnListNode* node, klist_val_t value);
int KnList_InsertAfter(KnList* self, KnListNode* node, klist_val_t value);
int KnList_InsertAt(KnList* self, int index, klist_val_t value);


// del by index, if out of range, do nothing
int KnList_DelNode(KnList* self, KnListNode* node);
// del at index
int KnList_DelAt(KnList* self, int index);


////
// iterator
KnListNode* KnList_IterHead(KnList* self);

KnListNode* KnList_IterTail(KnList* self);

bool KnList_IterHasNext(KnListNode* iter);

KnListNode* KnList_IterNext(KnListNode* iter);

bool KnList_IterHasPrev(KnListNode* iter);
KnListNode* KnList_IterPrev(KnListNode* iter);

klist_val_t KnList_IterVal(KnListNode* iter);



#ifdef __cplusplus
}
#endif

#endif