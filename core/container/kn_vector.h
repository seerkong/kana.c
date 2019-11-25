/**
 * KnVector
 * Copyright (c) 2019 Kong WeiXian
 *
 */

#ifndef KN_VEC_H
#define KN_VEC_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define KN_VEC_DEFAULT_CAPACITY 16
#define KN_VEC_RESIZE_RATIO 1.618

// a pointer or a fix number
typedef uint64_t kvec_val_t;

typedef struct _KnVector KnVector;

#define KN_VEC_UNDEF 0x0eL
#define KN_VEC_NIL 0x06L
#define KN_VEC_UKN 0x07L
#define KN_VEC_FIXNUM_BITS 1
#define KN_VEC_FIXNUM_TAG 1
#define KN_VEC_BOX_UINT(n)    ((kvec_val_t) ((((kvec_val_t)(n))*(kvec_val_t)((kvec_val_t)1<<KN_VEC_FIXNUM_BITS)) | KN_VEC_FIXNUM_TAG))
#define KN_VEC_UNBOX_UINT(n)   (((kvec_val_t)((kvec_val_t)(n) & ~KN_LIST_FIXNUM_TAG))/(kvec_val_t)((kvec_val_t)1<<KN_LIST_FIXNUM_BITS))


struct _KnVector {
    int32_t buffSize;
    // vector first data pos: BuffStart + HeadOffset
    // when call vec shift, offset + 1
    int32_t headOffset; 
    int32_t length;  // how many items stored
    kvec_val_t* buffStart;    // an array of KonHashMapEntry pointers
};

KnVector* KnVector_Init();

// set capacity, but item num is 0
KnVector* KnVector_InitWithCapacity(int32_t initCapacity);

// set capacity, and item num is `size`, default value set to KN_VEC_UKN
KnVector* KnVector_InitWithSize(int32_t size);

int32_t KnVector_Destroy(KnVector* self);

int32_t KnVector_Clear(KnVector* self);

int32_t KnVector_BuffSize(KnVector* self);
int32_t KnVector_Length(KnVector* self);
// how many slots left. BuffSize - HeadOffset - Length
int32_t KnVector_SpaceLeft(KnVector* self);
// is all slots used. BuffSize > Length
bool KnVector_IsFull(KnVector* self);

void KnVector_CheckResize(KnVector* self);
void KnVector_Grow(KnVector* self);
// TODO
void KnVector_Shink(KnVector* self);

int32_t KnVector_Push(KnVector* self, kvec_val_t value);
kvec_val_t KnVector_Pop(KnVector* self);
// add to head
int32_t KnVector_Unshift(KnVector* self, kvec_val_t value);
// get head
kvec_val_t KnVector_Shift(KnVector* self);

kvec_val_t KnVector_Head(KnVector* self);
kvec_val_t KnVector_Tail(KnVector* self);

kvec_val_t KnVector_AtIndex(KnVector* self, int32_t index);

int32_t KnVector_SetIndex(KnVector* self, int32_t index, kvec_val_t value);

#ifdef __cplusplus
}
#endif

#endif