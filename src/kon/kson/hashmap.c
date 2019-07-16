#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "murmurhash.h"
#include "hashmap.h"

#define KON_HASH_INDEX_SEED 31
#define KON_HASH_CRC_SEED 25

KonHashMap* KON_HashMapInit(uint32_t size)
{
    KonHashMap* hashmap = (KonHashMap*)malloc(sizeof(KonHashMap));
    if (hashmap == NULL) {
        return NULL;
    }
    hashmap->HashSize = size;
    hashmap->ItemSize = 0;
    KonHashMapEntry** buckets = (KonHashMapEntry**)malloc(size * sizeof(KonHashMapEntry*));
    if (buckets == NULL) {
        free(hashmap);
        return NULL;
    }
    hashmap->BucketVector = buckets;
    for (uint32_t i = 0; i < size; i++) {
        // printf("buckets+i addr %x, addr value %x\n", (buckets + i), *(buckets + i));
        *(buckets + i) = NULL;
    }
    return hashmap;
}
void KON_HashMapDestroy(KonHashMap* hashmap)
{
    KON_HashMapClear(hashmap);
    free(hashmap);
}

void KON_HashMapDelEntryList(KonHashMapEntry* bucket)
{
    if (bucket == NULL) {
        return;
    }
    KonHashMapEntry* iter = bucket;
    do {
        KonHashMapEntry* next = iter->Next;
        free(iter->Key);
        free(iter);
        iter = next;
    } while(iter != NULL);
}

void KON_HashMapClear(KonHashMap* hashmap)
{
    hashmap->ItemSize = 0;
    KonHashMapEntry** first = (KonHashMapEntry**)hashmap->BucketVector;
    for (uint32_t i = 0; i < hashmap->HashSize; i++) {
        KonHashMapEntry* hashBucket = (KonHashMapEntry*)(*(first + i));
        KON_HashMapDelEntryList(hashBucket);
        *(first + i) = NULL;
    }
}

KonHashMapEntry* KON_HashMapCreateEntry(uint32_t hashCode, char* key, void* value)
{
    KonHashMapEntry* entry = (KonHashMapEntry*)malloc(sizeof(KonHashMapEntry));
    entry->HashCode = hashCode;
    entry->CrcCode = 0;
    entry->Next = NULL;
    entry->Prev = NULL;
    entry->KeyLen = strlen(key);
    char* copiedKey = (char*)malloc((entry->KeyLen + 1) * sizeof(char));
    strncpy(copiedKey, key, entry->KeyLen);
    copiedKey[entry->KeyLen] = '\0';
    entry->Key = copiedKey;
    entry->Data = value;
    return entry;
}

void KON_HashMapPut(KonHashMap* hashmap, char* key, void* value)
{
    if (key == NULL) {
        return;
    }
    uint32_t keyLen = strlen(key);
    uint32_t hashCode = MurmurHash32(key, keyLen, KON_HASH_INDEX_SEED);
    
    uint32_t hashIndex = hashCode % hashmap->HashSize;
    // printf("entry key %s, hash index %ld, hash code %ld\n", key, hashIndex, hashCode);

    KonHashMapEntry** first = (KonHashMapEntry**)hashmap->BucketVector;
    KonHashMapEntry* bucket = first[hashIndex];
    
    if (bucket == NULL) {
        *(first + hashIndex) = KON_HashMapCreateEntry(hashCode, key, value);
        hashmap->ItemSize += 1;
    }
    else {
        KonHashMapEntry* iter = bucket;
        KonHashMapEntry* existEntry = NULL;
        uint32_t keyCrcCode = 0;

        do {
            KonHashMapEntry* next = iter->Next;
            // verify if is same key
            if (iter->HashCode == hashCode && iter->KeyLen == keyLen) {
                // generate query key crc code
                if (keyCrcCode == 0) {
                    keyCrcCode = MurmurHash32(key, keyLen, hashCode);
                }
                // generate entry crc code
                if (iter->CrcCode == 0) {
                    iter->CrcCode = MurmurHash32(iter->Key, iter->KeyLen, hashCode);
                }
                // not use string equal
                // may be collision
                if (iter->CrcCode == keyCrcCode) {
                    existEntry = iter;
                    break;
                }
            }

            iter = next;
        } while (iter != NULL);

        if (existEntry != NULL) {
            // update entry
            existEntry->Data = value;
        }
        else {
            // insert new entry to list head
            KonHashMapEntry* oldHead = bucket;
            KonHashMapEntry* newEntry = KON_HashMapCreateEntry(hashCode, key, value);
            *(first + hashIndex) = newEntry;
            newEntry->Next = oldHead;
            oldHead->Prev = newEntry;
            hashmap->ItemSize += 1;
        }
    }
}

