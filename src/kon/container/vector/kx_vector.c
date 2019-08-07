/**
 * KxVector
 * Copyright (c) 2019 Kong WeiXian
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "kx_vector.h"

KxVector* KxVector_Init()
{
    KxVector* self = (KxVector*)calloc(1, sizeof(KxVector));
    if (self == NULL) {
        return NULL;
    }
    self->HeadOffset = 0;
    self->Length = 0;
    self->BuffSize = 0;
    self->BuffStart = NULL;
    return self;
}

// alloc pool
XN* KxVector_AllocPoolWithCapacity(int32_t capacity)
{
    XN* poolStart = (XN*)calloc(capacity, sizeof(XN));
    if (poolStart == NULL) {
        return NULL;
    }
    memset(poolStart, KX_VECTOR_NULL, capacity);
    return poolStart;
}

// set capacity, but item num is 0
KxVector* KxVector_InitWithCapacity(int32_t initCapacity)
{
    KxVector* self = KxVector_Init();
    XN* poolStart = KxVector_AllocPoolWithCapacity(initCapacity);
    if (poolStart == NULL) {
        free(self);
        return NULL;
    }
    self->BuffStart = poolStart;
    self->BuffSize = initCapacity;

    return self;
}

// set capacity, and item num is `size`, default value set to KX_VECTOR_UKN
KxVector* KxVector_InitWithSize(int32_t size)
{
    KxVector* self = KxVector_InitWithCapacity(size);
    if (self == NULL) {
        return NULL;
    }
    memset(self->BuffStart, KX_VECTOR_NULL, size);
    self->Length = size;
    return self;
}

int32_t KxVector_Destroy(KxVector* self)
{
    if (self == NULL) {
        return -1;
    }
    if (self->BuffStart != NULL) {
        free(self->BuffStart);
    }
    free(self);
    return 1;
}

int32_t KxVector_Clear(KxVector* self)
{
    self->Length = 0;
    self->HeadOffset = 0;
    memset(self->BuffStart, KX_VECTOR_NULL, self->BuffSize);
}

int32_t KxVector_BuffSize(KxVector* self)
{
    if (self == NULL) {
        return -1;
    }
    return self->BuffSize;
}

int32_t KxVector_Length(KxVector* self)
{
    if (self == NULL) {
        return -1;
    }
    return self->Length;
}

void KxVector_CheckResize(KxVector* self)
{
    if ((self->HeadOffset + self->Length) >= self->BuffSize) {
        KxVector_Grow(self);
    }
    else {
        // TODO test if need shink
    }
}

void KxVector_Grow(KxVector* self)
{
    int32_t poolSize = (int32_t)(self->BuffSize * KX_VECTOR_RESIZE_RATIO);
    XN* poolStart = KxVector_AllocPoolWithCapacity(poolSize);
    if (poolStart == NULL) {
        free(self);
        return;
    }

    int32_t copyIndex = 0;
    for (int32_t i = self->HeadOffset; i < self->HeadOffset + self->Length; i++) {
        *(poolStart + copyIndex) = *(self->BuffStart + i);
        copyIndex += 1;
    }
    self->BuffSize = poolSize;
    self->HeadOffset = 0;
    self->BuffStart = poolStart;
}

void KxVector_Shink(KxVector* self)
{

}

int32_t KxVector_Push(KxVector* self, XN value)
{
    if (self->BuffSize == 0) {
        // first item
        XN* poolStart = KxVector_AllocPoolWithCapacity(KX_VECTOR_DEFAULT_CAPACITY);
        if (poolStart == NULL) {
            free(self);
            return -1;
        }
        self->BuffStart = poolStart;
        self->BuffSize = KX_VECTOR_DEFAULT_CAPACITY;
    }
    else {
        KxVector_CheckResize(self);
    }
    XN* slotPtr = self->BuffStart + self->HeadOffset + self->Length;

    *(slotPtr) = value;
    self->Length += 1;
    return 1;
}

XN KxVector_Pop(KxVector* self)
{
    XN* slotPtr = self->BuffStart + self->HeadOffset + self->Length - 1;
    XN value = *(slotPtr);
    self->Length -= 1;
    return value;
}

// add to head
int32_t KxVector_Unshift(KxVector* self, XN value)
{
    if (self->BuffSize == 0) {
        // first item
        XN* poolStart = KxVector_AllocPoolWithCapacity(KX_VECTOR_DEFAULT_CAPACITY);
        if (poolStart == NULL) {
            free(self);
            return -1;
        }
        self->BuffStart = poolStart;
        self->BuffSize = KX_VECTOR_DEFAULT_CAPACITY;
    }
    else {
        KxVector_CheckResize(self);
    }

    // if have slots at head
    if (self->HeadOffset > 0) {
        XN* slotPtr = self->BuffStart + self->HeadOffset - 1;
        *(slotPtr) = value;
        self->HeadOffset -= 1;
        self->Length += 1;
        return 2;
    }

    // need alloc new pool
    XN* poolStart = NULL;
    int32_t poolSize = self->BuffSize;
    if ((self->HeadOffset + self->Length) >= self->BuffSize) {
        // need grow
        poolSize = (int32_t)(self->BuffSize * KX_VECTOR_RESIZE_RATIO);
        poolStart = KxVector_AllocPoolWithCapacity(poolSize);
        
    }
    else {
        poolStart = KxVector_AllocPoolWithCapacity(poolSize);
    }
    if (poolStart == NULL) {
        free(self);
        return -1;
    }
    // set first to value
    // copy origin rest values to new pool
    *poolStart = value;
    int32_t copyIndex = 1;
    for (int32_t i = self->HeadOffset; i < self->HeadOffset + self->Length; i++) {
        *(poolStart + copyIndex) = *(self->BuffStart + i);
        copyIndex += 1;
    }
    self->BuffSize = poolSize;
    self->HeadOffset = 0;
    self->Length += 1;
    self->BuffStart = poolStart;
    return 1;
}

// get head
XN KxVector_Shift(KxVector* self)
{
    XN* slotPtr = self->BuffStart + self->HeadOffset;
    XN value = *(slotPtr);
    // move start offset
    self->HeadOffset += 1;
    self->Length -= 1;
    return value;
}

XN KxVector_AtIndex(KxVector* self, int32_t index)
{
    if (index < 0 || index >= self->Length) {
        return KX_VECTOR_NULL;
    }
    XN* slotPtr = self->BuffStart + self->HeadOffset + index;
    XN value = *(slotPtr);
    return value;
}

int32_t KxVector_SetIndex(KxVector* self, int32_t index, XN value)
{
    if (index < 0 || index >= self->Length) {
        return -1;
    }
    XN* slotPtr = self->BuffStart + self->HeadOffset + index;
    *(slotPtr) = value;
    return 1;
}
