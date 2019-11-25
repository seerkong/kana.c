/**
 * KnList
 * Copyright (c) 2019 Kong WeiXian
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include "kn_list.h"
#include "../../lib/tbox/tbox.h"



KnList* KnList_Init()
{
    KnList* self = (KnList*)tb_nalloc0(1, sizeof(KnList));
    if (self == NULL) {
        printf("KnList_Init failed\n");
        return NULL;
    }
    self->length = 0;
    self->head = KN_LIST_NIL;
    self->tail = KN_LIST_NIL;
    return self;
}

int KnList_Destroy(KnList* self)
{
    if (self == NULL) {
        return -1;
    }
    KnList_Clear(self);
    tb_free(self);
    return 1;
}

int KnList_Clear(KnList* self)
{
    KnListNode* iter = self->head;
    while ((klist_val_t)iter != KN_LIST_NIL) {
        KnListNode* next = iter->next;
        tb_free(iter);
        iter = next;
    }
    return 1;
}

// number of elements
uint32_t KnList_Length(KnList* self)
{
    return self->length;
}

// get val by index number
klist_val_t KnList_ValAt(KnList* self, int index)
{
    int cursor = 0;
    KnListNode* iter = self->head;
    while ((klist_val_t)iter != KN_LIST_NIL) {
        KnListNode* next = iter->next;
        
        if (index == cursor) {
            return iter->val;
        }

        cursor += 1;
        iter = next;
    }

    // not found
    return KN_LIST_UNDEF;
}

klist_val_t KnList_Head(KnList* self)
{
    if ((klist_val_t)self->head != KN_LIST_NIL) {
        return self->head->val;
    }
    else {
        return KN_LIST_NIL;
    }
}

klist_val_t KnList_Tail(KnList* self)
{
    if ((klist_val_t)self->tail != KN_LIST_NIL) {
        return self->tail->val;
    }
    else {
        return KN_LIST_NIL;
    }
}

KnListNode* KnList_NewNode(klist_val_t value)
{
    KnListNode* node = (KnListNode*)tb_nalloc0(1, sizeof(KnListNode));
    if (node == NULL) {
        return NULL;
    }
    node->val = value;
    node->prev = KN_LIST_NIL;
    node->next = KN_LIST_NIL;
    return node;
}

// add value to tail
int KnList_Push(KnList* self, klist_val_t value)
{
    KnListNode* node = KnList_NewNode(value);
    if (node == NULL) {
        return -1;
    }
    if ((klist_val_t)self->tail != KN_LIST_NIL) {
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
klist_val_t KnList_Pop(KnList* self)
{
    if ((klist_val_t)self->tail == KN_LIST_NIL) {
        return KN_LIST_UNDEF;
    }
    klist_val_t tailVal = self->tail->val;
    KnListNode* newTail = self->tail->prev;

    if (self->length == 1) {
        self->head = KN_LIST_NIL;
    }

    tb_free(self->tail);
    if ((klist_val_t)newTail != KN_LIST_NIL) {
        newTail->next = KN_LIST_NIL;
    }
    
    self->tail = newTail;
    self->length -= 1;
    return tailVal;
}

// add a list to tail
int KnList_Append(KnList* self, KnList* other)
{
    int cursor = 0;
    KnListNode* iter = other->head;
    while ((klist_val_t)iter != KN_LIST_NIL) {
        KnListNode* next = iter->next;
        
        klist_val_t val = iter->val;
        KnList_Push(self, val);

        cursor += 1;
        iter = next;
    }
    return 1;
}


// add value to head
int KnList_Unshift(KnList* self, klist_val_t value)
{
    KnListNode* node = KnList_NewNode(value);
    if (node == NULL) {
        return -1;
    }
    if ((klist_val_t)self->head != KN_LIST_NIL) {
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
klist_val_t KnList_Shift(KnList* self)
{
    if ((klist_val_t)self->head == KN_LIST_NIL) {
        return KN_LIST_UNDEF;
    }
    klist_val_t headVal = self->head->val;
    KnListNode* newHead = self->head->next;

    if (self->length == 1) {
        
        self->tail = KN_LIST_NIL;
    }
    tb_free(self->head);
    if ((klist_val_t)newHead != KN_LIST_NIL) {
        newHead->prev = KN_LIST_NIL;
    }
    
    self->head = newHead;
    self->length -= 1;
    return headVal;
}

// add a list at head
int KnList_Prepend(KnList* self, KnList* other)
{
    int cursor = 0;
    KnListNode* iter = other->head;
    while ((klist_val_t)iter != KN_LIST_NIL) {
        KnListNode* next = iter->next;
        
        klist_val_t val = iter->val;
        KnList_Unshift(self, val);

        cursor += 1;
        iter = next;
    }
    return 1;
}

// TODO check if is list head or tail
int KnList_InsertBefore(KnList* self, KnListNode* node, klist_val_t value)
{
    if (value == KN_LIST_NIL) {
        // TODO throw exception
        return -1;
    }
    KnListNode* newNode = KnList_NewNode(value);
    if (newNode == NULL) {
        return -1;
    }
    if ((klist_val_t)node->prev != KN_LIST_NIL) {
        node->prev->next = newNode;
    }
    newNode->next = node;
    newNode->prev = node->prev;
    node->prev = newNode;
    self->length += 1;
    return 1;
}

// TODO check if is list head or tail
int KnList_InsertAfter(KnList* self, KnListNode* node, klist_val_t value)
{
    if (value == KN_LIST_NIL) {
        // TODO throw exception
        return -1;
    }
    KnListNode* newNode = KnList_NewNode(value);
    if (newNode == NULL) {
        return -1;
    }
    if ((klist_val_t)node->next != KN_LIST_NIL) {
        node->next->prev = newNode;
    }
    
    node->prev = node;
    newNode->next = node->next;
    node->next = newNode;

    self->length += 1;
    return 1;
}

int KnList_InsertAt(KnList* self, int index, klist_val_t value)
{
    KnListNode* newNode = KnList_NewNode(value);
    if (newNode == NULL) {
        return -1;
    }

    int cursor = 0;
    KnListNode* iter = self->head;
    while ((klist_val_t)iter != KN_LIST_NIL) {
        KnListNode* next = iter->next;
        
        if (index == cursor) {
            KnList_InsertBefore(self, iter, newNode);
        }

        cursor += 1;
        iter = next;
    }

    // not found
    return -1;
}

// del by index, if out of range, do nothing
int KnList_DelNode(KnList* self, KnListNode* node)
{
    if (self->length == 1 && self->head == node) {
        
        self->head = KN_LIST_NIL;
        self->tail = KN_LIST_NIL;
    }
    else if (self->head == node) {
        self->head = node->next;
    }
    else if (self->tail == node) {
        self->tail = node->prev;
    }
    if ((klist_val_t)node->next != KN_LIST_NIL) {
        node->next->prev = node->prev;
    }
    if ((klist_val_t)node->prev != KN_LIST_NIL) {
        node->prev->next = node->next;
    }
    tb_free(node);

    self->length -= 1;
    return 1;
}

// del at index
int KnList_DelAt(KnList* self, int index)
{
    int cursor = 0;
    KnListNode* iter = self->head;
    while ((klist_val_t)iter != KN_LIST_NIL) {
        KnListNode* next = iter->next;
        
        if (index == cursor) {
            KnList_DelNode(self, iter);
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
KnListNode* KnList_IterHead(KnList* self)
{
    return self->head;
}

KnListNode* KnList_IterTail(KnList* self)
{
    return self->tail;
}

bool KnList_IterHasNext(KnListNode* iter)
{
    if (iter == NULL || (klist_val_t)iter == KN_LIST_NIL) {
        return false;
    }
    return ((klist_val_t)iter->next == KN_LIST_NIL) ? false : true;
}

KnListNode* KnList_IterNext(KnListNode* iter)
{
    return iter->next;
}

bool KnList_IterHasPrev(KnListNode* iter)
{
    if (iter == NULL || (klist_val_t)iter == KN_LIST_NIL) {
        return false;
    }
    return ((klist_val_t)iter->prev == KN_LIST_NIL) ? false : true;
}

KnListNode* KnList_IterPrev(KnListNode* iter)
{
    return iter->prev;
}

klist_val_t KnList_IterVal(KnListNode* iter)
{
    if ((klist_val_t)iter != KN_LIST_NIL) {
        return iter->val;
    }
    else {
        return KN_LIST_UNDEF;
    }
}