void* KON_HashMapGet(KonHashMap* hashmap, char* key)
{
    if (key == NULL) {
        return NULL;;
    }
    uint32_t keyLen = strlen(key);
    uint32_t hashCode = MurmurHash32(key, keyLen, KON_HASH_INDEX_SEED);
    uint32_t hashIndex = hashCode % hashmap->HashSize;
    KonHashMapEntry** first = (KonHashMapEntry**)hashmap->BucketVector;
    KonHashMapEntry* bucket = first[hashIndex];
    
    if (bucket == NULL) {
        return NULL;
    }
    else {
        KonHashMapEntry* iter = bucket;
        KonHashMapEntry* existEntry = NULL;
        uint32_t keyCrcCode = 0;

        do {
            KonHashMapEntry* next = iter->Next;
            // verify if is same key
            if (iter->HashCode == hashCode && iter->KeyLen == keyLen) {
                // generate query key crc code
                if (keyCrcCode == 0) {
                    keyCrcCode = MurmurHash32(key, keyLen, hashCode);
                }
                // generate entry crc code
                if (iter->CrcCode == 0) {
                    iter->CrcCode = MurmurHash32(iter->Key, iter->KeyLen, hashCode);
                }
                // not use string equal
                // may be collision
                if (iter->CrcCode == keyCrcCode) {
                    existEntry = iter;
                    break;
                }
            }

            iter = next;
        } while (iter != NULL);

        if (existEntry != NULL) {
            return existEntry->Data;
        }
        else {
            return NULL;
        }
    }
}

void KON_HashMapDel(KonHashMap* hashmap, char* key)
{
    if (key == NULL) {
        return;
    }
    uint32_t keyLen = strlen(key);
    uint32_t hashCode = MurmurHash32(key, keyLen, KON_HASH_INDEX_SEED);
    uint32_t hashIndex = hashCode % hashmap->HashSize;
    KonHashMapEntry** first = (KonHashMapEntry**)hashmap->BucketVector;
    KonHashMapEntry* bucket = first[hashIndex];
    
    if (bucket == NULL) {
        return;
    }
    else {
        KonHashMapEntry* iter = bucket;
        KonHashMapEntry* existEntry = NULL;
        uint32_t keyCrcCode = 0;

        do {
            KonHashMapEntry* next = iter->Next;
            // verify if is same key
            if (iter->HashCode == hashCode && iter->KeyLen == keyLen) {
                // generate query key crc code
                if (keyCrcCode == 0) {
                    keyCrcCode = MurmurHash32(key, keyLen, hashCode);
                }
                // generate entry crc code
                if (iter->CrcCode == 0) {
                    iter->CrcCode = MurmurHash32(iter->Key, iter->KeyLen, hashCode);
                }
                // not use string equal
                // may be collision
                if (iter->CrcCode == keyCrcCode) {
                    existEntry = iter;
                    
                    // do delete stuff
                    iter->Prev->Next = next;
                    if (next != NULL) {
                        next->Prev = iter->Prev;
                    }
                    free(existEntry->Key);
                    free(existEntry);
                    
                    
                    break;
                }
            }

            iter = next;
        } while (iter != NULL);
    }
}

