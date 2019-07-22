/**
 * Kon Programming Language
 * Copyright (c) 2019 Kong Weixian
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "murmurhash.h"
#include "kon_hashtable.h"

#define KON_HASH_INDEX_SEED 31
#define KON_HASH_VERIFY_SEED 25

KonHashTable* KonHashTable_Init(uint32_t powerOfTwo)
{
    KonHashTable* self = (KonHashTable*)calloc(1, sizeof(KonHashTable));
    if (self == NULL) {
        return NULL;
    }
    
    uint32_t hashSize = 2 << powerOfTwo;
    self->HashSize = hashSize;
    self->HashMask = hashSize - 1;
    self->PowerOfTwo = powerOfTwo;
    self->BucketUsed = 0;
    self->ItemNum = 0;
    self->ValListHead = KON_HASH_TABLE_NIL;
    self->ValListTail = KON_HASH_TABLE_NIL;

    KonHashTableKeyEntry** buckets = (KonHashTableKeyEntry**)calloc(hashSize, sizeof(void*));
    if (buckets == NULL) {
        free(self);
        return NULL;
    }

    for (uint32_t i = 0; i < hashSize; i++) {
        *(buckets + i) = KON_HASH_TABLE_NIL;
    }
    self->Buckets = buckets;

    return self;
}

int KonHashTable_Destroy(KonHashTable* self)
{
    if (self == NULL) {
        return -1;
    }
    KonHashTable_Clear(self);
    free(self);
    return 1;
}

// clear keys in this bucket and set value's key to NIL
void KonHashTable_ClearBucketKeys(KonHashTable* self, KonHashTableKeyEntry* bucket)
{
    if (bucket == KON_HASH_TABLE_NIL) {
        return;
    }
    KonHashTableKeyEntry* iter = bucket;
    while (iter != KON_HASH_TABLE_NIL) {
        KonHashTableKeyEntry* next = iter->Next;
        if (iter->ValEntry != KON_HASH_TABLE_NIL) {
            iter->ValEntry->KeyEntry = KON_HASH_TABLE_NIL;
        }
        free(iter->Key);
        free(iter);
        iter = next;
    }
}

void KonHashTable_ClearKeys(KonHashTable* self)
{
    KonHashTableKeyEntry** buckets = self->Buckets;

    self->BucketUsed = 0;
    for (uint32_t i = 0; i < self->HashSize; i++) {
        KonHashTableKeyEntry* bucket = (KonHashTableKeyEntry*)(*(buckets + i));
        KonHashTable_ClearBucketKeys(self, bucket);
        *(buckets + i) = KON_HASH_TABLE_NIL;
    }
}

// only clear one key entry, and update prev, next ref
void KonHashTable_DelKeyEntry(KonHashTable* self, KonHashTableKeyEntry* keyEntry) {
    if (keyEntry == KON_HASH_TABLE_NIL) {
        return;
    }

    // if the entry is the bucket first, update bucket
    uint32_t hashIndex = KonHashTable_KeyHashIndex(self, keyEntry->HashCode);
    if (self->Buckets[hashIndex] == keyEntry) {
        self->Buckets[hashIndex] = keyEntry->Next;
        if (keyEntry->Next == KON_HASH_TABLE_NIL) {
            self->BucketUsed -= 1;
        }
    }
    
    if (keyEntry->Next != KON_HASH_TABLE_NIL) {
        keyEntry->Next->Prev = keyEntry->Prev;
    }

    if (keyEntry->Prev != KON_HASH_TABLE_NIL) {
        keyEntry->Prev->Next = keyEntry->Next;
    }

    free(keyEntry->Key);
    free(keyEntry);

    KonHashTable_CheckRehash(self);
}

// assume all keys are cleared
// !!! must be called after KonHashTable_ClearKeys
void KonHashTable_ClearValues(KonHashTable* self)
{
    KonHashTableValEntry* iter = self->ValListHead;
    while (iter != KON_HASH_TABLE_NIL) {
        KonHashTableValEntry* next = iter->Next;
        free(iter);
        iter = next;
    }
}

int KonHashTable_Clear(KonHashTable* self)
{
    // free keys, and set value's key to NIL
    KonHashTable_ClearKeys(self);
    // KonHashTable_ClearValues(self);
    self->ValListHead = KON_HASH_TABLE_NIL;
    self->ValListTail = KON_HASH_TABLE_NIL;
    self->ItemNum = 0;
    return 1;
}

// number of elements
uint32_t KonHashTable_Length(KonHashTable* self)
{
    return self->ItemNum;
}

uint32_t KonHashTable_KeyHashCode(char* key, uint32_t keyLen)
{
    uint32_t hashCode = MurmurHash32(key, keyLen, KON_HASH_INDEX_SEED);
    return hashCode;
}

uint32_t KonHashTable_KeyHashIndex(KonHashTable* self, uint32_t hashCode)
{
    uint32_t hashIndex = self->HashMask & hashCode;
    return hashIndex;
}

uint32_t KonHashTable_KeyVerifyCode(char* key, int keyLen, uint32_t hashCode)
{
    // key + keylen(0x) + hashcode(0x)
    int verifyStrLen = keyLen +  8 + 8  + 1;
    char* tmp = (char*)calloc(verifyStrLen, sizeof(char));
    tmp[verifyStrLen] = '\0';
    snprintf(tmp, verifyStrLen, "%s%x%x", key, keyLen, hashCode);
    uint32_t verifyCode = MurmurHash32(tmp, verifyStrLen - 1, KON_HASH_VERIFY_SEED);

    free(tmp);
    return verifyCode;
}

bool KonHashTable_HasKey(KonHashTable* self, char* key)
{
    if (key == NULL) {
        return false;
    }
    uint32_t keyLen = strlen(key);
    uint32_t hashCode = KonHashTable_KeyHashCode(key, keyLen);
    uint32_t hashIndex = KonHashTable_KeyHashIndex(self, hashCode);
    uint32_t verifyCode = KonHashTable_KeyVerifyCode(key, keyLen, hashCode);

    KonHashTableKeyEntry* bucket = self->Buckets[hashIndex];

    if (bucket == KON_HASH_TABLE_NIL) {
        return false;
    }
    else {
        KonHashTableKeyEntry* iter = bucket;
        while (iter != KON_HASH_TABLE_NIL) {
            KonHashTableKeyEntry* next = iter->Next;
            if (iter->HashCode == hashCode
                && iter->KeyLen == keyLen
                && iter->VerifyCode == verifyCode
            ) {
                break;
            }
            iter = next;
        }
        if (iter == KON_HASH_TABLE_NIL) {
            return false;
        }
        else {
            return true;
        }
    }
}

KonHashTableValEntry* KonHashTable_ValEntryAtKey(KonHashTable* self, char* key)
{
    if (key == NULL) {
        return NULL;
    }
    
    uint32_t keyLen = strlen(key);
    uint32_t hashCode = KonHashTable_KeyHashCode(key, keyLen);
    uint32_t hashIndex = KonHashTable_KeyHashIndex(self, hashCode);
    
    uint32_t verifyCode = KonHashTable_KeyVerifyCode(key, keyLen, hashCode);

    KonHashTableKeyEntry* bucket = self->Buckets[hashIndex];
    if (bucket == KON_HASH_TABLE_NIL) {
        return NULL;
    }
    else {
        KonHashTableKeyEntry* iter = bucket;
        while (iter != KON_HASH_TABLE_NIL) {
            KonHashTableKeyEntry* next = iter->Next;
            if (iter->HashCode == hashCode
                && iter->KeyLen == keyLen
                && iter->VerifyCode == verifyCode
            ) {
                break;
            }
            iter = next;
        }

        if (iter == KON_HASH_TABLE_NIL) {
            return NULL;
        }
        else {
            return iter->ValEntry;
        }
    }
}
// get by key string
KN KonHashTable_AtKey(KonHashTable* self, char* key)
{
    if (key == NULL) {
        return KON_HASH_TABLE_NULL;
    }

    KonHashTableValEntry* valEntry = KonHashTable_ValEntryAtKey(self, key);
    
    // return KON_HASH_TABLE_NULL;
    if (valEntry == NULL) {
        return KON_HASH_TABLE_NULL;
    }

    return valEntry->Val;
}

KonHashTableValEntry* KonHashTable_ValEntryAtIndex(KonHashTable* self, int index)
{
    if (index >= self->ItemNum
        || index < 0
    ) {
        // out of range
        return NULL;
    }

    bool iterReverse = index >= (int)(self->ItemNum / 2) ? true : false;
    if (iterReverse) {
        uint32_t curIndex = self->ItemNum - 1;
        KonHashTableValEntry* iter = self->ValListTail;
        while (iter != KON_HASH_TABLE_NIL) {
            KonHashTableKeyEntry* next = iter->Prev;
            if (curIndex == index) {
                return iter;
            }
            iter = next;
            curIndex--;
        }
    }
    else {
        uint32_t curIndex = 0;
        KonHashTableValEntry* iter = self->ValListHead;
        while (iter != KON_HASH_TABLE_NIL) {
            KonHashTableKeyEntry* next = iter->Next;
            if (curIndex == index) {
                return iter;
            }
            iter = next;
            curIndex++;
        }
    }

    // TODO should not go here, throw
    return NULL;
}

// get by index number
KN KonHashTable_ValAtIndex(KonHashTable* self, int index)
{
    KonHashTableValEntry* valEntry = KonHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return KON_HASH_TABLE_NULL;
    }
    return valEntry->Val;    
}

const char* KonHashTable_KeyAtIndex(KonHashTable* self, int index)
{
    KonHashTableValEntry* valEntry = KonHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return NULL;
    }
    if (valEntry->KeyEntry == (void*)KON_HASH_TABLE_NIL) {
        return NULL;
    }
    return valEntry->KeyEntry->Key;    
}

KN KonHashTable_FirstVal(KonHashTable* self)
{
    return self->ValListHead->Val;
}

KN KonHashTable_LastVal(KonHashTable* self)
{
    return self->ValListTail->Val;
}

// init a key entry
KonHashTableKeyEntry* KonHashTable_CreateKeyEntry(KonHashTable* self, char* key)
{
    if (key == NULL) {
        return NULL;
    }
    KonHashTableKeyEntry* entry = (KonHashTableKeyEntry*)calloc(1, sizeof(KonHashTableKeyEntry));
    if (entry == NULL) {
        return NULL;
    }
    entry->KeyLen = strlen(key);
    char* copiedKey = (char*)calloc((entry->KeyLen + 1), sizeof(char));
    if (copiedKey == NULL) {
        return NULL;
    }
    strncpy(copiedKey, key, entry->KeyLen);
    copiedKey[entry->KeyLen] = '\0';
    entry->Key = copiedKey;

    uint32_t keyLen = strlen(key);
    uint32_t hashCode = KonHashTable_KeyHashCode(key, keyLen);
    uint32_t hashIndex = KonHashTable_KeyHashIndex(self, hashCode);
    uint32_t verifyCode = KonHashTable_KeyVerifyCode(key, keyLen, hashCode);

    entry->KeyLen = keyLen;
    entry->HashCode = hashCode;
    entry->VerifyCode = verifyCode;
    entry->ValEntry = KON_HASH_TABLE_NIL;
    entry->Prev = KON_HASH_TABLE_NIL;
    entry->Next = KON_HASH_TABLE_NIL;
    return entry;
}

KonHashTableValEntry* KonHashTable_CreateValEntry(KonHashTable* self, KN value)
{
    KonHashTableValEntry* entry = (KonHashTableValEntry*)calloc(1, sizeof(KonHashTableValEntry));
    if (entry == NULL) {
        return NULL;
    }
    entry->Val = value;
    entry->Prev = KON_HASH_TABLE_NIL;
    entry->Next = KON_HASH_TABLE_NIL;
    entry->KeyEntry = KON_HASH_TABLE_NIL;
    return entry;
}

// insert new value entry to tail
// and update ValListTail and ValListHead
void KonHashTable_PushValEntry(KonHashTable* self, KonHashTableValEntry* valEntry)
{
    KonHashTableValEntry* oldTail = self->ValListTail;
    if (oldTail != KON_HASH_TABLE_NIL) {
        oldTail->Next = valEntry;
    }

    // if is the first val
    if (self->ValListHead == KON_HASH_TABLE_NIL) {
        self->ValListHead = valEntry;
    }
    
    valEntry->Prev = oldTail;
    self->ValListTail = valEntry;
    self->ItemNum += 1;
}

// insert new value entry to head
// and update ValListTail and ValListHead
void KonHashTable_UnshiftValEntry(KonHashTable* self, KonHashTableValEntry* valEntry)
{
    KonHashTableValEntry* oldHead = self->ValListHead;
    if (oldHead != KON_HASH_TABLE_NIL) {
        oldHead->Prev = valEntry;
    }

    // if is the first val
    if (self->ValListTail == KON_HASH_TABLE_NIL) {
        self->ValListTail = valEntry;
    }

    valEntry->Next = oldHead;
    self->ValListHead = valEntry;
    self->ItemNum += 1;
}

// if no key, add
// if has key, unset origin val key ref, and set key val ref to the new one
// return the key entry
KonHashTableKeyEntry* KonHashTable_AddOrUpdateKeyEntry(KonHashTable* self, char* key, KonHashTableValEntry* valEntry)
{
    uint32_t keyLen = strlen(key);
    uint32_t hashCode = KonHashTable_KeyHashCode(key, keyLen);
    uint32_t hashIndex = KonHashTable_KeyHashIndex(self, hashCode);
    uint32_t verifyCode = KonHashTable_KeyVerifyCode(key, keyLen, hashCode);
    KonHashTableKeyEntry* bucket = self->Buckets[hashIndex];
    
    // find or create key entry
    KonHashTableKeyEntry* keyEntry = NULL;
    if (bucket != KON_HASH_TABLE_NIL) {
        // find key entry
        KonHashTableKeyEntry* iter = bucket;
        while (iter != KON_HASH_TABLE_NIL) {
            KonHashTableKeyEntry* next = iter->Next;

            if (iter->HashCode == hashCode
                && iter->KeyLen == keyLen
                && iter->VerifyCode == verifyCode
            ) {
                break;
            }

            iter = next;
        }

        if (iter != KON_HASH_TABLE_NIL) {
            // clear old value's key ref
            iter->ValEntry->KeyEntry = KON_HASH_TABLE_NIL;
            keyEntry = iter;

            keyEntry->ValEntry = valEntry;
            valEntry->KeyEntry = keyEntry;
            return keyEntry;
        }
        else {
            keyEntry = KonHashTable_CreateKeyEntry(self, key);

        }
    }
    else {
        keyEntry = KonHashTable_CreateKeyEntry(self, key);
    }

    if (keyEntry == NULL) {
        return NULL;
    }

    // insert new key entry as bucket first
    if (self->Buckets[hashIndex] != KON_HASH_TABLE_NIL) {
        self->Buckets[hashIndex]->Prev = keyEntry;
        keyEntry->Next = self->Buckets[hashIndex]->Prev;
    }
    else {
        self->BucketUsed += 1;
    }
    self->Buckets[hashIndex] = keyEntry;

    keyEntry->ValEntry = valEntry;
    valEntry->KeyEntry = keyEntry;

    KonHashTable_CheckRehash(self);
    return keyEntry;
}

int KonHashTable_PushVal(KonHashTable* self, KN value)
{
    KonHashTableValEntry* valEntry = KonHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    KonHashTable_PushValEntry(self, valEntry);
    return 1;
}

// push new val to tail
int KonHashTable_PushKv(KonHashTable* self, char* key, KN value)
{
    if (key == NULL) {
        return -1;
    }

    KonHashTableValEntry* valEntry = KonHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    KonHashTableKeyEntry* keyEntry = KonHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }
    // printf("KonHashTable_PushKv valEntry addr %x keyEntry addr %x, valEntry->KeyEntry %x, keyEntry->ValEntry %x , key %s\n",
    //     valEntry, keyEntry, valEntry->KeyEntry, keyEntry->ValEntry, keyEntry->Key
    // );
    KonHashTable_PushValEntry(self, valEntry);
    return 1;
}

// add value to head
int KonHashTable_UnshiftVal(KonHashTable* self, KN value)
{
    KonHashTableValEntry* valEntry = KonHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    KonHashTable_UnshiftValEntry(self, valEntry);
    return 1;
}
// add key value to head
int KonHashTable_UnshiftKv(KonHashTable* self, char* key, KN value)
{
    if (key == NULL) {
        return -1;
    }

    KonHashTableValEntry* valEntry = KonHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    KonHashTableKeyEntry* keyEntry = KonHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }
    KonHashTable_UnshiftValEntry(self, valEntry);
    return 1;
}

int KonHashTable_PutKv(KonHashTable* self, char* key, KN value)
{
    if (key == NULL) {
        return -1;
    }

    uint32_t keyLen = strlen(key);
    uint32_t hashCode = KonHashTable_KeyHashCode(key, keyLen);
    uint32_t hashIndex = KonHashTable_KeyHashIndex(self, hashCode);
    uint32_t verifyCode = KonHashTable_KeyVerifyCode(key, keyLen, hashCode);
    
    KonHashTableKeyEntry* bucket = self->Buckets[hashIndex];
    
    if (bucket != KON_HASH_TABLE_NIL) {
        // find key entry
        KonHashTableKeyEntry* iter = bucket;
        while (iter != KON_HASH_TABLE_NIL) {
            KonHashTableKeyEntry* next = iter->Next;

            if (iter->HashCode == hashCode
                && iter->KeyLen == keyLen
                && iter->VerifyCode == verifyCode
            ) {
                break;
            }

            iter = next;
        }

        if (iter != KON_HASH_TABLE_NIL) {
            // update val
            iter->ValEntry->Val = value;
            return 2;
        }
    }


    
    // add new k v
    KonHashTableKeyEntry* keyEntry = KonHashTable_CreateKeyEntry(self, key);
    if (keyEntry == NULL) {
        return -1;
    }

    KonHashTableValEntry* valEntry = KonHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    keyEntry->ValEntry = valEntry;
    valEntry->KeyEntry = keyEntry;

    // insert new key entry as bucket first
    if (self->Buckets[hashIndex] != KON_HASH_TABLE_NIL) {
        self->Buckets[hashIndex]->Prev = keyEntry;
        keyEntry->Next = self->Buckets[hashIndex]->Prev;
    }
    else {
        self->Buckets[hashIndex] += 1;
    }
    self->Buckets[hashIndex] = keyEntry;
    KonHashTable_PushValEntry(self, valEntry);


    return 1;
    
}

// set or update index item's key, if out of range, do nothing
// result 0: invalid or fail; 1 insert; 2 update
int KonHashTable_SetKeyAtIndex(KonHashTable* self, int index, char* key)
{
    KonHashTableValEntry* valEntry = KonHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        printf("KonHashTable_SetKeyAtIndex valEntry not found\n");
        return -1;
    }

    // remove old key
    KonHashTable_DelKeyEntry(self, valEntry->KeyEntry);
    valEntry->KeyEntry = KON_HASH_TABLE_NIL;

    KonHashTableKeyEntry* keyEntry = KonHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }
    valEntry->KeyEntry = keyEntry;
}

// set value by index, if out of range, do nothing
int KonHashTable_SetValAtIndex(KonHashTable* self, int index, KN value)
{
    KonHashTableValEntry* valEntry = KonHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return -1;
    }

    valEntry->Val = value;
    return 1;
}

// set index item value, if out of range, do nothing
// and update the key's value to this index
int KonHashTable_SetKvAtIndex(KonHashTable* self, int index, char* key, KN value)
{
    KonHashTableValEntry* valEntry = KonHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return -1;
    }

    valEntry->Val = value;

    // remove old key
    KonHashTable_DelKeyEntry(self, valEntry->KeyEntry);
    valEntry->KeyEntry = KON_HASH_TABLE_NIL;

    KonHashTableKeyEntry* keyEntry = KonHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }
    valEntry->KeyEntry = keyEntry;
    return 1;
}

void KonHashTable_DelValEntry(KonHashTable* self, KonHashTableValEntry* valEntry)
{
    // remove old key
    KonHashTable_DelKeyEntry(self, valEntry->KeyEntry);

    if (valEntry->Prev != KON_HASH_TABLE_NIL) {
        valEntry->Prev->Next = valEntry->Next;
    }

    if (valEntry->Next != KON_HASH_TABLE_NIL) {
        valEntry->Next->Prev = valEntry->Prev;
    }
    self->ItemNum -= 1;

    free(valEntry);
}

// del by key
int KonHashTable_DelByKey(KonHashTable* self, char* key)
{
    if (key == NULL) {
        return KON_HASH_TABLE_NULL;
    }

    KonHashTableValEntry* valEntry = KonHashTable_ValEntryAtKey(self, key);
 
    if (valEntry == NULL) {
        return KON_HASH_TABLE_NULL;
    }

    KonHashTable_DelValEntry(self, valEntry);
}

// del by index, if out of range, do nothing
int KonHashTable_DelByIndex(KonHashTable* self, int index)
{
    KonHashTableValEntry* valEntry = KonHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return -1;
    }

    KonHashTable_DelValEntry(self, valEntry);
    
}


////
// iterator func

KonHashTableIter KonHashTable_IterHead(KonHashTable* self)
{
    return (KonHashTableIter)self->ValListHead;
}

KonHashTableIter KonHashTable_IterTail(KonHashTable* self)
{
    return (KonHashTableIter)self->ValListTail;
}

bool KonHashTable_IterHasNext(KonHashTable* self, KonHashTableIter iter)
{
    KonHashTableValEntry* valEntry = (KonHashTableValEntry*)iter;
    return (iter->Next == KON_HASH_TABLE_NIL) ? false : true;
}

KonHashTableIter KonHashTable_IterNext(KonHashTable* self, KonHashTableIter iter)
{
    if (iter == KON_HASH_TABLE_NIL) {
        return (KonHashTableIter)KON_HASH_TABLE_NIL;
    }
    KonHashTableValEntry* valEntry = (KonHashTableValEntry*)iter;

    return (KonHashTableIter)(valEntry->Next);
}

const char* KonHashTable_IterGetKey(KonHashTable* self, KonHashTableIter iter)
{
    if (iter == KON_HASH_TABLE_NIL) {
        return NULL;
    }
    KonHashTableValEntry* valEntry = (KonHashTableValEntry*)iter;
    if (valEntry->KeyEntry == KON_HASH_TABLE_NIL) {
        return NULL;
    }
    return valEntry->KeyEntry->Key;
}

KN KonHashTable_IterGetVal(KonHashTable* self, KonHashTableIter iter)
{
    if (iter == KON_HASH_TABLE_NIL) {
        return KON_HASH_TABLE_NULL;
    }
    KonHashTableValEntry* valEntry = (KonHashTableValEntry*)iter;
    return valEntry->Val;
}

// set iter item key
// result 0: invalid or fail; 1 insert; 2 update
int KonHashTable_IterSetKey(KonHashTable* self, KonHashTableIter iter, char* key)
{
    KonHashTableValEntry* valEntry = (KonHashTableValEntry*)iter;
    if (valEntry == NULL || iter == KON_HASH_TABLE_NIL) {
        return -1;
    }

    // remove old key
    KonHashTable_DelKeyEntry(self, valEntry->KeyEntry);
    valEntry->KeyEntry = KON_HASH_TABLE_NIL;

    KonHashTableKeyEntry* keyEntry = KonHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }
    valEntry->KeyEntry = keyEntry;
    return 1;
}
// set iter item value
int KonHashTable_IterSetVal(KonHashTable* self, KonHashTableIter iter, KN value)
{
    KonHashTableValEntry* valEntry = (KonHashTableValEntry*)iter;
    if (valEntry == NULL || iter == KON_HASH_TABLE_NIL) {
        return -1;
    }
    valEntry->Val = value;
}


// del key and value
int KonHashTable_DelByIter(KonHashTable* self, KonHashTableIter iter, char* key)
{
    KonHashTableValEntry* valEntry = (KonHashTableValEntry*)iter;
    if (valEntry == NULL || iter == KON_HASH_TABLE_NIL) {
        return -1;
    }
    KonHashTable_DelValEntry(self, valEntry);
    return 1;
}

// insert value before iter
KonHashTableValEntry* KonHashTable_CreateValEntryBeforeIter(KonHashTable* self, KonHashTableIter iter, KN value)
{
    KonHashTableValEntry* valEntry = (KonHashTableValEntry*)iter;
    if (valEntry == NULL || iter == KON_HASH_TABLE_NIL) {
        return NULL;
    }

    KonHashTableValEntry* newValEntry = KonHashTable_CreateValEntry(self, value);
    if (newValEntry == NULL) {
        return NULL;
    }
    newValEntry->Next = valEntry;
    valEntry->Prev = newValEntry;

    KonHashTableValEntry* oldHead = valEntry->Prev;
    if (oldHead != KON_HASH_TABLE_NIL) {
        oldHead->Next = newValEntry;
    }
    newValEntry->Prev = oldHead;

    // update ValListHead
    if (self->ValListHead == valEntry) {
        self->ValListHead = newValEntry;
    }
    
    self->ItemNum += 1;
    return newValEntry;
}

int KonHashTable_InsertValBeforeIter(KonHashTable* self, KonHashTableIter iter, KN value)
{
    KonHashTableValEntry* newValEntry = KonHashTable_CreateValEntryBeforeIter(self, iter, value);
    if (newValEntry == NULL) {
        return -1;
    }
    else {
        return 1;
    }
}

// insert k v before iter
int KonHashTable_InsertKvBeforIter(KonHashTable* self, KonHashTableIter iter, char* key, KN value)
{
    KonHashTableValEntry* newValEntry = KonHashTable_CreateValEntryBeforeIter(self, iter, value);
    if (newValEntry == NULL) {
        return -1;
    }

    KonHashTable_AddOrUpdateKeyEntry(self, key, newValEntry);
    return 1;
}



KonHashTableValEntry* KonHashTable_CreateValEntryAfterIter(KonHashTable* self, KonHashTableIter iter, KN value)
{
    KonHashTableValEntry* valEntry = (KonHashTableValEntry*)iter;
    if (valEntry == NULL || iter == KON_HASH_TABLE_NIL) {
        return NULL;
    }

    KonHashTableValEntry* newValEntry = KonHashTable_CreateValEntry(self, value);
    if (newValEntry == NULL) {
        return NULL;
    }
    newValEntry->Prev = valEntry;
    valEntry->Next = newValEntry;

    KonHashTableValEntry* oldTail = valEntry->Next;
    if (oldTail != KON_HASH_TABLE_NIL) {
        oldTail->Prev = newValEntry;
    }
    newValEntry->Next = oldTail;

    // update ValListTail
    if (self->ValListTail == valEntry) {
        self->ValListTail = newValEntry;
    }
    
    self->ItemNum += 1;
    return newValEntry;
}

// insert value after iter
int KonHashTable_InsertValAfterIter(KonHashTable* self, KonHashTableIter iter, KN value)
{
    KonHashTableValEntry* newValEntry = KonHashTable_CreateValEntryAfterIter(self, iter, value);
    if (newValEntry == NULL) {
        return -1;
    }
    else {
        return 1;
    }
}
// insert k v after iter
int KonHashTable_InsertKvAfterIter(KonHashTable* self, KonHashTableIter iter, char* key, KN value)
{
    KonHashTableValEntry* newValEntry = KonHashTable_CreateValEntryAfterIter(self, iter, value);
    if (newValEntry == NULL) {
        return -1;
    }

    KonHashTable_AddOrUpdateKeyEntry(self, key, newValEntry);
    return 1;
}

int KonHashTable_FindRehashPowerOfTwo(KonHashTable* self, int previousN, bool isGrow)
{
    if (isGrow) {
        int nextSize = 2 << (previousN + 1);
        if (nextSize * KON_HASH_TABLE_REHASH_THRESHOLD > self->ItemNum) {
            // ok, return
            return previousN + 1;
        }
        else {
            return KonHashTable_FindRehashPowerOfTwo(self, previousN + 1, isGrow);
        }
    }
    else {
        // shink
        int nextSize = 2 << (previousN - 1);
        // min size 8
        if (nextSize == 4
            || (nextSize > self->ItemNum
                && (nextSize / 2) < self->ItemNum
               )
        ) {
            // ok
            return previousN - 1;
        }
        else {
            return KonHashTable_FindRehashPowerOfTwo(self, previousN - 1, isGrow);
        }
    }
}


void KonHashTable_Rehash(KonHashTable* self, int newPowerOfTwo)
{
    uint32_t newHashSize = 2 << newPowerOfTwo;
    uint32_t newHashMask = newHashSize - 1;
    
    KonHashTableKeyEntry** buckets = (KonHashTableKeyEntry**)calloc(newHashSize, sizeof(KonHashTableKeyEntry));
    if (buckets == NULL) {
        return;
    }

    for (uint32_t i = 0; i < newHashSize; i++) {
        *(buckets + i) = KON_HASH_TABLE_NIL;
    }

    // copy two new buckets
    for (uint32_t i = 0; i < self->HashSize; i++) {
        KonHashTableKeyEntry* iter = *(self->Buckets + i);
        while (iter != KON_HASH_TABLE_NIL) {
            KonHashTableKeyEntry* next = iter->Next;

            uint32_t hashCode = iter->HashCode;
            uint32_t newHashIndex = newHashMask | hashCode;

            if (*(buckets + newHashIndex) != KON_HASH_TABLE_NIL) {
                // bucket first entry
                iter->Prev = KON_HASH_TABLE_NIL;
                iter->Next = KON_HASH_TABLE_NIL;
            }
            else {
                // insert to list head
                KonHashTableKeyEntry* oldBucketFirst = *(buckets + newHashIndex);
                oldBucketFirst->Prev = iter;
                iter->Next = oldBucketFirst;
                iter->Prev = KON_HASH_TABLE_NIL;
            }
            *(buckets + newHashIndex) = iter;

            iter = next;
        }
    }

    uint32_t bucketUsed = 0;
    for (uint32_t i = 0; i < newHashSize; i++) {
        if (*(buckets + i) != KON_HASH_TABLE_NIL) {
            bucketUsed += 1;
        }
    }

    self->HashSize = newHashSize;
    self->HashMask = newHashMask;
    self->PowerOfTwo = newPowerOfTwo;
    self->BucketUsed = bucketUsed;
}

void KonHashTable_CheckRehash(KonHashTable* self) {

    uint32_t currSize = self->ItemNum;
    if (currSize * (1.0 - KON_HASH_TABLE_REHASH_THRESHOLD) > currSize) {
        // shrink
        int n = KonHashTable_FindRehashPowerOfTwo(self, self->PowerOfTwo, false);
        KonHashTable_Rehash(self, n);
    }
    else if (currSize * (1.0 + KON_HASH_TABLE_REHASH_THRESHOLD) < currSize) {
        // grow
        int n = KonHashTable_FindRehashPowerOfTwo(self, self->PowerOfTwo, true);
        KonHashTable_Rehash(self, n);
    }

}