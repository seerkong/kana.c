#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include "../../core/container/kx_hashtable.h"
#include <assert.h>
#include "../greatest.h"

SUITE(test_hashtable_iter);



TEST Table_IterNext(void) {
    KxHashTable* table = KxHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stddef.h";
    char* value4 = "sys/time.h";
    char* value5 = "string.h";
    char* value6 = "stdint.h";
    char* value7 = "kon_hashmap.h";

    KxHashTable_PutKv(table, "abc", value1);
    KxHashTable_PutKv(table, "bcd", value2);
    KxHashTable_PutKv(table, "cde", value3);
    KxHashTable_PutKv(table, "efg", value4);
    KxHashTable_PutKv(table, "hi", value5);
    KxHashTable_PutKv(table, "jklmn", value6);
    KxHashTable_PutKv(table, "opqrstuvwxyz", value7);

    KxHashTableIter iter = KxHashTable_IterHead(table);
    while (iter != KX_HASH_TABLE_NIL) {
        KxHashTableIter next = KxHashTable_IterNext(table, iter);
        const char* key = KxHashTable_IterGetKey(table, iter);
        char* val = (char*)KxHashTable_IterGetVal(table, iter);
        printf("iter next , key %s, val %s\n", key, val);
        iter = next;
    }
    ASSERT_EQm("destroy ptr", KxHashTable_Destroy(table), 1);
    PASS();
}



TEST Table_IterPrev(void) {
    KxHashTable* table = KxHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stddef.h";
    char* value4 = "sys/time.h";
    char* value5 = "string.h";
    char* value6 = "stdint.h";
    char* value7 = "kon_hashmap.h";

    KxHashTable_PutKv(table, "abc", value1);
    KxHashTable_PutKv(table, "bcd", value2);
    KxHashTable_PutKv(table, "cde", value3);
    KxHashTable_PutKv(table, "efg", value4);
    KxHashTable_PutKv(table, "hi", value5);
    KxHashTable_PutKv(table, "jklmn", value6);
    KxHashTable_PutKv(table, "opqrstuvwxyz", value7);

    KxHashTableIter iter = KxHashTable_IterTail(table);
    while (iter != KX_HASH_TABLE_NIL) {
        KxHashTableIter next = KxHashTable_IterPrev(table, iter);
        const char* key = KxHashTable_IterGetKey(table, iter);
        char* val = (char*)KxHashTable_IterGetVal(table, iter);
        printf("iter next , key %s, val %s\n", key, val);
        iter = next;
    }

    ASSERT_EQm("destroy ptr", KxHashTable_Destroy(table), 1);
    PASS();
}


SUITE(test_hashtable_iter) {
    RUN_TEST(Table_IterNext);
    RUN_TEST(Table_IterPrev);


}
