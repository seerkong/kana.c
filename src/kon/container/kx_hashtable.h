/**
 * KxHashTable
 * Copyright (c) 2019 Kong WeiXian
 *
 */


#ifndef KxHashTable__H
#define KxHashTable__H 1
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
typedef struct _KxHashTable KxHashTable;
typedef struct _KxHashTableKeyEntry KxHashTableKeyEntry;
typedef struct _KxHashTableValEntry KxHashTableValEntry;
typedef struct _KxHashTableValEntry* KxHashTableIter;
// typedef void* XN;
// typedef volatile union _Kon* XN;
// a pointer or a fix number
typedef long int ktable_val_t;

#define KX_HASH_TABLE_REHASH_THRESHOLD 0.25

#define KX_HASH_TABLE_UNDEF 0x0eL
#define KX_HASH_TABLE_NIL 0x1eL
#define KX_HASH_TABLE_UKN 0x2eL
#define KX_TABLE_FIXNUM_BITS 1
#define KX_TABLE_FIXNUM_TAG 1
#define KX_TABLE_BOX_UINT(n)    ((ktable_val_t) ((((ktable_val_t)(n))*(ktable_val_t)((ktable_val_t)1<<KX_LIST_FIXNUM_BITS)) | KX_LIST_FIXNUM_TAG))
#define KX_TABLE_UNBOX_UINT(n)   (((ktable_val_t)((ktable_val_t)(n) & ~KX_LIST_FIXNUM_TAG))/(ktable_val_t)((ktable_val_t)1<<KX_LIST_FIXNUM_BITS))


struct _KxHashTable {
    uint32_t HashSize;  // hash vector size 2**n
    uint32_t HashMask;  // size - 1
    uint32_t PowerOfTwo;  // 2**n = HashSize, the n

    uint32_t BucketUsed;   // BucketVector used
    uint32_t ItemNum;  // how many items stored
    KxHashTableKeyEntry** Buckets;
    KxHashTableValEntry* ValListHead;
    KxHashTableValEntry* ValListTail;
};

struct _KxHashTableKeyEntry {
    KxHashTableKeyEntry* Prev;
    KxHashTableKeyEntry* Next;
    KxHashTableValEntry* ValEntry;
    uint32_t HashCode;      // hash(Key)
    uint32_t KeyLen;
    uint32_t VerifyCode;    // hash(Key+KeyLen+ HashCode)
};

struct _KxHashTableValEntry {
    KxHashTableValEntry* Prev;
    KxHashTableValEntry* Next;
    char* Key;
    ktable_val_t Val;
};

// the n of 2**n
KxHashTable* KxHashTable_Init(uint32_t powerOfTwo);

int KxHashTable_Destroy(KxHashTable* self);

int KxHashTable_Clear(KxHashTable* self);

// number of elements
uint32_t KxHashTable_Length(KxHashTable* self);

bool KxHashTable_HasKey(KxHashTable* self, const char* key);

// get value by key string
ktable_val_t KxHashTable_AtKey(KxHashTable* self, const char* key);

// get val by index number
ktable_val_t KxHashTable_ValAtIndex(KxHashTable* self, int index);
// get key by index number
const char* KxHashTable_KeyAtIndex(KxHashTable* self, int index);

ktable_val_t KxHashTable_FirstVal(KxHashTable* self);

ktable_val_t KxHashTable_LastVal(KxHashTable* self);


// add value to tail
int KxHashTable_PushVal(KxHashTable* self, ktable_val_t value);
// add key value to tail, 0 fail 1 insert key 2 update key to new value
int KxHashTable_PushKv(KxHashTable* self, char* key, ktable_val_t value);
// add value to head
int KxHashTable_UnshiftVal(KxHashTable* self, ktable_val_t value);
// add key value to head
int KxHashTable_UnshiftKv(KxHashTable* self, const char* key, ktable_val_t value);
// set key, add or update  value, result 0: invalid or fail; 1 insert; 2 update
int KxHashTable_PutKv(KxHashTable* self, const char* key, ktable_val_t value);

// set or update index item's key, if out of range, do nothing
// result 0: invalid or fail; 1 insert; 2 update
int KxHashTable_SetKeyAtIndex(KxHashTable* self, int index, const char* key);
// set value by index, if out of range, do nothing
int KxHashTable_SetValAtIndex(KxHashTable* self, int index, ktable_val_t value);
// set index item's key and value, if out of range, do nothing
int KxHashTable_SetKvAtIndex(KxHashTable* self, int index, const char* key, ktable_val_t value);

// del by key
int KxHashTable_DelByKey(KxHashTable* self, const char* key);
// del by index, if out of range, do nothing
int KxHashTable_DelByIndex(KxHashTable* self, int index);



////
// iterator
KxHashTableIter KxHashTable_IterHead(KxHashTable* self);

KxHashTableIter KxHashTable_IterTail(KxHashTable* self);

bool KxHashTable_IterHasNext(KxHashTable* self, KxHashTableIter iter);

KxHashTableIter KxHashTable_IterNext(KxHashTable* self, KxHashTableIter iter);

bool KxHashTable_IterHasPrev(KxHashTable* self, KxHashTableIter iter);

KxHashTableIter KxHashTable_IterPrev(KxHashTable* self, KxHashTableIter iter);


const char* KxHashTable_IterGetKey(KxHashTable* self, KxHashTableIter iter);

ktable_val_t KxHashTable_IterGetVal(KxHashTable* self, KxHashTableIter iter);

// set iter item key
// result 0: invalid or fail; 1 insert; 2 update
int KxHashTable_IterSetKey(KxHashTable* self, KxHashTableIter iter, char* key);
// set iter item value
int KxHashTable_IterSetVal(KxHashTable* self, KxHashTableIter iter, ktable_val_t value);


// del key and value
int KxHashTable_DelByIter(KxHashTable* self, KxHashTableIter iter, char* key);

// insert value before iter
int KxHashTable_InsertValBeforeIter(KxHashTable* self, KxHashTableIter iter, ktable_val_t value);
// insert k v before iter
int KxHashTable_InsertKvBeforIter(KxHashTable* self, KxHashTableIter iter, char* key, ktable_val_t value);

// insert value after iter
int KxHashTable_InsertValAfterIter(KxHashTable* self, KxHashTableIter iter, ktable_val_t value);
// insert k v after iter
int KxHashTable_InsertKvAfterIter(KxHashTable* self, KxHashTableIter iter, char* key, ktable_val_t value);

KxHashTable* KxHashTable_ShadowClone(KxHashTable* source);

////
// internal
uint32_t KxHashTable_KeyHashIndex(KxHashTable* self, uint32_t hashCode);
void KxHashTable_CheckRehash(KxHashTable* self);
void KxHashTable_PrintKeys(KxHashTable* self);

#ifdef __cplusplus
}
#endif

#endif
