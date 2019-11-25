#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>

#include <assert.h>
#include "../greatest.h"
#include "core/kana.h"

SUITE(test_hashtable_iter);



TEST Table_IterNext(void) {
    KnHashTable* table = KnHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stddef.h";
    char* value4 = "sys/time.h";
    char* value5 = "string.h";
    char* value6 = "stdint.h";
    char* value7 = "kon_hashmap.h";

    KnHashTable_PutKv(table, "abc", value1);
    KnHashTable_PutKv(table, "bcd", value2);
    KnHashTable_PutKv(table, "cde", value3);
    KnHashTable_PutKv(table, "efg", value4);
    KnHashTable_PutKv(table, "hi", value5);
    KnHashTable_PutKv(table, "jklmn", value6);
    KnHashTable_PutKv(table, "opqrstuvwxyz", value7);

    KnHashTableIter iter = KnHashTable_IterHead(table);
    while (iter != KN_HASH_TABLE_NIL) {
        KnHashTableIter next = KnHashTable_IterNext(table, iter);
        const char* key = KnHashTable_IterGetKey(table, iter);
        char* val = (char*)KnHashTable_IterGetVal(table, iter);
        printf("iter next , key %s, val %s\n", key, val);
        iter = next;
    }
    ASSERT_EQm("destroy ptr", KnHashTable_Destroy(table), 1);
    PASS();
}



TEST Table_IterPrev(void) {
    KnHashTable* table = KnHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stddef.h";
    char* value4 = "sys/time.h";
    char* value5 = "string.h";
    char* value6 = "stdint.h";
    char* value7 = "kon_hashmap.h";

    KnHashTable_PutKv(table, "abc", value1);
    KnHashTable_PutKv(table, "bcd", value2);
    KnHashTable_PutKv(table, "cde", value3);
    KnHashTable_PutKv(table, "efg", value4);
    KnHashTable_PutKv(table, "hi", value5);
    KnHashTable_PutKv(table, "jklmn", value6);
    KnHashTable_PutKv(table, "opqrstuvwxyz", value7);

    KnHashTableIter iter = KnHashTable_IterTail(table);
    while (iter != KN_HASH_TABLE_NIL) {
        KnHashTableIter next = KnHashTable_IterPrev(table, iter);
        const char* key = KnHashTable_IterGetKey(table, iter);
        char* val = (char*)KnHashTable_IterGetVal(table, iter);
        printf("iter next , key %s, val %s\n", key, val);
        iter = next;
    }

    ASSERT_EQm("destroy ptr", KnHashTable_Destroy(table), 1);
    PASS();
}


SUITE(test_hashtable_iter) {
    RUN_TEST(Table_IterNext);
    RUN_TEST(Table_IterPrev);


}
