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
#include "../../lib/tbox/tbox.h"

KxVector* KxVector_Init()
{
    KxVector* self = (KxVector*)tb_nalloc0(1, sizeof(KxVector));
    if (self == NULL) {
        return NULL;
    }
    self->HeadOffset = 0;
    self->Length = 0;
    self->BuffSize = 0;
    self->BuffStart = NULL;
    return self;
}

// alloc pool, default value set to KX_VEC_UNDEF
kvec_val_t* KxVector_AllocPoolWithCapacity(int32_t capacity)
{
    kvec_val_t* poolStart = (kvec_val_t*)tb_nalloc0(capacity, sizeof(kvec_val_t));
    if (poolStart == NULL) {
        return NULL;
    }
    memset(poolStart, KX_VEC_UNDEF, capacity);
    return poolStart;
}

// set capacity, but item num is 0
KxVector* KxVector_InitWithCapacity(int32_t initCapacity)
{
    KxVector* self = KxVector_Init();
    kvec_val_t* poolStart = KxVector_AllocPoolWithCapacity(initCapacity);
    if (poolStart == NULL) {
        tb_free(self);
        return NULL;
    }
    self->BuffStart = poolStart;
    self->BuffSize = initCapacity;

    return self;
}

// set capacity, and item num is `size`, default value set to KX_VEC_UKN
KxVector* KxVector_InitWithSize(int32_t size)
{
    KxVector* self = KxVector_InitWithCapacity(size);
    if (self == NULL) {
        return NULL;
    }
    memset(self->BuffStart, KX_VEC_UKN, size);
    self->Length = size;
    return self;
}

int32_t KxVector_Destroy(KxVector* self)
{
    if (self == NULL) {
        return -1;
    }
    if (self->BuffStart != NULL) {
        tb_free(self->BuffStart);
    }
    tb_free(self);
    return 1;
}

int32_t KxVector_Clear(KxVector* self)
{
    self->Length = 0;
    self->HeadOffset = 0;
    memset(self->BuffStart, KX_VEC_UNDEF, self->BuffSize);
    return 1;
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

int32_t KxVector_SpaceLeft(KxVector* self)
{
    return (self->BuffSize - self->HeadOffset - self->Length);
}

bool KxVector_IsFull(KxVector* self)
{
    return (self->BuffSize - self->Length) > 0 ? false : true;
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
    int32_t poolSize = (int32_t)(self->BuffSize * KX_VEC_RESIZE_RATIO);
    kvec_val_t* poolStart = KxVector_AllocPoolWithCapacity(poolSize);
    if (poolStart == NULL) {
        tb_free(self);
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

int32_t KxVector_Push(KxVector* self, kvec_val_t value)
{
    if (self->BuffSize == 0) {
        // first item
        kvec_val_t* poolStart = KxVector_AllocPoolWithCapacity(KX_VEC_DEFAULT_CAPACITY);
        if (poolStart == NULL) {
            tb_free(self);
            return -1;
        }
        self->BuffStart = poolStart;
        self->BuffSize = KX_VEC_DEFAULT_CAPACITY;
    }
    else {
        KxVector_CheckResize(self);
    }
    kvec_val_t* slotPtr = self->BuffStart + self->HeadOffset + self->Length;

    *(slotPtr) = value;
    self->Length += 1;
    return 1;
}

kvec_val_t KxVector_Pop(KxVector* self)
{
    kvec_val_t* slotPtr = self->BuffStart + self->HeadOffset + self->Length - 1;
    kvec_val_t value = *(slotPtr);
    self->Length -= 1;
    return value;
}

// add to head
int32_t KxVector_Unshift(KxVector* self, kvec_val_t value)
{
    if (self->BuffSize == 0) {
        // first item
        kvec_val_t* poolStart = KxVector_AllocPoolWithCapacity(KX_VEC_DEFAULT_CAPACITY);
        if (poolStart == NULL) {
            tb_free(self);
            return -1;
        }
        self->BuffStart = poolStart;
        self->BuffSize = KX_VEC_DEFAULT_CAPACITY;
    }
    else {
        KxVector_CheckResize(self);
    }

    // if have slots at head
    if (self->HeadOffset > 0) {
        kvec_val_t* slotPtr = self->BuffStart + self->HeadOffset - 1;
        *(slotPtr) = value;
        self->HeadOffset -= 1;
        self->Length += 1;
        return 2;
    }

    // need alloc new pool
    kvec_val_t* poolStart = NULL;
    int32_t poolSize = self->BuffSize;
    if ((self->HeadOffset + self->Length) >= self->BuffSize) {
        // need grow
        poolSize = (int32_t)(self->BuffSize * KX_VEC_RESIZE_RATIO);
        poolStart = KxVector_AllocPoolWithCapacity(poolSize);
        
    }
    else {
        poolStart = KxVector_AllocPoolWithCapacity(poolSize);
    }
    if (poolStart == NULL) {
        tb_free(self);
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
kvec_val_t KxVector_Shift(KxVector* self)
{
    kvec_val_t* slotPtr = self->BuffStart + self->HeadOffset;
    kvec_val_t value = *(slotPtr);
    // move start offset
    self->HeadOffset += 1;
    self->Length -= 1;
    return value;
}

kvec_val_t KxVector_Head(KxVector* self)
{
    return KxVector_AtIndex(self, 0);
}

kvec_val_t KxVector_Tail(KxVector* self)
{
    return KxVector_AtIndex(self, self->Length - 1);
}

kvec_val_t KxVector_AtIndex(KxVector* self, int32_t index)
{
    if (index < 0 || index >= self->Length) {
        return KX_VEC_UNDEF;
    }
    kvec_val_t* slotPtr = self->BuffStart + self->HeadOffset + index;
    kvec_val_t value = *(slotPtr);
    return value;
}

int32_t KxVector_SetIndex(KxVector* self, int32_t index, kvec_val_t value)
{
    if (index < 0 || index >= self->Length) {
        return -1;
    }
    kvec_val_t* slotPtr = self->BuffStart + self->HeadOffset + index;
    *(slotPtr) = value;
    return 1;
}
