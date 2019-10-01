/**
 * KxHashTable
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

KxHashTable* KxHashTable_Init(uint32_t powerOfTwo)
{
    KxHashTable* self = (KxHashTable*)tb_nalloc0(1, sizeof(KxHashTable));
    if (self == NULL) {
        return NULL;
    }
    
    uint32_t hashSize = 2 << powerOfTwo;
    self->hashSize = hashSize;
    self->hashMask = hashSize - 1;
    self->powerOfTwo = powerOfTwo;
    self->bucketUsed = 0;
    self->itemNum = 0;
    self->valListHead = KX_HASH_TABLE_NIL;
    self->valListTail = KX_HASH_TABLE_NIL;

    KxHashTableKeyEntry** buckets = (KxHashTableKeyEntry**)tb_nalloc0(hashSize, sizeof(void*));
    if (buckets == NULL) {
        tb_free(self);
        return NULL;
    }

    for (uint32_t i = 0; i < hashSize; i++) {
        *(buckets + i) = KX_HASH_TABLE_NIL;
    }
    self->buckets = buckets;

    return self;
}

int KxHashTable_Destroy(KxHashTable* self)
{
    if (self == NULL) {
        return -1;
    }
    KxHashTable_Clear(self);
    tb_free(self);
    return 1;
}

// clear keys in this bucket and set value's key to NIL
void KxHashTable_ClearBucketKeys(KxHashTable* self, KxHashTableKeyEntry* bucket)
{
    if ((ktable_val_t)bucket == KX_HASH_TABLE_NIL) {
        return;
    }
    KxHashTableKeyEntry* iter = bucket;
    while ((ktable_val_t)iter != KX_HASH_TABLE_NIL) {
        KxHashTableKeyEntry* next = iter->next;
        if ((ktable_val_t)iter->valEntry != KX_HASH_TABLE_NIL) {
            tb_free(iter->valEntry->key);
            iter->valEntry->key = NULL;
        }
        tb_free(iter);
        iter = next;
    }
}

void KxHashTable_ClearKeys(KxHashTable* self)
{
    KxHashTableKeyEntry** buckets = self->buckets;

    self->bucketUsed = 0;
    for (uint32_t i = 0; i < self->hashSize; i++) {
        KxHashTableKeyEntry* bucket = (KxHashTableKeyEntry*)(*(buckets + i));
        KxHashTable_ClearBucketKeys(self, bucket);
        *(buckets + i) = KX_HASH_TABLE_NIL;
    }
}

// only clear one key entry, and update prev, next ref
void KxHashTable_DelKeyEntry(KxHashTable* self, KxHashTableKeyEntry* keyEntry) {
    if ((ktable_val_t)keyEntry == KX_HASH_TABLE_NIL) {
        return;
    }

    // if the entry is the bucket first, update bucket
    uint32_t hashIndex = KxHashTable_KeyHashIndex(self, keyEntry->hashCode);
    if (self->buckets[hashIndex] == keyEntry) {
        self->buckets[hashIndex] = keyEntry->next;
        if ((ktable_val_t)keyEntry->next == KX_HASH_TABLE_NIL) {
            self->bucketUsed -= 1;
        }
    }
    
    if ((ktable_val_t)keyEntry->next != KX_HASH_TABLE_NIL) {
        keyEntry->next->prev = keyEntry->prev;
    }

    if ((ktable_val_t)keyEntry->prev != KX_HASH_TABLE_NIL) {
        keyEntry->prev->next = keyEntry->next;
    }

    tb_free(keyEntry);

    KxHashTable_CheckRehash(self);
}

// assume all keys are cleared
// !!! must be called after KxHashTable_ClearKeys
void KxHashTable_ClearValues(KxHashTable* self)
{
    KxHashTableValEntry* iter = self->valListHead;
    while ((ktable_val_t)iter != KX_HASH_TABLE_NIL) {
        KxHashTableValEntry* next = iter->next;
        if (iter->key != NULL) {
            tb_free(iter->key);
        }
        tb_free(iter);
        iter = next;
    }
}

int KxHashTable_Clear(KxHashTable* self)
{
    // free keys, and set value's key to NIL
    KxHashTable_ClearKeys(self);
    // KxHashTable_ClearValues(self);
    self->valListHead = KX_HASH_TABLE_NIL;
    self->valListTail = KX_HASH_TABLE_NIL;
    self->itemNum = 0;
    return 1;
}

// number of elements
uint32_t KxHashTable_Length(KxHashTable* self)
{
    return self->itemNum;
}

uint32_t KxHashTable_KeyHashCode(char* key, uint32_t keyLen)
{
    uint32_t hashCode = MurmurHash32(key, keyLen, KX_HASH_INDEX_SEED);
    return hashCode;
}

uint32_t KxHashTable_KeyHashIndex(KxHashTable* self, uint32_t hashCode)
{
    uint32_t hashIndex = self->hashMask & hashCode;
    return hashIndex;
}

uint32_t KxHashTable_KeyVerifyCode(char* key, int keyLen, uint32_t hashCode)
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
KxHashTableKeyEntry* KxHashTable_GetKeyEntry(KxHashTable* self, char* key)
{
    if (key == NULL) {
        return KX_HASH_TABLE_UNDEF;
    }
    uint32_t keyLen = strlen(key);
    uint32_t hashCode = KxHashTable_KeyHashCode(key, keyLen);
    uint32_t hashIndex = KxHashTable_KeyHashIndex(self, hashCode);
    uint32_t verifyCode = KxHashTable_KeyVerifyCode(key, keyLen, hashCode);

    KxHashTableKeyEntry* bucket = self->buckets[hashIndex];

    if ((ktable_val_t)bucket == KX_HASH_TABLE_NIL) {
        return KX_HASH_TABLE_UNDEF;
    }
    else {
        KxHashTableKeyEntry* iter = bucket;
        while ((ktable_val_t)iter != KX_HASH_TABLE_NIL) {
            KxHashTableKeyEntry* next = iter->next;
            if (iter->hashCode == hashCode
                && iter->keyLen == keyLen
                && iter->verifyCode == verifyCode
            ) {
                break;
            }
            iter = next;
        }
        if ((ktable_val_t)iter == KX_HASH_TABLE_NIL) {
            return KX_HASH_TABLE_UNDEF;
        }
        else {
            return iter;
        }
    }
}

bool KxHashTable_HasKey(KxHashTable* self, const char* key)
{
    KxHashTableKeyEntry* keyEntry = KxHashTable_GetKeyEntry(self, key);
    if ((ktable_val_t)keyEntry == KX_HASH_TABLE_UNDEF) {
        return false;
    }
    else {
        return true;
    }
}

KxHashTableValEntry* KxHashTable_ValEntryAtKey(KxHashTable* self, char* key)
{
    KxHashTableKeyEntry* keyEntry = KxHashTable_GetKeyEntry(self, key);
    if ((ktable_val_t)keyEntry == KX_HASH_TABLE_UNDEF) {
        return NULL;
    }
    else {
        return keyEntry->valEntry;;
    }
}
// get by key string
ktable_val_t KxHashTable_AtKey(KxHashTable* self, const char* key)
{
    if (key == NULL) {
        return KX_HASH_TABLE_UNDEF;
    }

    KxHashTableValEntry* valEntry = KxHashTable_ValEntryAtKey(self, key);
    
    // return KX_HASH_TABLE_UNDEF;
    if (valEntry == NULL) {
        return KX_HASH_TABLE_UNDEF;
    }

    return valEntry->val;
}

KxHashTableValEntry* KxHashTable_ValEntryAtIndex(KxHashTable* self, int index)
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
        KxHashTableValEntry* iter = self->valListTail;
        while ((ktable_val_t)iter != KX_HASH_TABLE_NIL) {
            KxHashTableValEntry* next = iter->prev;
            if (curIndex == index) {
                return iter;
            }
            iter = next;
            curIndex--;
        }
    }
    else {
        uint32_t curIndex = 0;
        KxHashTableValEntry* iter = self->valListHead;
        while ((ktable_val_t)iter != KX_HASH_TABLE_NIL) {
            KxHashTableValEntry* next = iter->next;
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
ktable_val_t KxHashTable_ValAtIndex(KxHashTable* self, int index)
{
    KxHashTableValEntry* valEntry = KxHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return KX_HASH_TABLE_UNDEF;
    }
    return valEntry->val;    
}

const char* KxHashTable_KeyAtIndex(KxHashTable* self, int index)
{
    KxHashTableValEntry* valEntry = KxHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return NULL;
    }
    return valEntry->key;    
}

ktable_val_t KxHashTable_FirstVal(KxHashTable* self)
{
    return self->valListHead->val;
}

ktable_val_t KxHashTable_LastVal(KxHashTable* self)
{
    return self->valListTail->val;
}

// init a key entry
KxHashTableKeyEntry* KxHashTable_CreateKeyEntry(KxHashTable* self, char* key)
{
    if (key == NULL) {
        return NULL;
    }
    KxHashTableKeyEntry* entry = (KxHashTableKeyEntry*)tb_nalloc0(1, sizeof(KxHashTableKeyEntry));
    if (entry == NULL) {
        return NULL;
    }
    entry->keyLen = strlen(key);

    uint32_t keyLen = strlen(key);
    uint32_t hashCode = KxHashTable_KeyHashCode(key, keyLen);
    uint32_t hashIndex = KxHashTable_KeyHashIndex(self, hashCode);
    uint32_t verifyCode = KxHashTable_KeyVerifyCode(key, keyLen, hashCode);

    entry->keyLen = keyLen;
    entry->hashCode = hashCode;
    entry->verifyCode = verifyCode;
    entry->valEntry = KX_HASH_TABLE_NIL;
    entry->prev = KX_HASH_TABLE_NIL;
    entry->next = KX_HASH_TABLE_NIL;
    return entry;
}

KxHashTableValEntry* KxHashTable_CreateValEntry(KxHashTable* self, ktable_val_t value)
{
    KxHashTableValEntry* entry = (KxHashTableValEntry*)tb_nalloc0(1, sizeof(KxHashTableValEntry));
    if (entry == NULL) {
        return NULL;
    }
    entry->key = NULL;
    entry->val = value;
    entry->prev = KX_HASH_TABLE_NIL;
    entry->next = KX_HASH_TABLE_NIL;
    return entry;
}

// insert new value entry to tail
// and update ValListTail and ValListHead
void KxHashTable_PushValEntry(KxHashTable* self, KxHashTableValEntry* valEntry)
{
    KxHashTableValEntry* oldTail = self->valListTail;
    if ((ktable_val_t)oldTail != KX_HASH_TABLE_NIL) {
        oldTail->next = valEntry;
    }

    // if is the first val
    if ((ktable_val_t)self->valListHead == KX_HASH_TABLE_NIL) {
        self->valListHead = valEntry;
    }
    
    valEntry->prev = oldTail;
    self->valListTail = valEntry;
    self->itemNum += 1;
}

// insert new value entry to head
// and update ValListTail and ValListHead
void KxHashTable_UnshiftValEntry(KxHashTable* self, KxHashTableValEntry* valEntry)
{
    KxHashTableValEntry* oldHead = self->valListHead;
    if ((ktable_val_t)oldHead != KX_HASH_TABLE_NIL) {
        oldHead->prev = valEntry;
    }

    // if is the first val
    if ((ktable_val_t)self->valListTail == KX_HASH_TABLE_NIL) {
        self->valListTail = valEntry;
    }

    valEntry->next = oldHead;
    self->valListHead = valEntry;
    self->itemNum += 1;
}

// update the cstr key of a k v pair.
// free the old key
void KxHashTable_UpdateEntryKeyCstr(KxHashTableValEntry* valEntry, char* key)
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
KxHashTableKeyEntry* KxHashTable_AddOrUpdateKeyEntry(KxHashTable* self, char* key, KxHashTableValEntry* valEntry)
{
    // set entry key
    KxHashTable_UpdateEntryKeyCstr(valEntry, key);

    uint32_t keyLen = strlen(key);
    uint32_t hashCode = KxHashTable_KeyHashCode(key, keyLen);
    uint32_t hashIndex = KxHashTable_KeyHashIndex(self, hashCode);
    KxHashTableKeyEntry* bucket = self->buckets[hashIndex];
    
    // find or create key entry
    KxHashTableKeyEntry* keyEntry = KxHashTable_GetKeyEntry(self, key);
    if ((ktable_val_t)keyEntry != KX_HASH_TABLE_UNDEF) {
        // free origin key
        if (keyEntry->valEntry->key != NULL) {
            tb_free(keyEntry->valEntry->key);
            keyEntry->valEntry->key = NULL;
        }
        keyEntry->valEntry = valEntry;
        return keyEntry;
    }
    keyEntry = KxHashTable_CreateKeyEntry(self, key);

    if (keyEntry == NULL) {
        return NULL;
    }

    // insert new key entry as bucket first
    if ((ktable_val_t)self->buckets[hashIndex] != KX_HASH_TABLE_NIL) {
        self->buckets[hashIndex]->prev = keyEntry;
        keyEntry->next = self->buckets[hashIndex]->prev;
    }
    else {
        self->bucketUsed += 1;
    }
    self->buckets[hashIndex] = keyEntry;

    keyEntry->valEntry = valEntry;

    KxHashTable_CheckRehash(self);
    return keyEntry;
}

int KxHashTable_PushVal(KxHashTable* self, ktable_val_t value)
{
    KxHashTableValEntry* valEntry = KxHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    KxHashTable_PushValEntry(self, valEntry);
    return 1;
}

// push new val to tail
int KxHashTable_PushKv(KxHashTable* self, char* key, ktable_val_t value)
{
    if (key == NULL) {
        return -1;
    }

    KxHashTableValEntry* valEntry = KxHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    KxHashTableKeyEntry* keyEntry = KxHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }

    KxHashTable_PushValEntry(self, valEntry);
    return 1;
}

// add value to head
int KxHashTable_UnshiftVal(KxHashTable* self, ktable_val_t value)
{
    KxHashTableValEntry* valEntry = KxHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    KxHashTable_UnshiftValEntry(self, valEntry);
    return 1;
}
// add key value to head
int KxHashTable_UnshiftKv(KxHashTable* self, const char* key, ktable_val_t value)
{
    if (key == NULL) {
        return -1;
    }

    KxHashTableValEntry* valEntry = KxHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    KxHashTableKeyEntry* keyEntry = KxHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }
    KxHashTable_UnshiftValEntry(self, valEntry);
    return 1;
}

int KxHashTable_PutKv(KxHashTable* self, const char* key, ktable_val_t value)
{
    if (key == NULL) {
        return -1;
    }

    uint32_t keyLen = strlen(key);
    uint32_t hashCode = KxHashTable_KeyHashCode(key, keyLen);
    uint32_t hashIndex = KxHashTable_KeyHashIndex(self, hashCode);

    KxHashTableKeyEntry* keyEntry = KxHashTable_GetKeyEntry(self, key);
    if ((ktable_val_t)keyEntry != KX_HASH_TABLE_UNDEF) {
        // update val
        keyEntry->valEntry->val = value;
        KxHashTable_UpdateEntryKeyCstr(keyEntry->valEntry, key);
        return 2;
    }

    // add new k v
    keyEntry = KxHashTable_CreateKeyEntry(self, key);
    if (keyEntry == NULL) {
        return -1;
    }

    KxHashTableValEntry* valEntry = KxHashTable_CreateValEntry(self, value);
    if (valEntry == NULL) {
        return -1;
    }

    keyEntry->valEntry = valEntry;
    KxHashTable_UpdateEntryKeyCstr(valEntry, key);

    // insert new key entry as bucket first
    if ((ktable_val_t)self->buckets[hashIndex] != KX_HASH_TABLE_NIL) {
        self->buckets[hashIndex]->prev = keyEntry;
        keyEntry->next = self->buckets[hashIndex];
    }
    else {
        self->buckets[hashIndex] += 1;
    }
    self->buckets[hashIndex] = keyEntry;
    KxHashTable_PushValEntry(self, valEntry);


    return 1;
    
}

// set or update index item's key, if out of range, do nothing
// result 0: invalid or fail; 1 insert; 2 update
int KxHashTable_SetKeyAtIndex(KxHashTable* self, int index, const char* key)
{
    KxHashTableValEntry* valEntry = KxHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        printf("KxHashTable_SetKeyAtIndex valEntry not found\n");
        return -1;
    }

    KxHashTableKeyEntry* keyEntry = KxHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }
    KxHashTable_UpdateEntryKeyCstr(valEntry, key);
    return 1;
}

// set value by index, if out of range, do nothing
int KxHashTable_SetValAtIndex(KxHashTable* self, int index, ktable_val_t value)
{
    KxHashTableValEntry* valEntry = KxHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return -1;
    }

    valEntry->val = value;
    return 1;
}

// set index item value, if out of range, do nothing
// and update the key's value to this index
int KxHashTable_SetKvAtIndex(KxHashTable* self, int index, const char* key, ktable_val_t value)
{
    KxHashTableValEntry* valEntry = KxHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return -1;
    }

    valEntry->val = value;

    KxHashTableKeyEntry* keyEntry = KxHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }
    KxHashTable_UpdateEntryKeyCstr(valEntry, key);
    return 1;
}

void KxHashTable_DelValEntry(KxHashTable* self, KxHashTableValEntry* valEntry)
{
    if (valEntry->key != NULL) {
        KxHashTableKeyEntry* keyEntry = KxHashTable_GetKeyEntry(self, valEntry->key);
        if ((ktable_val_t)keyEntry != KX_HASH_TABLE_UNDEF) {
            // remove old key
            KxHashTable_DelKeyEntry(self, keyEntry);
        }
        tb_free(valEntry->key);
    }

    if ((ktable_val_t)valEntry->prev != KX_HASH_TABLE_NIL) {
        valEntry->prev->next = valEntry->next;
    }

    if ((ktable_val_t)valEntry->next != KX_HASH_TABLE_NIL) {
        valEntry->next->prev = valEntry->prev;
    }
    self->itemNum -= 1;

    tb_free(valEntry);
}

// del by key
int KxHashTable_DelByKey(KxHashTable* self, const char* key)
{
    if (key == NULL) {
        return KX_HASH_TABLE_UNDEF;
    }

    KxHashTableValEntry* valEntry = KxHashTable_ValEntryAtKey(self, key);
 
    if (valEntry == NULL) {
        return KX_HASH_TABLE_UNDEF;
    }

    KxHashTable_DelValEntry(self, valEntry);
    return 1;
}

// del by index, if out of range, do nothing
int KxHashTable_DelByIndex(KxHashTable* self, int index)
{
    KxHashTableValEntry* valEntry = KxHashTable_ValEntryAtIndex(self, index);
    if (valEntry == NULL) {
        return -1;
    }

    KxHashTable_DelValEntry(self, valEntry);
    return 1;
}


////
// iterator func

KxHashTableIter KxHashTable_IterHead(KxHashTable* self)
{
    return (KxHashTableIter)self->valListHead;
}

KxHashTableIter KxHashTable_IterTail(KxHashTable* self)
{
    return (KxHashTableIter)self->valListTail;
}

bool KxHashTable_IterHasNext(KxHashTable* self, KxHashTableIter iter)
{
    KxHashTableValEntry* valEntry = (KxHashTableValEntry*)iter;
    return ((ktable_val_t)iter->next == KX_HASH_TABLE_NIL) ? false : true;
}

KxHashTableIter KxHashTable_IterNext(KxHashTable* self, KxHashTableIter iter)
{
    if ((ktable_val_t)iter == KX_HASH_TABLE_NIL) {
        return (KxHashTableIter)KX_HASH_TABLE_NIL;
    }
    KxHashTableValEntry* valEntry = (KxHashTableValEntry*)iter;

    return (KxHashTableIter)(valEntry->next);
}

bool KxHashTable_IterHasPrev(KxHashTable* self, KxHashTableIter iter)
{
    KxHashTableValEntry* valEntry = (KxHashTableValEntry*)iter;
    return ((ktable_val_t)iter->prev == KX_HASH_TABLE_NIL) ? false : true;
}

KxHashTableIter KxHashTable_IterPrev(KxHashTable* self, KxHashTableIter iter)
{
    if ((ktable_val_t)iter == KX_HASH_TABLE_NIL) {
        return (KxHashTableIter)KX_HASH_TABLE_NIL;
    }
    KxHashTableValEntry* valEntry = (KxHashTableValEntry*)iter;

    return (KxHashTableIter)(valEntry->prev);
}

const char* KxHashTable_IterGetKey(KxHashTable* self, KxHashTableIter iter)
{
    if ((ktable_val_t)iter == KX_HASH_TABLE_NIL) {
        return NULL;
    }
    KxHashTableValEntry* valEntry = (KxHashTableValEntry*)iter;
    return valEntry->key;
}

ktable_val_t KxHashTable_IterGetVal(KxHashTable* self, KxHashTableIter iter)
{
    if ((ktable_val_t)iter == KX_HASH_TABLE_NIL) {
        return KX_HASH_TABLE_UNDEF;
    }
    KxHashTableValEntry* valEntry = (KxHashTableValEntry*)iter;
    return valEntry->val;
}

// set iter item key
// result 0: invalid or fail; 1 insert; 2 update
int KxHashTable_IterSetKey(KxHashTable* self, KxHashTableIter iter, char* key)
{
    KxHashTableValEntry* valEntry = (KxHashTableValEntry*)iter;
    if (valEntry == NULL || (ktable_val_t)iter == KX_HASH_TABLE_NIL) {
        return -1;
    }

    KxHashTableKeyEntry* keyEntry = KxHashTable_AddOrUpdateKeyEntry(self, key, valEntry);
    if (keyEntry == NULL) {
        return -1;
    }
    KxHashTable_UpdateEntryKeyCstr(valEntry, key);
    return 1;
}
// set iter item value
int KxHashTable_IterSetVal(KxHashTable* self, KxHashTableIter iter, ktable_val_t value)
{
    KxHashTableValEntry* valEntry = (KxHashTableValEntry*)iter;
    if (valEntry == NULL || (ktable_val_t)iter == KX_HASH_TABLE_NIL) {
        return -1;
    }
    valEntry->val = value;
    return 1;
}


// del key and value
int KxHashTable_DelByIter(KxHashTable* self, KxHashTableIter iter, char* key)
{
    KxHashTableValEntry* valEntry = (KxHashTableValEntry*)iter;
    if (valEntry == NULL || (ktable_val_t)iter == KX_HASH_TABLE_NIL) {
        return -1;
    }
    KxHashTable_DelValEntry(self, valEntry);
    return 1;
}

// insert value before iter
KxHashTableValEntry* KxHashTable_CreateValEntryBeforeIter(KxHashTable* self, KxHashTableIter iter, ktable_val_t value)
{
    KxHashTableValEntry* valEntry = (KxHashTableValEntry*)iter;
    if (valEntry == NULL || (ktable_val_t)iter == KX_HASH_TABLE_NIL) {
        return NULL;
    }

    KxHashTableValEntry* newValEntry = KxHashTable_CreateValEntry(self, value);
    if (newValEntry == NULL) {
        return NULL;
    }
    newValEntry->next = valEntry;
    valEntry->prev = newValEntry;

    KxHashTableValEntry* oldHead = valEntry->prev;
    if ((ktable_val_t)oldHead != KX_HASH_TABLE_NIL) {
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

int KxHashTable_InsertValBeforeIter(KxHashTable* self, KxHashTableIter iter, ktable_val_t value)
{
    KxHashTableValEntry* newValEntry = KxHashTable_CreateValEntryBeforeIter(self, iter, value);
    if (newValEntry == NULL) {
        return -1;
    }
    else {
        return 1;
    }
}

// insert k v before iter
int KxHashTable_InsertKvBeforIter(KxHashTable* self, KxHashTableIter iter, char* key, ktable_val_t value)
{
    KxHashTableValEntry* newValEntry = KxHashTable_CreateValEntryBeforeIter(self, iter, value);
    if (newValEntry == NULL) {
        return -1;
    }

    KxHashTable_AddOrUpdateKeyEntry(self, key, newValEntry);
    return 1;
}



KxHashTableValEntry* KxHashTable_CreateValEntryAfterIter(KxHashTable* self, KxHashTableIter iter, ktable_val_t value)
{
    KxHashTableValEntry* valEntry = (KxHashTableValEntry*)iter;
    if (valEntry == NULL || (ktable_val_t)iter == KX_HASH_TABLE_NIL) {
        return NULL;
    }

    KxHashTableValEntry* newValEntry = KxHashTable_CreateValEntry(self, value);
    if (newValEntry == NULL) {
        return NULL;
    }
    newValEntry->prev = valEntry;
    valEntry->next = newValEntry;

    KxHashTableValEntry* oldTail = valEntry->next;
    if ((ktable_val_t)oldTail != KX_HASH_TABLE_NIL) {
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
int KxHashTable_InsertValAfterIter(KxHashTable* self, KxHashTableIter iter, ktable_val_t value)
{
    KxHashTableValEntry* newValEntry = KxHashTable_CreateValEntryAfterIter(self, iter, value);
    if (newValEntry == NULL) {
        return -1;
    }
    else {
        return 1;
    }
}
// insert k v after iter
int KxHashTable_InsertKvAfterIter(KxHashTable* self, KxHashTableIter iter, char* key, ktable_val_t value)
{
    KxHashTableValEntry* newValEntry = KxHashTable_CreateValEntryAfterIter(self, iter, value);
    if (newValEntry == NULL) {
        return -1;
    }

    KxHashTable_AddOrUpdateKeyEntry(self, key, newValEntry);
    return 1;
}

int KxHashTable_FindRehashPowerOfTwo(KxHashTable* self, int previousN, bool isGrow)
{
    if (isGrow) {
        int nextSize = 2 << (previousN + 1);
        if (nextSize * KX_HASH_TABLE_REHASH_THRESHOLD > self->itemNum) {
            // ok, return
            return previousN + 1;
        }
        else {
            return KxHashTable_FindRehashPowerOfTwo(self, previousN + 1, isGrow);
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
            return KxHashTable_FindRehashPowerOfTwo(self, previousN - 1, isGrow);
        }
    }
}


void KxHashTable_Rehash(KxHashTable* self, int newPowerOfTwo)
{
    uint32_t newHashSize = 2 << newPowerOfTwo;
    uint32_t newHashMask = newHashSize - 1;
    
    KxHashTableKeyEntry** buckets = (KxHashTableKeyEntry**)tb_nalloc0(newHashSize, sizeof(KxHashTableKeyEntry));
    if (buckets == NULL) {
        return;
    }

    for (uint32_t i = 0; i < newHashSize; i++) {
        *(buckets + i) = KX_HASH_TABLE_NIL;
    }

    // copy two new buckets
    for (uint32_t i = 0; i < self->hashSize; i++) {
        KxHashTableKeyEntry* iter = *(self->buckets + i);
        while ((ktable_val_t)iter != KX_HASH_TABLE_NIL) {
            KxHashTableKeyEntry* next = iter->next;

            uint32_t hashCode = iter->hashCode;
            uint32_t newHashIndex = newHashMask | hashCode;

            if ((ktable_val_t)(*(buckets + newHashIndex)) != KX_HASH_TABLE_NIL) {
                // bucket first entry
                iter->prev = KX_HASH_TABLE_NIL;
                iter->next = KX_HASH_TABLE_NIL;
            }
            else {
                // insert to list head
                KxHashTableKeyEntry* oldBucketFirst = *(buckets + newHashIndex);
                oldBucketFirst->prev = iter;
                iter->next = oldBucketFirst;
                iter->prev = KX_HASH_TABLE_NIL;
            }
            *(buckets + newHashIndex) = iter;

            iter = next;
        }
    }

    uint32_t bucketUsed = 0;
    for (uint32_t i = 0; i < newHashSize; i++) {
        if ((ktable_val_t)(*(buckets + i)) != KX_HASH_TABLE_NIL) {
            bucketUsed += 1;
        }
    }

    self->hashSize = newHashSize;
    self->hashMask = newHashMask;
    self->powerOfTwo = newPowerOfTwo;
    self->bucketUsed = bucketUsed;
}

void KxHashTable_CheckRehash(KxHashTable* self) {

    uint32_t currSize = self->itemNum;
    if (currSize * (1.0 - KX_HASH_TABLE_REHASH_THRESHOLD) > currSize) {
        // shrink
        int n = KxHashTable_FindRehashPowerOfTwo(self, self->powerOfTwo, false);
        KxHashTable_Rehash(self, n);
    }
    else if (currSize * (1.0 + KX_HASH_TABLE_REHASH_THRESHOLD) < currSize) {
        // grow
        int n = KxHashTable_FindRehashPowerOfTwo(self, self->powerOfTwo, true);
        KxHashTable_Rehash(self, n);
    }

}

KxHashTable* KxHashTable_ShadowClone(KxHashTable* source)
{
    if (source == NULL) {
        return NULL;
    }
    KxHashTable* copy = KxHashTable_Init(source->powerOfTwo);
    KxHashTableValEntry* iter = source->valListHead;
    while ((ktable_val_t)iter != KX_HASH_TABLE_NIL) {
        KxHashTableValEntry* next = iter->next;
        if (iter->key != NULL) {
            KxHashTable_PushKv(copy, iter->key, iter->val);
        }
        else {
            KxHashTable_PushVal(copy, iter->val);
        }
        iter = next;
    }
    return copy;
}

// for debug
void KxHashTable_PrintKeys(KxHashTable* self)
{
    // KxHashTableValEntry* iter = self->valListHead;
    // while ((ktable_val_t)iter != KX_HASH_TABLE_NIL) {
    //     KxHashTableValEntry* next = iter->next;
    //     if (iter->key != NULL) {
    //         printf("key: %s\n", iter->key);
    //     }
    //     iter = next;
    // }
}
