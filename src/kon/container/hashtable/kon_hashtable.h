/**
 * Kon Programming Language
 * Copyright (c) 2019 Kong Weixian
 *
 */


#ifndef KonHashTable__H
#define KonHashTable__H 1
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
typedef struct _KonHashTable KonHashTable;
typedef struct _KonHashTableKeyEntry KonHashTableKeyEntry;
typedef struct _KonHashTableValEntry KonHashTableValEntry;
typedef struct _KonHashTableValEntry* KonHashTableIter;
// typedef void* KN;
typedef volatile union _Kon* KN;

#define KON_HASH_TABLE_REHASH_THRESHOLD 0.25
#define KON_HASH_TABLE_UKN 0x3e     // 62 00111110
#define KON_HASH_TABLE_NULL 0x33e    // 830 11000111110
#define KON_HASH_TABLE_NIL 0x43e    // 1086 10000111110

struct _KonHashTable {
    uint32_t HashSize;  // hash vector size 2**n
    uint32_t HashMask;  // size - 1
    uint32_t PowerOfTwo;  // 2**n = HashSize, the n

    uint32_t BucketUsed;   // BucketVector used
    uint32_t ItemNum;  // how many items stored
    KonHashTableKeyEntry** Buckets;
    KonHashTableValEntry* ValListHead;
    KonHashTableValEntry* ValListTail;
};

struct _KonHashTableKeyEntry {
    KonHashTableKeyEntry* Prev;
    KonHashTableKeyEntry* Next;
    KonHashTableValEntry* ValEntry;
    uint32_t HashCode;      // hash(Key)
    uint32_t KeyLen;
    uint32_t VerifyCode;    // hash(Key+KeyLen+ HashCode)

    char* Key;
};

struct _KonHashTableValEntry {
    KonHashTableValEntry* Prev;
    KonHashTableValEntry* Next;
    KonHashTableKeyEntry* KeyEntry;
    KN Val;
};

// the n of 2**n
KonHashTable* KonHashTable_Init(uint32_t powerOfTwo);

int KonHashTable_Destroy(KonHashTable* self);

int KonHashTable_Clear(KonHashTable* self);

// number of elements
uint32_t KonHashTable_Length(KonHashTable* self);

bool KonHashTable_HasKey(KonHashTable* self, char* key);

// get value by key string
KN KonHashTable_AtKey(KonHashTable* self, char* key);

// get val by index number
KN KonHashTable_ValAtIndex(KonHashTable* self, int index);
// get key by index number
const char* KonHashTable_KeyAtIndex(KonHashTable* self, int index);

KN KonHashTable_FirstVal(KonHashTable* self);

KN KonHashTable_LastVal(KonHashTable* self);


// add value to tail
int KonHashTable_PushVal(KonHashTable* self, KN value);
// add key value to tail, 0 fail 1 insert key 2 update key to new value
int KonHashTable_PushKv(KonHashTable* self, char* key, KN value);
// add value to head
int KonHashTable_UnshiftVal(KonHashTable* self, KN value);
// add key value to head
int KonHashTable_UnshiftKv(KonHashTable* self, char* key, KN value);
// set key, add or update  value, result 0: invalid or fail; 1 insert; 2 update
int KonHashTable_PutKv(KonHashTable* self, char* key, KN value);

// set or update index item's key, if out of range, do nothing
// result 0: invalid or fail; 1 insert; 2 update
int KonHashTable_SetKeyAtIndex(KonHashTable* self, int index, char* key);
// set value by index, if out of range, do nothing
int KonHashTable_SetValAtIndex(KonHashTable* self, int index, KN value);
// set index item's key and value, if out of range, do nothing
int KonHashTable_SetKvAtIndex(KonHashTable* self, int index, char* key, KN value);

// del by key
int KonHashTable_DelByKey(KonHashTable* self, char* key);
// del by index, if out of range, do nothing
int KonHashTable_DelByIndex(KonHashTable* self, int index);



////
// iterator
KonHashTableIter KonHashTable_IterHead(KonHashTable* self);

KonHashTableIter KonHashTable_IterTail(KonHashTable* self);

bool KonHashTable_IterHasNext(KonHashTable* self, KonHashTableIter iter);

KonHashTableIter KonHashTable_IterNext(KonHashTable* self, KonHashTableIter iter);

const char* KonHashTable_IterGetKey(KonHashTable* self, KonHashTableIter iter);

KN KonHashTable_IterGetVal(KonHashTable* self, KonHashTableIter iter);

// set iter item key
// result 0: invalid or fail; 1 insert; 2 update
int KonHashTable_IterSetKey(KonHashTable* self, KonHashTableIter iter, char* key);
// set iter item value
int KonHashTable_IterSetVal(KonHashTable* self, KonHashTableIter iter, KN value);


// del key and value
int KonHashTable_DelByIter(KonHashTable* self, KonHashTableIter iter, char* key);

// insert value before iter
int KonHashTable_InsertValBeforeIter(KonHashTable* self, KonHashTableIter iter, KN value);
// insert k v before iter
int KonHashTable_InsertKvBeforIter(KonHashTable* self, KonHashTableIter iter, char* key, KN value);

// insert value after iter
int KonHashTable_InsertValAfterIter(KonHashTable* self, KonHashTableIter iter, KN value);
// insert k v after iter
int KonHashTable_InsertKvAfterIter(KonHashTable* self, KonHashTableIter iter, char* key, KN value);



////
// internal
uint32_t KonHashTable_KeyHashIndex(KonHashTable* self, uint32_t hashCode);
void KonHashTable_CheckRehash(KonHashTable* self);

#ifdef __cplusplus
}
#endif

#endif