/**
 * KnVector
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

#include "kn_vector.h"
#include "../../lib/tbox/tbox.h"

KnVector* KnVector_Init()
{
    KnVector* self = (KnVector*)tb_nalloc0(1, sizeof(KnVector));
    if (self == NULL) {
        return NULL;
    }
    self->headOffset = 0;
    self->length = 0;
    self->buffSize = 0;
    self->buffStart = NULL;
    return self;
}

// alloc pool, default value set to KN_VEC_UNDEF
kvec_val_t* KnVector_AllocPoolWithCapacity(int32_t capacity)
{
    kvec_val_t* poolStart = (kvec_val_t*)tb_nalloc0(capacity, sizeof(kvec_val_t));
    if (poolStart == NULL) {
        return NULL;
    }
    memset(poolStart, KN_VEC_UNDEF, capacity);
    return poolStart;
}

// set capacity, but item num is 0
KnVector* KnVector_InitWithCapacity(int32_t initCapacity)
{
    KnVector* self = KnVector_Init();
    kvec_val_t* poolStart = KnVector_AllocPoolWithCapacity(initCapacity);
    if (poolStart == NULL) {
        tb_free(self);
        return NULL;
    }
    self->buffStart = poolStart;
    self->buffSize = initCapacity;

    return self;
}

// set capacity, and item num is `size`, default value set to KN_VEC_UKN
KnVector* KnVector_InitWithSize(int32_t size)
{
    KnVector* self = KnVector_InitWithCapacity(size);
    if (self == NULL) {
        return NULL;
    }
    memset(self->buffStart, KN_VEC_UKN, size);
    self->length = size;
    return self;
}

int32_t KnVector_Destroy(KnVector* self)
{
    if (self == NULL) {
        return -1;
    }
    if (self->buffStart != NULL) {
        tb_free(self->buffStart);
    }
    tb_free(self);
    return 1;
}

int32_t KnVector_Clear(KnVector* self)
{
    self->length = 0;
    self->headOffset = 0;
    memset(self->buffStart, KN_VEC_UNDEF, self->buffSize);
    return 1;
}

int32_t KnVector_BuffSize(KnVector* self)
{
    if (self == NULL) {
        return -1;
    }
    return self->buffSize;
}

int32_t KnVector_Length(KnVector* self)
{
    if (self == NULL) {
        return -1;
    }
    return self->length;
}

int32_t KnVector_SpaceLeft(KnVector* self)
{
    return (self->buffSize - self->headOffset - self->length);
}

bool KnVector_IsFull(KnVector* self)
{
    return (self->buffSize - self->length) > 0 ? false : true;
}

void KnVector_CheckResize(KnVector* self)
{
    if ((self->headOffset + self->length) >= self->buffSize) {
        KnVector_Grow(self);
    }
    else {
        // TODO test if need shink
    }
}

void KnVector_Grow(KnVector* self)
{
    int32_t poolSize = (int32_t)(self->buffSize * KN_VEC_RESIZE_RATIO);
    kvec_val_t* poolStart = KnVector_AllocPoolWithCapacity(poolSize);
    if (poolStart == NULL) {
        tb_free(self);
        return;
    }

    int32_t copyIndex = 0;
    for (int32_t i = self->headOffset; i < self->headOffset + self->length; i++) {
        *(poolStart + copyIndex) = *(self->buffStart + i);
        copyIndex += 1;
    }
    self->buffSize = poolSize;
    self->headOffset = 0;
    self->buffStart = poolStart;
}

void KnVector_Shink(KnVector* self)
{

}

int32_t KnVector_Push(KnVector* self, kvec_val_t value)
{
    if (self->buffSize == 0) {
        // first item
        kvec_val_t* poolStart = KnVector_AllocPoolWithCapacity(KN_VEC_DEFAULT_CAPACITY);
        if (poolStart == NULL) {
            tb_free(self);
            return -1;
        }
        self->buffStart = poolStart;
        self->buffSize = KN_VEC_DEFAULT_CAPACITY;
    }
    else {
        KnVector_CheckResize(self);
    }
    kvec_val_t* slotPtr = self->buffStart + self->headOffset + self->length;

    *(slotPtr) = value;
    self->length += 1;
    return 1;
}

kvec_val_t KnVector_Pop(KnVector* self)
{
    kvec_val_t* slotPtr = self->buffStart + self->headOffset + self->length - 1;
    kvec_val_t value = *(slotPtr);
    self->length -= 1;
    return value;
}

// add to head
int32_t KnVector_Unshift(KnVector* self, kvec_val_t value)
{
    if (self->buffSize == 0) {
        // first item
        kvec_val_t* poolStart = KnVector_AllocPoolWithCapacity(KN_VEC_DEFAULT_CAPACITY);
        if (poolStart == NULL) {
            tb_free(self);
            return -1;
        }
        self->buffStart = poolStart;
        self->buffSize = KN_VEC_DEFAULT_CAPACITY;
    }
    else {
        KnVector_CheckResize(self);
    }

    // if have slots at head
    if (self->headOffset > 0) {
        kvec_val_t* slotPtr = self->buffStart + self->headOffset - 1;
        *(slotPtr) = value;
        self->headOffset -= 1;
        self->length += 1;
        return 2;
    }

    // need alloc new pool
    kvec_val_t* poolStart = NULL;
    int32_t poolSize = self->buffSize;
    if ((self->headOffset + self->length) >= self->buffSize) {
        // need grow
        poolSize = (int32_t)(self->buffSize * KN_VEC_RESIZE_RATIO);
        poolStart = KnVector_AllocPoolWithCapacity(poolSize);
        
    }
    else {
        poolStart = KnVector_AllocPoolWithCapacity(poolSize);
    }
    if (poolStart == NULL) {
        tb_free(self);
        return -1;
    }
    // set first to value
    // copy origin rest values to new pool
    *poolStart = value;
    int32_t copyIndex = 1;
    for (int32_t i = self->headOffset; i < self->headOffset + self->length; i++) {
        *(poolStart + copyIndex) = *(self->buffStart + i);
        copyIndex += 1;
    }
    self->buffSize = poolSize;
    self->headOffset = 0;
    self->length += 1;
    self->buffStart = poolStart;
    return 1;
}

// get head
kvec_val_t KnVector_Shift(KnVector* self)
{
    kvec_val_t* slotPtr = self->buffStart + self->headOffset;
    kvec_val_t value = *(slotPtr);
    // move start offset
    self->headOffset += 1;
    self->length -= 1;
    return value;
}

kvec_val_t KnVector_Head(KnVector* self)
{
    return KnVector_AtIndex(self, 0);
}

kvec_val_t KnVector_Tail(KnVector* self)
{
    return KnVector_AtIndex(self, self->length - 1);
}

kvec_val_t KnVector_AtIndex(KnVector* self, int32_t index)
{
    if (index < 0 || index >= self->length) {
        return KN_VEC_UNDEF;
    }
    kvec_val_t* slotPtr = self->buffStart + self->headOffset + index;
    kvec_val_t value = *(slotPtr);
    return value;
}

int32_t KnVector_SetIndex(KnVector* self, int32_t index, kvec_val_t value)
{
    if (index < 0 || index >= self->length) {
        return -1;
    }
    kvec_val_t* slotPtr = self->buffStart + self->headOffset + index;
    *(slotPtr) = value;
    return 1;
}
