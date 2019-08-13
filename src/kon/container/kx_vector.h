/**
 * KxVector
 * Copyright (c) 2019 Kong WeiXian
 *
 */

#ifndef KX_VECTOR_H
#define KX_VECTOR_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define KX_VECTOR_DEFAULT_CAPACITY 16
#define KX_VECTOR_RESIZE_RATIO 1.618

typedef struct _KxVector KxVector;
// typedef void* XN;
typedef volatile union _Kon* XN;

#define KX_VECTOR_UNDEF 0x0e
#define KX_VECTOR_NIL 0x1e
#define KX_VECTOR_UKN 0x2e

struct _KxVector {
    int32_t BuffSize;
    // vector first data pos: BuffStart + HeadOffset
    // when call vec shift, offset + 1
    int32_t HeadOffset; 
    int32_t Length;  // how many items stored
    XN* BuffStart;    // an array of KonHashMapEntry pointers
};

KxVector* KxVector_Init();

// set capacity, but item num is 0
KxVector* KxVector_InitWithCapacity(int32_t initCapacity);

// set capacity, and item num is `size`, default value set to KX_VECTOR_UKN
KxVector* KxVector_InitWithSize(int32_t size);

int32_t KxVector_Destroy(KxVector* self);

int32_t KxVector_Clear(KxVector* self);

int32_t KxVector_BuffSize(KxVector* self);
int32_t KxVector_Length(KxVector* self);

void KxVector_CheckResize(KxVector* self);
void KxVector_Grow(KxVector* self);
void KxVector_Shink(KxVector* self);

int32_t KxVector_Push(KxVector* self, XN value);
XN KxVector_Pop(KxVector* self);
// add to head
int32_t KxVector_Unshift(KxVector* self, XN value);
// get head
XN KxVector_Shift(KxVector* self);

XN KxVector_AtIndex(KxVector* self, int32_t index);

int32_t KxVector_SetIndex(KxVector* self, int32_t index, XN value);

#ifdef __cplusplus
}
#endif

#endif