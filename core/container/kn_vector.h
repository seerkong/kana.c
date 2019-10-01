/**
 * KxVector
 * Copyright (c) 2019 Kong WeiXian
 *
 */

#ifndef KX_VEC_H
#define KX_VEC_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define KX_VEC_DEFAULT_CAPACITY 16
#define KX_VEC_RESIZE_RATIO 1.618

// a pointer or a fix number
// typedef long int kvec_val_t;
typedef uint64_t kvec_val_t;

typedef struct _KxVector KxVector;
// typedef void* XN;
// typedef volatile union _Kon* XN;

#define KX_VEC_UNDEF 0x0eL
#define KX_VEC_NIL 0x06L
#define KX_VEC_UKN 0x07L
#define KX_VEC_FIXNUM_BITS 1
#define KX_VEC_FIXNUM_TAG 1
#define KX_VEC_BOX_UINT(n)    ((kvec_val_t) ((((kvec_val_t)(n))*(kvec_val_t)((kvec_val_t)1<<KX_VEC_FIXNUM_BITS)) | KX_VEC_FIXNUM_TAG))
#define KX_VEC_UNBOX_UINT(n)   (((kvec_val_t)((kvec_val_t)(n) & ~KX_LIST_FIXNUM_TAG))/(kvec_val_t)((kvec_val_t)1<<KX_LIST_FIXNUM_BITS))


struct _KxVector {
    int32_t buffSize;
    // vector first data pos: BuffStart + HeadOffset
    // when call vec shift, offset + 1
    int32_t headOffset; 
    int32_t length;  // how many items stored
    kvec_val_t* buffStart;    // an array of KonHashMapEntry pointers
};

KxVector* KxVector_Init();

// set capacity, but item num is 0
KxVector* KxVector_InitWithCapacity(int32_t initCapacity);

// set capacity, and item num is `size`, default value set to KX_VEC_UKN
KxVector* KxVector_InitWithSize(int32_t size);

int32_t KxVector_Destroy(KxVector* self);

int32_t KxVector_Clear(KxVector* self);

int32_t KxVector_BuffSize(KxVector* self);
int32_t KxVector_Length(KxVector* self);
// how many slots left. BuffSize - HeadOffset - Length
int32_t KxVector_SpaceLeft(KxVector* self);
// is all slots used. BuffSize > Length
bool KxVector_IsFull(KxVector* self);

void KxVector_CheckResize(KxVector* self);
void KxVector_Grow(KxVector* self);
// TODO
void KxVector_Shink(KxVector* self);

int32_t KxVector_Push(KxVector* self, kvec_val_t value);
kvec_val_t KxVector_Pop(KxVector* self);
// add to head
int32_t KxVector_Unshift(KxVector* self, kvec_val_t value);
// get head
kvec_val_t KxVector_Shift(KxVector* self);

kvec_val_t KxVector_Head(KxVector* self);
kvec_val_t KxVector_Tail(KxVector* self);

kvec_val_t KxVector_AtIndex(KxVector* self, int32_t index);

int32_t KxVector_SetIndex(KxVector* self, int32_t index, kvec_val_t value);

#ifdef __cplusplus
}
#endif

#endif