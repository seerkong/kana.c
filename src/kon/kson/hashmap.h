#ifndef KON_HASHMAP_H
#define KON_HASHMAP_H 1
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
typedef struct _KonHashMap KonHashMap;
typedef struct _KonHashMapEntry KonHashMapEntry;
typedef struct _KonHashMapIter KonHashMapIter;

struct _KonHashMap {
    uint32_t HashSize;  // hash vector size
    uint32_t ItemSize;  // how many items stored
    KonHashMapEntry** BucketVector;    // an array of KonHashMapEntry pointers
};

struct _KonHashMapEntry {
    KonHashMapEntry* Prev;
    KonHashMapEntry* Next;
    uint32_t HashCode;
    uint32_t CrcCode;
    uint32_t KeyLen;
    
    char* Key;
    void* Data;
};

struct _KonHashMapIter {
    uint32_t HashIndex;
    KonHashMapEntry* EntryCursor;
};

KonHashMap* KON_HashMapInit(uint32_t size);
void KON_HashMapDestroy(KonHashMap* hashmap);
void KON_HashMapClear(KonHashMap* hashmap);
void KON_HashMapPut(KonHashMap* hashmap, char* key, void* value);
void* KON_HashMapGet(KonHashMap* hashmap, char* key);
bool KON_HashMapHasKey(KonHashMap* hashmap, char* key);
void KON_HashMapDel(KonHashMap* hashmap, char* key);
uint32_t KON_HashMapHashSize(KonHashMap* hashmap);
uint32_t KON_HashMapItemSize(KonHashMap* hashmap);
// uint32_t KON_HashMapHashResize(KonHashMap* hashmap);

KonHashMapIter* KON_HashMapIterHead(KonHashMap* hashmap);
KonHashMapIter* KON_HashMapIterTail(KonHashMap* hashmap);
char* KON_HashMapIterItemKey(KonHashMap* hashmap, KonHashMapIter* iter);
void* KON_HashMapIterItemValue(KonHashMap* hashmap, KonHashMapIter* iter);
KonHashMapIter* KON_HashMapIterNext(KonHashMap* hashmap, KonHashMapIter* iter);
bool KON_HashMapIterHasNext(KonHashMap* hashmap, KonHashMapIter* iter);
bool KON_HashMapIterEqual(KonHashMapIter* left, KonHashMapIter* right);
void KON_HashMapIterDestroy(KonHashMapIter* iter);

// for debug
void KON_HashMapDumpKeys(KonHashMap* hashMap);

#ifdef __cplusplus
}
#endif

#endif