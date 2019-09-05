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
    self->length = 0;
    self->head = KX_LIST_NIL;
    self->tail = KX_LIST_NIL;
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
    KxListNode* iter = self->head;
    while ((klist_val_t)iter != KX_LIST_NIL) {
        KxListNode* next = iter->next;
        tb_free(iter);
        iter = next;
    }
    return 1;
}

// number of elements
uint32_t KxList_Length(KxList* self)
{
    return self->length;
}

// get val by index number
klist_val_t KxList_ValAt(KxList* self, int index)
{
    int cursor = 0;
    KxListNode* iter = self->head;
    while ((klist_val_t)iter != KX_LIST_NIL) {
        KxListNode* next = iter->next;
        
        if (index == cursor) {
            return iter->val;
        }

        cursor += 1;
        iter = next;
    }

    // not found
    return KX_LIST_UNDEF;
}

klist_val_t KxList_Head(KxList* self)
{
    if ((klist_val_t)self->head != KX_LIST_NIL) {
        return self->head->val;
    }
    else {
        return KX_LIST_NIL;
    }
}

klist_val_t KxList_Tail(KxList* self)
{
    if ((klist_val_t)self->tail != KX_LIST_NIL) {
        return self->tail->val;
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
    node->val = value;
    node->prev = KX_LIST_NIL;
    node->next = KX_LIST_NIL;
    return node;
}

// add value to tail
int KxList_Push(KxList* self, klist_val_t value)
{
    KxListNode* node = KxList_NewNode(value);
    if (node == NULL) {
        return -1;
    }
    if ((klist_val_t)self->tail != KX_LIST_NIL) {
        self->tail->next = node;
    }
    if (self->length == 0) {
        // lenth is 0
        self->head = node;
    }
    node->prev = self->tail;
    self->tail = node;
    self->length += 1;
    return 1;
}
// get tail and remove
klist_val_t KxList_Pop(KxList* self)
{
    if ((klist_val_t)self->tail == KX_LIST_NIL) {
        return KX_LIST_UNDEF;
    }
    klist_val_t tailVal = self->tail->val;
    KxListNode* newTail = self->tail->prev;

    if (self->length == 1) {
        self->head = KX_LIST_NIL;
    }

    tb_free(self->tail);
    if ((klist_val_t)newTail != KX_LIST_NIL) {
        newTail->next = KX_LIST_NIL;
    }
    
    self->tail = newTail;
    self->length -= 1;
    return tailVal;
}

// add a list to tail
int KxList_Append(KxList* self, KxList* other)
{
    int cursor = 0;
    KxListNode* iter = other->head;
    while ((klist_val_t)iter != KX_LIST_NIL) {
        KxListNode* next = iter->next;
        
        klist_val_t val = iter->val;
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
    if ((klist_val_t)self->head != KX_LIST_NIL) {
        self->head->prev = node;
    }
    if (self->length == 0) {
        // lenth is 0
        self->tail = node;
    }
    node->next = self->head;
    self->head = node;
    self->length += 1;
    return 1;
}

// get head and remove
klist_val_t KxList_Shift(KxList* self)
{
    if ((klist_val_t)self->head == KX_LIST_NIL) {
        return KX_LIST_UNDEF;
    }
    klist_val_t headVal = self->head->val;
    KxListNode* newHead = self->head->next;

    if (self->length == 1) {
        
        self->tail = KX_LIST_NIL;
    }
    tb_free(self->head);
    if ((klist_val_t)newHead != KX_LIST_NIL) {
        newHead->prev = KX_LIST_NIL;
    }
    
    self->head = newHead;
    self->length -= 1;
    return headVal;
}

// add a list at head
int KxList_Prepend(KxList* self, KxList* other)
{
    int cursor = 0;
    KxListNode* iter = other->head;
    while ((klist_val_t)iter != KX_LIST_NIL) {
        KxListNode* next = iter->next;
        
        klist_val_t val = iter->val;
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
    if ((klist_val_t)node->prev != KX_LIST_NIL) {
        node->prev->next = newNode;
    }
    newNode->next = node;
    newNode->prev = node->prev;
    node->prev = newNode;
    self->length += 1;
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
    if ((klist_val_t)node->next != KX_LIST_NIL) {
        node->next->prev = newNode;
    }
    
    node->prev = node;
    newNode->next = node->next;
    node->next = newNode;

    self->length += 1;
    return 1;
}

int KxList_InsertAt(KxList* self, int index, klist_val_t value)
{
    KxListNode* newNode = KxList_NewNode(value);
    if (newNode == NULL) {
        return -1;
    }

    int cursor = 0;
    KxListNode* iter = self->head;
    while ((klist_val_t)iter != KX_LIST_NIL) {
        KxListNode* next = iter->next;
        
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
    if (self->length == 1 && self->head == node) {
        
        self->head = KX_LIST_NIL;
        self->tail = KX_LIST_NIL;
    }
    else if (self->head == node) {
        self->head = node->next;
    }
    else if (self->tail == node) {
        self->tail = node->prev;
    }
    if ((klist_val_t)node->next != KX_LIST_NIL) {
        node->next->prev = node->prev;
    }
    if ((klist_val_t)node->prev != KX_LIST_NIL) {
        node->prev->next = node->next;
    }
    tb_free(node);

    self->length -= 1;
    return 1;
}

// del at index
int KxList_DelAt(KxList* self, int index)
{
    int cursor = 0;
    KxListNode* iter = self->head;
    while ((klist_val_t)iter != KX_LIST_NIL) {
        KxListNode* next = iter->next;
        
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
    return self->head;
}

KxListNode* KxList_IterTail(KxList* self)
{
    return self->tail;
}

bool KxList_IterHasNext(KxListNode* iter)
{
    if (iter == NULL || (klist_val_t)iter == KX_LIST_NIL) {
        return false;
    }
    return ((klist_val_t)iter->next == KX_LIST_NIL) ? false : true;
}

KxListNode* KxList_IterNext(KxListNode* iter)
{
    return iter->next;
}

klist_val_t KxList_IterVal(KxListNode* iter)
{
    if ((klist_val_t)iter != KX_LIST_NIL) {
        return iter->val;
    }
    else {
        return KX_LIST_UNDEF;
    }
}