bool KON_HashMapHasKey(KonHashMap* hashmap, char* key)
{
    if (key == NULL) {
        return false;
    }
    uint32_t keyLen = strlen(key);
    uint32_t hashCode = MurmurHash32(key, keyLen, KON_HASH_INDEX_SEED);
    uint32_t hashIndex = hashCode % hashmap->HashSize;
    KonHashMapEntry** first = (KonHashMapEntry**)hashmap->BucketVector;
    KonHashMapEntry* bucket = first[hashIndex];

    if (bucket == NULL) {
        return false;
    }
    else {
        KonHashMapEntry* iter = bucket;
        KonHashMapEntry* existEntry = NULL;
        uint32_t keyCrcCode = 0;

        do {
            KonHashMapEntry* next = iter->Next;
            // verify if is same key
            if (iter->HashCode == hashCode && iter->KeyLen == keyLen) {
                // generate query key crc code
                if (keyCrcCode == 0) {
                    keyCrcCode = MurmurHash32(key, keyLen, hashCode);
                }
                // generate entry crc code
                if (iter->CrcCode == 0) {
                    iter->CrcCode = MurmurHash32(iter->Key, iter->KeyLen, hashCode);
                }
                // not use string equal
                // may be collision
                if (iter->CrcCode == keyCrcCode) {
                    existEntry = iter;
                    break;
                }
            }

            iter = next;
        } while (iter != NULL);

        if (existEntry != NULL) {
            return true;
        }
        else {
            return false;
        }
    }
}

uint32_t KON_HashMapHashSize(KonHashMap* hashmap)
{
    return hashmap->HashSize;
}

uint32_t KON_HashMapItemSize(KonHashMap* hashmap)
{
    return hashmap->ItemSize;
}


KonHashMapIter* KON_HashMapIterHead(KonHashMap* hashmap)
{
    KonHashMapEntry** buckets = hashmap->BucketVector;
    uint32_t size = hashmap->HashSize;
    uint32_t index = 0;

    do {
        KonHashMapEntry* firstEntry = *(buckets + index);
        if (firstEntry) {
            KonHashMapIter* iter = (KonHashMapIter*)malloc(sizeof(KonHashMapIter));
            iter->HashIndex = index;
            iter->EntryCursor = firstEntry;
            return iter;
        }
        index += 1;
    } while (index < size);
    return NULL;
}

KonHashMapIter* KON_HashMapIterTail(KonHashMap* hashmap)
{
    // TODO
    return NULL;
}

char* KON_HashMapIterItemKey(KonHashMap* hashmap, KonHashMapIter* iter)
{
    return iter->EntryCursor->Key;
}

void* KON_HashMapIterItemValue(KonHashMap* hashmap, KonHashMapIter* iter)
{
    return iter->EntryCursor->Data;
}

KonHashMapIter* KON_HashMapIterNext(KonHashMap* hashmap, KonHashMapIter* iter)
{
    if (iter->EntryCursor->Next) {
        iter->EntryCursor = iter->EntryCursor->Next;
    }
    else {
        // find next bucket
        KonHashMapEntry** buckets = hashmap->BucketVector;
        uint32_t size = hashmap->HashSize;
        uint32_t index = iter->HashIndex + 1;
        bool hasNext = false;
        while (index < size) {

            KonHashMapEntry* nextBucket = *(buckets + index);
            
            if (nextBucket) {
                hasNext = true;
                iter->HashIndex = index;
                iter->EntryCursor = nextBucket;
                break;
            }
            index += 1;
        }
        if (!hasNext) {
            return NULL;
        }
    }

    
    return iter;
}

bool KON_HashMapIterHasNext(KonHashMap* hashmap, KonHashMapIter* iter)
{
    if (iter->EntryCursor->Next) {
        return true;
    }
    else {
        // find next bucket
        KonHashMapEntry** buckets = hashmap->BucketVector;
        uint32_t size = hashmap->HashSize;
        uint32_t index = iter->HashIndex + 1;
        bool hasNext = false;
        do {

            KonHashMapEntry* nextBucket = *(buckets + index);
            
            if (nextBucket) {
                hasNext = true;
                break;
            }
            index += 1;
        } while (index < size);
        return hasNext;
    }
}

bool KON_HashMapIterEqual(KonHashMapIter* left, KonHashMapIter* right)
{
    if (left->HashIndex == right->HashIndex
        && left->EntryCursor == right->EntryCursor
    ) {
        return true;
    }
    else {
        return false;
    }
}

void KON_HashMapIterDestroy(KonHashMapIter* iter)
{
    free(iter);
}

void KON_HashMapDumpKeys(KonHashMap* hashMap)
{
    KonHashMapIter* iter = KON_HashMapIterHead(hashMap);
    printf("HashMapKeys: ");
    while (iter) {
        char* key = KON_HashMapIterItemKey(hashMap, iter);
        printf("%s ", key);

        iter = KON_HashMapIterNext(hashMap, iter);
    }
    printf("\n");
    KON_HashMapIterDestroy(iter);
}
