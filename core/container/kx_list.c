/**
 * KxList
 * Copyright (c) 2019 Kong WeiXian
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include "kx_list.h"
#include "../../lib/tbox/tbox.h"



KxList* KxList_Init()
{
    KxList* self = (KxList*)tb_nalloc0(1, sizeof(KxList));
    if (self == NULL) {
        printf("KxList_Init failed\n");
        return NULL;
    }
    self->Length = 0;
    self->Head = KX_LIST_NIL;
    self->Tail = KX_LIST_NIL;
    return self;
}

int KxList_Destroy(KxList* self)
{
    if (self == NULL) {
        return -1;
    }
    KxList_Clear(self);
    tb_free(self);
    return 1;
}

int KxList_Clear(KxList* self)
{
    KxListNode* iter = self->Head;
    while ((klist_val_t)iter != KX_LIST_NIL) {
        KxListNode* next = iter->Next;
        tb_free(iter);
        iter = next;
    }
    return 1;
}

// number of elements
uint32_t KxList_Length(KxList* self)
{
    return self->Length;
}

// get val by index number
klist_val_t KxList_ValAt(KxList* self, int index)
{
    int cursor = 0;
    KxListNode* iter = self->Head;
    while ((klist_val_t)iter != KX_LIST_NIL) {
        KxListNode* next = iter->Next;
        
        if (index == cursor) {
            return iter->Val;
        }

        cursor += 1;
        iter = next;
    }

    // not found
    return KX_LIST_UNDEF;
}

klist_val_t KxList_Head(KxList* self)
{
    if ((klist_val_t)self->Head != KX_LIST_NIL) {
        return self->Head->Val;
    }
    else {
        return KX_LIST_NIL;
    }
}

klist_val_t KxList_Tail(KxList* self)
{
    if ((klist_val_t)self->Tail != KX_LIST_NIL) {
        return self->Tail->Val;
    }
    else {
        return KX_LIST_NIL;
    }
}

KxListNode* KxList_NewNode(klist_val_t value)
{
    KxListNode* node = (KxListNode*)tb_nalloc0(1, sizeof(KxListNode));
    if (node == NULL) {
        return NULL;
    }
    node->Val = value;
    node->Prev = KX_LIST_NIL;
    node->Next = KX_LIST_NIL;
    return node;
}

// add value to tail
int KxList_Push(KxList* self, klist_val_t value)
{
    KxListNode* node = KxList_NewNode(value);
    if (node == NULL) {
        return -1;
    }
    if ((klist_val_t)self->Tail != KX_LIST_NIL) {
        self->Tail->Next = node;
    }
    if (self->Length == 0) {
        // lenth is 0
        self->Head = node;
    }
    node->Prev = self->Tail;
    self->Tail = node;
    self->Length += 1;
    return 1;
}
// get tail and remove
klist_val_t KxList_Pop(KxList* self)
{
    if ((klist_val_t)self->Tail == KX_LIST_NIL) {
        return KX_LIST_UNDEF;
    }
    klist_val_t tailVal = self->Tail->Val;
    KxListNode* newTail = self->Tail->Prev;

    if (self->Length == 1) {
        self->Head = KX_LIST_NIL;
    }

    tb_free(self->Tail);
    if ((klist_val_t)newTail != KX_LIST_NIL) {
        newTail->Next = KX_LIST_NIL;
    }
    
    self->Tail = newTail;
    self->Length -= 1;
    return tailVal;
}

// add a list to tail
int KxList_Append(KxList* self, KxList* other)
{
    int cursor = 0;
    KxListNode* iter = other->Head;
    while ((klist_val_t)iter != KX_LIST_NIL) {
        KxListNode* next = iter->Next;
        
        klist_val_t val = iter->Val;
        KxList_Push(self, val);

        cursor += 1;
        iter = next;
    }
    return 1;
}


// add value to head
int KxList_Unshift(KxList* self, klist_val_t value)
{
    KxListNode* node = KxList_NewNode(value);
    if (node == NULL) {
        return -1;
    }
    if ((klist_val_t)self->Head != KX_LIST_NIL) {
        self->Head->Prev = node;
    }
    if (self->Length == 0) {
        // lenth is 0
        self->Tail = node;
    }
    node->Next = self->Head;
    self->Head = node;
    self->Length += 1;
    return 1;
}

// get head and remove
klist_val_t KxList_Shift(KxList* self)
{
    if ((klist_val_t)self->Head == KX_LIST_NIL) {
        return KX_LIST_UNDEF;
    }
    klist_val_t headVal = self->Head->Val;
    KxListNode* newHead = self->Head->Next;

    if (self->Length == 1) {
        
        self->Tail = KX_LIST_NIL;
    }
    tb_free(self->Head);
    if ((klist_val_t)newHead != KX_LIST_NIL) {
        newHead->Prev = KX_LIST_NIL;
    }
    
    self->Head = newHead;
    self->Length -= 1;
    return headVal;
}

// add a list at head
int KxList_Prepend(KxList* self, KxList* other)
{
    int cursor = 0;
    KxListNode* iter = other->Head;
    while ((klist_val_t)iter != KX_LIST_NIL) {
        KxListNode* next = iter->Next;
        
        klist_val_t val = iter->Val;
        KxList_Unshift(self, val);

        cursor += 1;
        iter = next;
    }
    return 1;
}

// TODO check if is list head or tail
int KxList_InsertBefore(KxList* self, KxListNode* node, klist_val_t value)
{
    if (value == KX_LIST_NIL) {
        // TODO throw exception
        return -1;
    }
    KxListNode* newNode = KxList_NewNode(value);
    if (newNode == NULL) {
        return -1;
    }
    if ((klist_val_t)node->Prev != KX_LIST_NIL) {
        node->Prev->Next = newNode;
    }
    newNode->Next = node;
    newNode->Prev = node->Prev;
    node->Prev = newNode;
    self->Length += 1;
    return 1;
}

// TODO check if is list head or tail
int KxList_InsertAfter(KxList* self, KxListNode* node, klist_val_t value)
{
    if (value == KX_LIST_NIL) {
        // TODO throw exception
        return -1;
    }
    KxListNode* newNode = KxList_NewNode(value);
    if (newNode == NULL) {
        return -1;
    }
    if ((klist_val_t)node->Next != KX_LIST_NIL) {
        node->Next->Prev = newNode;
    }
    
    node->Prev = node;
    newNode->Next = node->Next;
    node->Next = newNode;

    self->Length += 1;
    return 1;
}

int KxList_InsertAt(KxList* self, int index, klist_val_t value)
{
    KxListNode* newNode = KxList_NewNode(value);
    if (newNode == NULL) {
        return -1;
    }

    int cursor = 0;
    KxListNode* iter = self->Head;
    while ((klist_val_t)iter != KX_LIST_NIL) {
        KxListNode* next = iter->Next;
        
        if (index == cursor) {
            KxList_InsertBefore(self, iter, newNode);
        }

        cursor += 1;
        iter = next;
    }

    // not found
    return -1;
}

// del by index, if out of range, do nothing
int KxList_DelNode(KxList* self, KxListNode* node)
{
    if (self->Length == 1 && self->Head == node) {
        
        self->Head = KX_LIST_NIL;
        self->Tail = KX_LIST_NIL;
    }
    else if (self->Head == node) {
        self->Head = node->Next;
    }
    else if (self->Tail == node) {
        self->Tail = node->Prev;
    }
    if ((klist_val_t)node->Next != KX_LIST_NIL) {
        node->Next->Prev = node->Prev;
    }
    if ((klist_val_t)node->Prev != KX_LIST_NIL) {
        node->Prev->Next = node->Next;
    }
    tb_free(node);

    self->Length -= 1;
    return 1;
}

// del at index
int KxList_DelAt(KxList* self, int index)
{
    int cursor = 0;
    KxListNode* iter = self->Head;
    while ((klist_val_t)iter != KX_LIST_NIL) {
        KxListNode* next = iter->Next;
        
        if (index == cursor) {
            KxList_DelNode(self, iter);
            return 1;
        }

        cursor += 1;
        iter = next;
    }

    // not found
    return -1;
}

////
// iterator
KxListNode* KxList_IterHead(KxList* self)
{
    return self->Head;
}

KxListNode* KxList_IterTail(KxList* self)
{
    return self->Tail;
}

bool KxList_IterHasNext(KxListNode* iter)
{
    if (iter == NULL || (klist_val_t)iter == KX_LIST_NIL) {
        return false;
    }
    return ((klist_val_t)iter->Next == KX_LIST_NIL) ? false : true;
}

KxListNode* KxList_IterNext(KxListNode* iter)
{
    return iter->Next;
}

klist_val_t KxList_IterVal(KxListNode* iter)
{
    if ((klist_val_t)iter != KX_LIST_NIL) {
        return iter->Val;
    }
    else {
        return KX_LIST_UNDEF;
    }
}
