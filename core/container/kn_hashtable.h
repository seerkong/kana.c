/**
 * KnHashTable
 * Copyright (c) 2019 Kong WeiXian
 *
 */


#ifndef KnHashTable__H
#define KnHashTable__H 1
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
typedef struct _KnHashTable KnHashTable;
typedef struct _KnHashTableKeyEntry KnHashTableKeyEntry;
typedef struct _KnHashTableValEntry KnHashTableValEntry;
typedef struct _KnHashTableValEntry* KnHashTableIter;

typedef uint64_t ktable_val_t;

#define KN_HASH_TABLE_REHASH_THRESHOLD 0.25

#define KN_HASH_TABLE_UNDEF 0x0eL
#define KN_HASH_TABLE_NIL 0x06L
#define KN_HASH_TABLE_UKN 0x07L
#define KN_TABLE_FIXNUM_BITS 1
#define KN_TABLE_FIXNUM_TAG 1
#define KN_TABLE_BOX_UINT(n)    ((ktable_val_t) ((((ktable_val_t)(n))*(ktable_val_t)((ktable_val_t)1<<KN_LIST_FIXNUM_BITS)) | KN_LIST_FIXNUM_TAG))
#define KN_TABLE_UNBOX_UINT(n)   (((ktable_val_t)((ktable_val_t)(n) & ~KN_LIST_FIXNUM_TAG))/(ktable_val_t)((ktable_val_t)1<<KN_LIST_FIXNUM_BITS))


struct _KnHashTable {
    uint32_t hashSize;  // hash vector size 2**n
    uint32_t hashMask;  // size - 1
    uint32_t powerOfTwo;  // 2**n = HashSize, the n

    uint32_t bucketUsed;   // BucketVector used
    uint32_t itemNum;  // how many items stored
    KnHashTableKeyEntry** buckets;
    KnHashTableValEntry* valListHead;
    KnHashTableValEntry* valListTail;
};

struct _KnHashTableKeyEntry {
    KnHashTableKeyEntry* prev;
    KnHashTableKeyEntry* next;
    KnHashTableValEntry* valEntry;
    uint32_t hashCode;      // hash(Key)
    uint32_t keyLen;
    uint32_t verifyCode;    // hash(Key+KeyLen+ HashCode)
};

struct _KnHashTableValEntry {
    KnHashTableValEntry* prev;
    KnHashTableValEntry* next;
    char* key;
    ktable_val_t val;
};

// the n of 2**n
KnHashTable* KnHashTable_Init(uint32_t powerOfTwo);

int KnHashTable_Destroy(KnHashTable* self);

int KnHashTable_Clear(KnHashTable* self);

// number of elements
uint32_t KnHashTable_Length(KnHashTable* self);

bool KnHashTable_HasKey(KnHashTable* self, const char* key);

// get value by key string
ktable_val_t KnHashTable_AtKey(KnHashTable* self, const char* key);

// get val by index number
ktable_val_t KnHashTable_ValAtIndex(KnHashTable* self, int index);
// get key by index number
const char* KnHashTable_KeyAtIndex(KnHashTable* self, int index);

ktable_val_t KnHashTable_FirstVal(KnHashTable* self);

ktable_val_t KnHashTable_LastVal(KnHashTable* self);


// add value to tail
int KnHashTable_PushVal(KnHashTable* self, ktable_val_t value);
// add key value to tail, 0 fail 1 insert key 2 update key to new value
int KnHashTable_PushKv(KnHashTable* self, char* key, ktable_val_t value);
// add value to head
int KnHashTable_UnshiftVal(KnHashTable* self, ktable_val_t value);
// add key value to head
int KnHashTable_UnshiftKv(KnHashTable* self, const char* key, ktable_val_t value);
// set key, add or update  value, result 0: invalid or fail; 1 insert; 2 update
int KnHashTable_PutKv(KnHashTable* self, const char* key, ktable_val_t value);

// set or update index item's key, if out of range, do nothing
// result 0: invalid or fail; 1 insert; 2 update
int KnHashTable_SetKeyAtIndex(KnHashTable* self, int index, const char* key);
// set value by index, if out of range, do nothing
int KnHashTable_SetValAtIndex(KnHashTable* self, int index, ktable_val_t value);
// set index item's key and value, if out of range, do nothing
int KnHashTable_SetKvAtIndex(KnHashTable* self, int index, const char* key, ktable_val_t value);

// del by key
int KnHashTable_DelByKey(KnHashTable* self, const char* key);
// del by index, if out of range, do nothing
int KnHashTable_DelByIndex(KnHashTable* self, int index);



////
// iterator
KnHashTableIter KnHashTable_IterHead(KnHashTable* self);

KnHashTableIter KnHashTable_IterTail(KnHashTable* self);

bool KnHashTable_IterHasNext(KnHashTable* self, KnHashTableIter iter);

KnHashTableIter KnHashTable_IterNext(KnHashTable* self, KnHashTableIter iter);

bool KnHashTable_IterHasPrev(KnHashTable* self, KnHashTableIter iter);

KnHashTableIter KnHashTable_IterPrev(KnHashTable* self, KnHashTableIter iter);


const char* KnHashTable_IterGetKey(KnHashTable* self, KnHashTableIter iter);

ktable_val_t KnHashTable_IterGetVal(KnHashTable* self, KnHashTableIter iter);

// set iter item key
// result 0: invalid or fail; 1 insert; 2 update
int KnHashTable_IterSetKey(KnHashTable* self, KnHashTableIter iter, char* key);
// set iter item value
int KnHashTable_IterSetVal(KnHashTable* self, KnHashTableIter iter, ktable_val_t value);


// del key and value
int KnHashTable_DelByIter(KnHashTable* self, KnHashTableIter iter, char* key);

// insert value before iter
int KnHashTable_InsertValBeforeIter(KnHashTable* self, KnHashTableIter iter, ktable_val_t value);
// insert k v before iter
int KnHashTable_InsertKvBeforIter(KnHashTable* self, KnHashTableIter iter, char* key, ktable_val_t value);

// insert value after iter
int KnHashTable_InsertValAfterIter(KnHashTable* self, KnHashTableIter iter, ktable_val_t value);
// insert k v after iter
int KnHashTable_InsertKvAfterIter(KnHashTable* self, KnHashTableIter iter, char* key, ktable_val_t value);

KnHashTable* KnHashTable_ShadowClone(KnHashTable* source);

////
// internal
uint32_t KnHashTable_KeyHashIndex(KnHashTable* self, uint32_t hashCode);
void KnHashTable_CheckRehash(KnHashTable* self);
void KnHashTable_PrintKeys(KnHashTable* self);

#ifdef __cplusplus
}
#endif

#endif
