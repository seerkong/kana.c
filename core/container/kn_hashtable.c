/**
 * KnHashTable
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

#include "murmurhash.h"
#include "kn_hashtable.h"
#include "../../lib/tbox/tbox.h"

#define KX_HASH_INDEX_SEED 31
#define KX_HASH_VERIFY_SEED 25

KnHashTable* KnHashTable_Init(uint32_t powerOfTwo)
{
    KnHashTable* self = (KnHashTable*)tb_nalloc0(1, sizeof(KnHashTable));
    if (self == NULL) {
        return NULL;
    }
    
    uint32_t hashSize = 2 << powerOfTwo;
    self->hashSize = hashSize;
    self->hashMask = hashSize - 1;
    self->powerOfTwo = powerOfTwo;
    self->bucketUsed = 0;
    self->itemNum = 0;
    self->valListHead = KN_HASH_TABLE_NIL;
    self->valListTail = KN_HASH_TABLE_NIL;

    KnHashTableKeyEntry** buckets = (KnHashTableKeyEntry**)tb_nalloc0(hashSize, sizeof(void*));
    if (buckets == NULL) {
        tb_free(self);
        return NULL;
    }

    for (uint32_t i = 0; i < hashSize; i++) {
        *(buckets + i) = KN_HASH_TABLE_NIL;
    }
    self->buckets = buckets;

    return self;
}

int KnHashTable_Destroy(KnHashTable* self)
{
    if (self == NULL) {
        return -1;
    }
    KnHashTable_Clear(self);
    tb_free(self);
    return 1;
}

// clear keys in this bucket and set value's key to NIL
void KnHashTable_ClearBucketKeys(KnHashTable* self, KnHashTableKeyEntry* bucket)
{
    if ((ktable_val_t)bucket == KN_HASH_TABLE_NIL) {
        return;
    }
    KnHashTableKeyEntry* iter = bucket;
    while ((ktable_val_t)iter != KN_HASH_TABLE_NIL) {
        KnHashTableKeyEntry* next = iter->next;
        if ((ktable_val_t)iter->valEntry != KN_HASH_TABLE_NIL) {
            tb_free(iter->valEntry->key);
            iter->valEntry->key = NULL;
        }
        tb_free(iter);
        iter = next;
    }
}

void KnHashTable_ClearKeys(KnHashTable* self)
{
    KnHashTableKeyEntry** buckets = self->buckets;

    self->bucketUsed = 0;
    for (uint32_t i = 0; i < self->hashSize; i++) {
        KnHashTableKeyEntry* bucket = (KnHashTableKeyEntry*)(*(buckets + i));
        KnHashTable_ClearBucketKeys(self, bucket);
        *(buckets + i) = KN_HASH_TABLE_NIL;
    }
}

// only clear one key entry, and update prev, next ref
void KnHashTable_DelKeyEntry(KnHashTable* self, KnHashTableKeyEntry* keyEntry) {
    if ((ktable_val_t)keyEntry == KN_HASH_TABLE_NIL) {
        return;
    }

    // if the entry is the bucket first, update bucket
    uint32_t hashIndex = KnHashTable_KeyHashIndex(self, keyEntry->hashCode);
    if (self->buckets[hashIndex] == keyEntry) {
        self->buckets[hashIndex] = keyEntry->next;
        if ((ktable_val_t)keyEntry->next == KN_HASH_TABLE_NIL) {
            self->bucketUsed -= 1;
        }
    }
    
    if ((ktable_val_t)keyEntry->next != KN_HASH_TABLE_NIL) {
        keyEntry->next->prev = keyEntry->prev;
    }

    if ((ktable_val_t)keyEntry->prev != KN_HASH_TABLE_NIL) {
        keyEntry->prev->next = keyEntry->next;
    }

    tb_free(keyEntry);

    KnHashTable_CheckRehash(self);
}

// assume all keys are cleared
// !!! must be called after KnHashTable_ClearKeys
void KnHashTable_ClearValues(KnHashTable* self)
{
    KnHashTableValEntry* iter = self->valListHead;
    while ((ktable_val_t)iter != KN_HASH_TABLE_NIL) {
        KnHashTableValEntry* next = iter->next;
        if (iter->key != NULL) {
            tb_free(iter->key);
        }
        tb_free(iter);
        iter = next;
    }
}

int KnHashTable_Clear(KnHashTable* self)
{
    // free keys, and set value's key to NIL
    KnHashTable_ClearKeys(self);
    // KnHashTable_ClearValues(self);
    self->valListHead = KN_HASH_TABLE_NIL;
    self->valListTail = KN_HASH_TABLE_NIL;
    self->itemNum = 0;
    return 1;
}

// number of elements
uint32_t KnHashTable_Length(KnHashTable* self)
{
    return self->itemNum;
}

uint32_t KnHashTable_KeyHashCode(char* key, uint32_t keyLen)
{
    uint32_t hashCode = MurmurHash32(key, keyLen, KX_HASH_INDEX_SEED);
    return hashCode;
}

uint32_t KnHashTable_KeyHashIndex(KnHashTable* self, uint32_t hashCode)
{
    uint32_t hashIndex = self->hashMask & hashCode;
    return hashIndex;
}

uint32_t KnHashTable_KeyVerifyCode(char* key, int keyLen, uint32_t hashCode)
{
    // key + keylen(0x) + hashcode(0x)
    int verifyStrLen = keyLen +  8 + 8  + 1;
    char* tmp = (char*)tb_nalloc0(verifyStrLen, sizeof(char));
    tmp[verifyStrLen] = '\0';
    snprintf(tmp, verifyStrLen, "%s%x%x", key, keyLen, hashCode);
    uint32_t verifyCode = MurmurHash32(tmp, verifyStrLen - 1, KX_HASH_VERIFY_SEED);

    tb_free(tmp);
    return verifyCode;
}

// get KeyEntry by cstr key
KnHashTableKeyEntry* KnHashTable_GetKeyEntry(KnHashTable* self, char* key)
{
    if (key == NULL) {
        return KN_HASH_TABLE_UNDEF;
    }
    uint32_t keyLen = strlen(key);
    uint32_t hashCode = KnHashTable_KeyHashCode(key, keyLen);
    uint32_t hashIndex = KnHashTable_KeyHashIndex(self, hashCode);
    uint32_t verifyCode = KnHashTable_KeyVerifyCode(key, keyLen, hashCode);

    KnHashTableKeyEntry* bucket = self->buckets[hashIndex];

    if ((ktable_val_t)bucket == KN_HASH_TABLE_NIL) {
        return KN_HASH_TABLE_UNDEF;
    }
    else {
        KnHashTableKeyEntry* iter = bucket;
        while ((ktable_val_t)iter != KN_HASH_TABLE_NIL) {
            KnHashTableKeyEntry* next = iter->next;
            if (iter->hashCode == hashCode
                && iter->keyLen == keyLen
                && iter->verifyCode == verifyCode
            ) {
                break;
            }
            iter = next;
        }
        if ((ktable_val_t)iter == KN_HASH_TABLE_NIL) {
            return KN_HASH_TABLE_UNDEF;
        }
        else {
            return iter;
        }
    }
}

bool KnHashTable_HasKey(KnHashTable* self, const char* key)
{
    KnHashTableKeyEntry* keyEntry = KnHashTable_GetKeyEntry(self, key);
    if ((ktable_val_t)keyEntry == KN_HASH_TABLE_UNDEF) {
        return false;
    }
    else {
        return true;
    }
}

KnHashTableValEntry* KnHashTable_ValEntryAtKey(KnHashTable* self, char* key)
{
    KnHashTableKeyEntry* keyEntry = KnHashTable_GetKeyEntry(self, key);
    if ((ktable_val_t)keyEntry == KN_HASH_TABLE_UNDEF) {
        return NULL;
    }
    else {
        return keyEntry->valEntry;;
    }
}
// get by key string
ktable_val_t KnHashTable_AtKey(KnHashTable* self, const char* key)
{
    if (key == NULL) {
        return KN_HASH_TABLE_UNDEF;
    }

    KnHashTableValEntry* valEntry = KnHashTable_ValEntryAtKey(self, key);
    
    // return KN_HASH_TABLE_UNDEF;
    if (valEntry == NULL) {
        return KN_HASH_TABLE_UNDEF;
    }

    return valEntry->val;
}

KnHashTableValEntry* KnHashTable_ValEntryAtIndex(KnHashTable* self, int index)
{
    if (index >= self->itemNum
        || index < 0
    ) {
        // out of range
        return NULL;
    }

    bool iterReverse = index >= (int)(self->itemNum / 2) ? true : false;
    if (iterReverse) {
        uint32_t curIndex = self->itemNum - 1;
        KnHashTableValEntry* iter = self->valListTail;
        while ((ktable_val_t)iter != KN_HASH_TABLE_NIL) {
            KnHashTableValEntry* next = iter->prev;
            if (curIndex == index) {
                return iter;
            }
            iter = next;
            curIndex--;
        }
    }
    else {
        uint32_t curIndex = 0;
        KnHashTableValEntry* iter = self->valListHead;
        while ((ktable_val_t)iter != KN_HASH_TABLE_NIL) {
            KnHashTableValEntry* next = iter->next;
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
ktable_val_t KnHashTable_ValAtIndex(KnHashTable* self, int index)
{
    KnHashTableValEntry* valEntry = KnHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return KN_HASH_TABLE_UNDEF;
    }
    return valEntry->val;    
}

const char* KnHashTable_KeyAtIndex(KnHashTable* self, int index)
{
    KnHashTableValEntry* valEntry = KnHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return NULL;
    }
    return valEntry->key;    
}

ktable_val_t KnHashTable_FirstVal(KnHashTable* self)
{
    return self->valListHead->val;
}

ktable_val_t KnHashTable_LastVal(KnHashTable* self)
{
    return self->valListTail->val;
}

// init a key entry
KnHashTableKeyEntry* KnHashTable_CreateKeyEntry(KnHashTable* self, char* key)
{
    if (key == NULL) {
        return NULL;
    }
    KnHashTableKeyEntry* entry = (KnHashTableKeyEntry*)tb_nalloc0(1, sizeof(KnHashTableKeyEntry));
    if (entry == NULL) {
        return NULL;
    }
    entry->keyLen = strlen(key);

    uint32_t keyLen = strlen(key);
    uint32_t hashCode = KnHashTable_KeyHashCode(key, keyLen);
    uint32_t hashIndex = KnHashTable_KeyHashIndex(self, hashCode);
    uint32_t verifyCode = KnHashTable_KeyVerifyCode(key, keyLen, hashCode);

    entry->keyLen = keyLen;
    entry->hashCode = hashCode;
    entry->verifyCode = verifyCode;
    entry->valEntry = KN_HASH_TABLE_NIL;
    entry->prev = KN_HASH_TABLE_NIL;
    entry->next = KN_HASH_TABLE_NIL;
    return entry;
}

KnHashTableValEntry* KnHashTable_CreateValEntry(KnHashTable* self, ktable_val_t value)
{
    KnHashTableValEntry* entry = (KnHashTableValEntry*)tb_nalloc0(1, sizeof(KnHashTableValEntry));
    if (entry == NULL) {
        return NULL;
    }
    entry->key = NULL;
    entry->val = value;
    entry->prev = KN_HASH_TABLE_NIL;
    entry->next = KN_HASH_TABLE_NIL;
    return entry;
}

// insert new value entry to tail
// and update ValListTail and ValListHead
void KnHashTable_PushValEntry(KnHashTable* self, KnHashTableValEntry* valEntry)
{
    KnHashTableValEntry* oldTail = self->valListTail;
    if ((ktable_val_t)oldTail != KN_HASH_TABLE_NIL) {
        oldTail->next = valEntry;
    }

    // if is the first val
    if ((ktable_val_t)self->valListHead == KN_HASH_TABLE_NIL) {
        self->valListHead = valEntry;
    }
    
    valEntry->prev = oldTail;
    self->valListTail = valEntry;
    self->itemNum += 1;
}

// insert new value entry to head
// and update ValListTail and ValListHead
void KnHashTable_UnshiftValEntry(KnHashTable* self, KnHashTableValEntry* valEntry)
{
    KnHashTableValEntry* oldHead = self->valListHead;
    if ((ktable_val_t)oldHead != KN_HASH_TABLE_NIL) {
        oldHead->prev = valEntry;
    }

    // if is the first val
    if ((ktable_val_t)self->valListTail == KN_HASH_TABLE_NIL) {
        self->valListTail = valEntry;
    }

    valEntry->next = oldHead;
    self->valListHead = valEntry;
    self->itemNum += 1;
}

// update the cstr key of a k v pair.
// free the old key
void KnHashTable_UpdateEntryKeyCstr(KnHashTableValEntry* valEntry, char* key)
{
    // set entry key
    if (valEntry->key != NULL) {
        tb_free(valEntry->key);
    }
    int strLen = strlen(key);
    char* copiedKey = (char*)tb_nalloc0((strLen + 1), sizeof(char));
    if (copiedKey == NULL) {
        return;
    }

    strncpy(copiedKey, key, strLen);
    copiedKey[strLen] = '\0';
    valEntry->key = copiedKey;
}

// if no key, add
// if has key, unset origin val key ref, and set key val ref to the new one
// return the key entry
KnHashTableKeyEntry* KnHashTable_AddOrUpdateKeyEntry(KnHashTable* self, char* key, KnHashTableValEntry* valEntry)
{
    // set entry key
    KnHashTable_UpdateEntryKeyCstr(valEntry, key);

    uint32_t keyLen = strlen(key);
    uint32_t hashCode = KnHashTable_KeyHashCode(key, keyLen);
    uint32_t hashIndex = KnHashTable_KeyHashIndex(self, hashCode);
    KnHashTableKeyEntry* bucket = self->buckets[hashIndex];
    
    // find or create key entry
    KnHashTableKeyEntry* keyEntry = KnHashTable_GetKeyEntry(self, key);
    if ((ktable_val_t)keyEntry != KN_HASH_TABLE_UNDEF) {
        // free origin key
        if (keyEntry->valEntry->key != NULL) {
            tb_free(keyEntry->valEntry->key);
            keyEntry->valEntry->key = NULL;
        }
        keyEntry->valEntry = valEntry;
        return keyEntry;
    }
    keyEntry = KnHashTable_CreateKeyEntry(self, key);

    if (keyEntry == NULL) {
        return NULL;
    }

    // insert new key entry as bucket first
    if ((ktable_val_t)self->buckets[hashIndex] != KN_HASH_TABLE_NIL) {
        self->buckets[hashIndex]->prev = keyEntry;
        keyEntry->next = self->buckets[hashIndex]->prev;
    }
    else {
        self->bucketUsed += 1;
    }
    self->buckets[hashIndex] = keyEntry;

    keyEntry->valEntry = valEntry;

    KnHashTable_CheckRehash(self);
    return keyEntry;
}

int KnHashTable_PushVal(KnHashTable* self, ktable_val_t value)
{
    KnHashTableValEntry* valEntry = KnHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    KnHashTable_PushValEntry(self, valEntry);
    return 1;
}

// push new val to tail
int KnHashTable_PushKv(KnHashTable* self, char* key, ktable_val_t value)
{
    if (key == NULL) {
        return -1;
    }

    KnHashTableValEntry* valEntry = KnHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    KnHashTableKeyEntry* keyEntry = KnHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }

    KnHashTable_PushValEntry(self, valEntry);
    return 1;
}

// add value to head
int KnHashTable_UnshiftVal(KnHashTable* self, ktable_val_t value)
{
    KnHashTableValEntry* valEntry = KnHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    KnHashTable_UnshiftValEntry(self, valEntry);
    return 1;
}
// add key value to head
int KnHashTable_UnshiftKv(KnHashTable* self, const char* key, ktable_val_t value)
{
    if (key == NULL) {
        return -1;
    }

    KnHashTableValEntry* valEntry = KnHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    KnHashTableKeyEntry* keyEntry = KnHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }
    KnHashTable_UnshiftValEntry(self, valEntry);
    return 1;
}

int KnHashTable_PutKv(KnHashTable* self, const char* key, ktable_val_t value)
{
    if (key == NULL) {
        return -1;
    }

    uint32_t keyLen = strlen(key);
    uint32_t hashCode = KnHashTable_KeyHashCode(key, keyLen);
    uint32_t hashIndex = KnHashTable_KeyHashIndex(self, hashCode);

    KnHashTableKeyEntry* keyEntry = KnHashTable_GetKeyEntry(self, key);
    if ((ktable_val_t)keyEntry != KN_HASH_TABLE_UNDEF) {
        // update val
        keyEntry->valEntry->val = value;
        KnHashTable_UpdateEntryKeyCstr(keyEntry->valEntry, key);
        return 2;
    }

    // add new k v
    keyEntry = KnHashTable_CreateKeyEntry(self, key);
    if (keyEntry == NULL) {
        return -1;
    }

    KnHashTableValEntry* valEntry = KnHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    keyEntry->valEntry = valEntry;
    KnHashTable_UpdateEntryKeyCstr(valEntry, key);

    // insert new key entry as bucket first
    if ((ktable_val_t)self->buckets[hashIndex] != KN_HASH_TABLE_NIL) {
        self->buckets[hashIndex]->prev = keyEntry;
        keyEntry->next = self->buckets[hashIndex];
    }
    else {
        self->buckets[hashIndex] += 1;
    }
    self->buckets[hashIndex] = keyEntry;
    KnHashTable_PushValEntry(self, valEntry);


    return 1;
    
}

// set or update index item's key, if out of range, do nothing
// result 0: invalid or fail; 1 insert; 2 update
int KnHashTable_SetKeyAtIndex(KnHashTable* self, int index, const char* key)
{
    KnHashTableValEntry* valEntry = KnHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        printf("KnHashTable_SetKeyAtIndex valEntry not found\n");
        return -1;
    }

    KnHashTableKeyEntry* keyEntry = KnHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }
    KnHashTable_UpdateEntryKeyCstr(valEntry, key);
    return 1;
}

// set value by index, if out of range, do nothing
int KnHashTable_SetValAtIndex(KnHashTable* self, int index, ktable_val_t value)
{
    KnHashTableValEntry* valEntry = KnHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return -1;
    }

    valEntry->val = value;
    return 1;
}

// set index item value, if out of range, do nothing
// and update the key's value to this index
int KnHashTable_SetKvAtIndex(KnHashTable* self, int index, const char* key, ktable_val_t value)
{
    KnHashTableValEntry* valEntry = KnHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return -1;
    }

    valEntry->val = value;

    KnHashTableKeyEntry* keyEntry = KnHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }
    KnHashTable_UpdateEntryKeyCstr(valEntry, key);
    return 1;
}

void KnHashTable_DelValEntry(KnHashTable* self, KnHashTableValEntry* valEntry)
{
    if (valEntry->key != NULL) {
        KnHashTableKeyEntry* keyEntry = KnHashTable_GetKeyEntry(self, valEntry->key);
        if ((ktable_val_t)keyEntry != KN_HASH_TABLE_UNDEF) {
            // remove old key
            KnHashTable_DelKeyEntry(self, keyEntry);
        }
        tb_free(valEntry->key);
    }

    if ((ktable_val_t)valEntry->prev != KN_HASH_TABLE_NIL) {
        valEntry->prev->next = valEntry->next;
    }

    if ((ktable_val_t)valEntry->next != KN_HASH_TABLE_NIL) {
        valEntry->next->prev = valEntry->prev;
    }
    self->itemNum -= 1;

    tb_free(valEntry);
}

// del by key
int KnHashTable_DelByKey(KnHashTable* self, const char* key)
{
    if (key == NULL) {
        return KN_HASH_TABLE_UNDEF;
    }

    KnHashTableValEntry* valEntry = KnHashTable_ValEntryAtKey(self, key);
 
    if (valEntry == NULL) {
        return KN_HASH_TABLE_UNDEF;
    }

    KnHashTable_DelValEntry(self, valEntry);
    return 1;
}

// del by index, if out of range, do nothing
int KnHashTable_DelByIndex(KnHashTable* self, int index)
{
    KnHashTableValEntry* valEntry = KnHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return -1;
    }

    KnHashTable_DelValEntry(self, valEntry);
    return 1;
}


////
// iterator func

KnHashTableIter KnHashTable_IterHead(KnHashTable* self)
{
    return (KnHashTableIter)self->valListHead;
}

KnHashTableIter KnHashTable_IterTail(KnHashTable* self)
{
    return (KnHashTableIter)self->valListTail;
}

bool KnHashTable_IterHasNext(KnHashTable* self, KnHashTableIter iter)
{
    KnHashTableValEntry* valEntry = (KnHashTableValEntry*)iter;
    return ((ktable_val_t)iter->next == KN_HASH_TABLE_NIL) ? false : true;
}

KnHashTableIter KnHashTable_IterNext(KnHashTable* self, KnHashTableIter iter)
{
    if ((ktable_val_t)iter == KN_HASH_TABLE_NIL) {
        return (KnHashTableIter)KN_HASH_TABLE_NIL;
    }
    KnHashTableValEntry* valEntry = (KnHashTableValEntry*)iter;

    return (KnHashTableIter)(valEntry->next);
}

bool KnHashTable_IterHasPrev(KnHashTable* self, KnHashTableIter iter)
{
    KnHashTableValEntry* valEntry = (KnHashTableValEntry*)iter;
    return ((ktable_val_t)iter->prev == KN_HASH_TABLE_NIL) ? false : true;
}

KnHashTableIter KnHashTable_IterPrev(KnHashTable* self, KnHashTableIter iter)
{
    if ((ktable_val_t)iter == KN_HASH_TABLE_NIL) {
        return (KnHashTableIter)KN_HASH_TABLE_NIL;
    }
    KnHashTableValEntry* valEntry = (KnHashTableValEntry*)iter;

    return (KnHashTableIter)(valEntry->prev);
}

const char* KnHashTable_IterGetKey(KnHashTable* self, KnHashTableIter iter)
{
    if ((ktable_val_t)iter == KN_HASH_TABLE_NIL) {
        return NULL;
    }
    KnHashTableValEntry* valEntry = (KnHashTableValEntry*)iter;
    return valEntry->key;
}

ktable_val_t KnHashTable_IterGetVal(KnHashTable* self, KnHashTableIter iter)
{
    if ((ktable_val_t)iter == KN_HASH_TABLE_NIL) {
        return KN_HASH_TABLE_UNDEF;
    }
    KnHashTableValEntry* valEntry = (KnHashTableValEntry*)iter;
    return valEntry->val;
}

// set iter item key
// result 0: invalid or fail; 1 insert; 2 update
int KnHashTable_IterSetKey(KnHashTable* self, KnHashTableIter iter, char* key)
{
    KnHashTableValEntry* valEntry = (KnHashTableValEntry*)iter;
    if (valEntry == NULL || (ktable_val_t)iter == KN_HASH_TABLE_NIL) {
        return -1;
    }

    KnHashTableKeyEntry* keyEntry = KnHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }
    KnHashTable_UpdateEntryKeyCstr(valEntry, key);
    return 1;
}
// set iter item value
int KnHashTable_IterSetVal(KnHashTable* self, KnHashTableIter iter, ktable_val_t value)
{
    KnHashTableValEntry* valEntry = (KnHashTableValEntry*)iter;
    if (valEntry == NULL || (ktable_val_t)iter == KN_HASH_TABLE_NIL) {
        return -1;
    }
    valEntry->val = value;
    return 1;
}


// del key and value
int KnHashTable_DelByIter(KnHashTable* self, KnHashTableIter iter, char* key)
{
    KnHashTableValEntry* valEntry = (KnHashTableValEntry*)iter;
    if (valEntry == NULL || (ktable_val_t)iter == KN_HASH_TABLE_NIL) {
        return -1;
    }
    KnHashTable_DelValEntry(self, valEntry);
    return 1;
}

// insert value before iter
KnHashTableValEntry* KnHashTable_CreateValEntryBeforeIter(KnHashTable* self, KnHashTableIter iter, ktable_val_t value)
{
    KnHashTableValEntry* valEntry = (KnHashTableValEntry*)iter;
    if (valEntry == NULL || (ktable_val_t)iter == KN_HASH_TABLE_NIL) {
        return NULL;
    }

    KnHashTableValEntry* newValEntry = KnHashTable_CreateValEntry(self, value);
    if (newValEntry == NULL) {
        return NULL;
    }
    newValEntry->next = valEntry;
    valEntry->prev = newValEntry;

    KnHashTableValEntry* oldHead = valEntry->prev;
    if ((ktable_val_t)oldHead != KN_HASH_TABLE_NIL) {
        oldHead->next = newValEntry;
    }
    newValEntry->prev = oldHead;

    // update ValListHead
    if (self->valListHead == valEntry) {
        self->valListHead = newValEntry;
    }
    
    self->itemNum += 1;
    return newValEntry;
}

int KnHashTable_InsertValBeforeIter(KnHashTable* self, KnHashTableIter iter, ktable_val_t value)
{
    KnHashTableValEntry* newValEntry = KnHashTable_CreateValEntryBeforeIter(self, iter, value);
    if (newValEntry == NULL) {
        return -1;
    }
    else {
        return 1;
    }
}

// insert k v before iter
int KnHashTable_InsertKvBeforIter(KnHashTable* self, KnHashTableIter iter, char* key, ktable_val_t value)
{
    KnHashTableValEntry* newValEntry = KnHashTable_CreateValEntryBeforeIter(self, iter, value);
    if (newValEntry == NULL) {
        return -1;
    }

    KnHashTable_AddOrUpdateKeyEntry(self, key, newValEntry);
    return 1;
}



KnHashTableValEntry* KnHashTable_CreateValEntryAfterIter(KnHashTable* self, KnHashTableIter iter, ktable_val_t value)
{
    KnHashTableValEntry* valEntry = (KnHashTableValEntry*)iter;
    if (valEntry == NULL || (ktable_val_t)iter == KN_HASH_TABLE_NIL) {
        return NULL;
    }

    KnHashTableValEntry* newValEntry = KnHashTable_CreateValEntry(self, value);
    if (newValEntry == NULL) {
        return NULL;
    }
    newValEntry->prev = valEntry;
    valEntry->next = newValEntry;

    KnHashTableValEntry* oldTail = valEntry->next;
    if ((ktable_val_t)oldTail != KN_HASH_TABLE_NIL) {
        oldTail->prev = newValEntry;
    }
    newValEntry->next = oldTail;

    // update ValListTail
    if (self->valListTail == valEntry) {
        self->valListTail = newValEntry;
    }
    
    self->itemNum += 1;
    return newValEntry;
}

// insert value after iter
int KnHashTable_InsertValAfterIter(KnHashTable* self, KnHashTableIter iter, ktable_val_t value)
{
    KnHashTableValEntry* newValEntry = KnHashTable_CreateValEntryAfterIter(self, iter, value);
    if (newValEntry == NULL) {
        return -1;
    }
    else {
        return 1;
    }
}
// insert k v after iter
int KnHashTable_InsertKvAfterIter(KnHashTable* self, KnHashTableIter iter, char* key, ktable_val_t value)
{
    KnHashTableValEntry* newValEntry = KnHashTable_CreateValEntryAfterIter(self, iter, value);
    if (newValEntry == NULL) {
        return -1;
    }

    KnHashTable_AddOrUpdateKeyEntry(self, key, newValEntry);
    return 1;
}

int KnHashTable_FindRehashPowerOfTwo(KnHashTable* self, int previousN, bool isGrow)
{
    if (isGrow) {
        int nextSize = 2 << (previousN + 1);
        if (nextSize * KN_HASH_TABLE_REHASH_THRESHOLD > self->itemNum) {
            // ok, return
            return previousN + 1;
        }
        else {
            return KnHashTable_FindRehashPowerOfTwo(self, previousN + 1, isGrow);
        }
    }
    else {
        // shink
        int nextSize = 2 << (previousN - 1);
        // min size 8
        if (nextSize == 4
            || (nextSize > self->itemNum
                && (nextSize / 2) < self->itemNum
               )
        ) {
            // ok
            return previousN - 1;
        }
        else {
            return KnHashTable_FindRehashPowerOfTwo(self, previousN - 1, isGrow);
        }
    }
}


void KnHashTable_Rehash(KnHashTable* self, int newPowerOfTwo)
{
    uint32_t newHashSize = 2 << newPowerOfTwo;
    uint32_t newHashMask = newHashSize - 1;
    
    KnHashTableKeyEntry** buckets = (KnHashTableKeyEntry**)tb_nalloc0(newHashSize, sizeof(KnHashTableKeyEntry));
    if (buckets == NULL) {
        return;
    }

    for (uint32_t i = 0; i < newHashSize; i++) {
        *(buckets + i) = KN_HASH_TABLE_NIL;
    }

    // copy two new buckets
    for (uint32_t i = 0; i < self->hashSize; i++) {
        KnHashTableKeyEntry* iter = *(self->buckets + i);
        while ((ktable_val_t)iter != KN_HASH_TABLE_NIL) {
            KnHashTableKeyEntry* next = iter->next;

            uint32_t hashCode = iter->hashCode;
            uint32_t newHashIndex = newHashMask | hashCode;

            if ((ktable_val_t)(*(buckets + newHashIndex)) != KN_HASH_TABLE_NIL) {
                // bucket first entry
                iter->prev = KN_HASH_TABLE_NIL;
                iter->next = KN_HASH_TABLE_NIL;
            }
            else {
                // insert to list head
                KnHashTableKeyEntry* oldBucketFirst = *(buckets + newHashIndex);
                oldBucketFirst->prev = iter;
                iter->next = oldBucketFirst;
                iter->prev = KN_HASH_TABLE_NIL;
            }
            *(buckets + newHashIndex) = iter;

            iter = next;
        }
    }

    uint32_t bucketUsed = 0;
    for (uint32_t i = 0; i < newHashSize; i++) {
        if ((ktable_val_t)(*(buckets + i)) != KN_HASH_TABLE_NIL) {
            bucketUsed += 1;
        }
    }

    self->hashSize = newHashSize;
    self->hashMask = newHashMask;
    self->powerOfTwo = newPowerOfTwo;
    self->bucketUsed = bucketUsed;
}

void KnHashTable_CheckRehash(KnHashTable* self) {

    uint32_t currSize = self->itemNum;
    if (currSize * (1.0 - KN_HASH_TABLE_REHASH_THRESHOLD) > currSize) {
        // shrink
        int n = KnHashTable_FindRehashPowerOfTwo(self, self->powerOfTwo, false);
        KnHashTable_Rehash(self, n);
    }
    else if (currSize * (1.0 + KN_HASH_TABLE_REHASH_THRESHOLD) < currSize) {
        // grow
        int n = KnHashTable_FindRehashPowerOfTwo(self, self->powerOfTwo, true);
        KnHashTable_Rehash(self, n);
    }

}

KnHashTable* KnHashTable_ShadowClone(KnHashTable* source)
{
    if (source == NULL) {
        return NULL;
    }
    KnHashTable* copy = KnHashTable_Init(source->powerOfTwo);
    KnHashTableValEntry* iter = source->valListHead;
    while ((ktable_val_t)iter != KN_HASH_TABLE_NIL) {
        KnHashTableValEntry* next = iter->next;
        if (iter->key != NULL) {
            KnHashTable_PushKv(copy, iter->key, iter->val);
        }
        else {
            KnHashTable_PushVal(copy, iter->val);
        }
        iter = next;
    }
    return copy;
}

// for debug
void KnHashTable_PrintKeys(KnHashTable* self)
{
    // KnHashTableValEntry* iter = self->valListHead;
    // while ((ktable_val_t)iter != KN_HASH_TABLE_NIL) {
    //     KnHashTableValEntry* next = iter->next;
    //     if (iter->key != NULL) {
    //         printf("key: %s\n", iter->key);
    //     }
    //     iter = next;
    // }
}
