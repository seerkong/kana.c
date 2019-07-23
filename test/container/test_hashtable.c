#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include "../../src/kon/container/hashtable/kx_hashtable.h"
#include <assert.h>
#include "../greatest.h"

SUITE(test_hashtable);

TEST Table_NewDelete(void) {
    KxHashTable* table = KxHashTable_Init(4);
    // KxHashTable_Destroy(table);
    ASSERT_EQ_FMT(1, (int)KxHashTable_Destroy(table), "%d");
    ASSERT_EQ_FMT(-1, (int)KxHashTable_Destroy(NULL), "%d");
    PASS();
}


TEST Table_HasKey(void) {
    KxHashTable* table = KxHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";

    KxHashTable_PutKv(table, "writeln", value1);
    KxHashTable_PutKv(table, "a", value2);
    ASSERT_EQ_FMT(true, KxHashTable_HasKey(table, "writeln"), "%d");
    ASSERT_EQ_FMT(false, KxHashTable_HasKey(table, "sss"), "%d");
    PASS();
}

TEST Table_KeySetKeyGet(void) {
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

    ASSERT_EQ_FMT(value1, KxHashTable_AtKey(table, "abc"), "%s");
    ASSERT_EQ_FMT(value2, KxHashTable_AtKey(table, "bcd"), "%s");
    ASSERT_EQ_FMT(value3, KxHashTable_AtKey(table, "cde"), "%s");
    ASSERT_EQ_FMT(value4, KxHashTable_AtKey(table, "efg"), "%s");

    ASSERT_EQm("destroy ptr", KxHashTable_Destroy(table), 1);
    PASS();
}



TEST Table_MoreKeySetKeyGet(void) {
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

    ASSERT_EQ_FMT(value1, KxHashTable_AtKey(table, "abc"), "%s");
    ASSERT_EQ_FMT(value2, KxHashTable_AtKey(table, "bcd"), "%s");
    ASSERT_EQ_FMT(value3, KxHashTable_AtKey(table, "cde"), "%s");
    ASSERT_EQ_FMT(value4, KxHashTable_AtKey(table, "efg"), "%s");

    ASSERT_EQm("destroy ptr", KxHashTable_Destroy(table), 1);
    PASS();
}


TEST Table_FirstLast(void) {
    KxHashTable* table = KxHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";

    KxHashTable_PutKv(table, "abc", value1);
    ASSERT_EQ_FMT(value1, KxHashTable_FirstVal(table), "%s");
    ASSERT_EQ_FMT(value1, KxHashTable_LastVal(table), "%s");
    KxHashTable_PutKv(table, "bcd", value2);
    ASSERT_EQ_FMT(value1, KxHashTable_FirstVal(table), "%s");
    ASSERT_EQ_FMT(value2, KxHashTable_LastVal(table), "%s");
    PASS();
}

TEST Table_PushVal_IndexGet(void) {
    KxHashTable* table = KxHashTable_Init(4);

    for (int i = 0; i < 5; i++) {
        KxHashTable_PushVal(table, (XN)i);
    }

    ASSERT_EQ_FMT(5, KxHashTable_Length(table), "%d");

    for (int i = 0; i < 5; i++) {
        ASSERT_EQ_FMT(i, (int)KxHashTable_ValAtIndex(table, i), "%d");
    }

    ASSERT_EQm("destroy ptr", KxHashTable_Destroy(table), 1);
    PASS();
}

TEST Table_PutKv_IndexGet(void) {
    KxHashTable* table = KxHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";

    KxHashTable_PutKv(table, "abc", value1);
    KxHashTable_PutKv(table, "bcd", value2);

    KxHashTable_PushVal(table, value3);

    ASSERT_EQ_FMT(value1, KxHashTable_AtKey(table, "abc"), "%s");
    ASSERT_EQ_FMT(value1, KxHashTable_ValAtIndex(table, 0), "%s");
    ASSERT_EQ_FMT(value2, KxHashTable_ValAtIndex(table, 1), "%s");
    ASSERT_EQ_FMT(value3, KxHashTable_ValAtIndex(table, 2), "%s");

    ASSERT_EQm("destroy ptr", KxHashTable_Destroy(table), 1);
    PASS();
}

TEST Table_PushKv_IndexGet(void) {
    KxHashTable* table = KxHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";

    KxHashTable_PushKv(table, "abc", value1);
    KxHashTable_PushKv(table, "bcd", value2);
    KxHashTable_PushKv(table, "abc", value3);

    KxHashTable_PushVal(table, value3);

    ASSERT_EQ_FMT(value3, KxHashTable_AtKey(table, "abc"), "%s");

    ASSERT_EQ_FMT(value1, KxHashTable_ValAtIndex(table, 0), "%s");
    ASSERT_EQ_FMT(value2, KxHashTable_ValAtIndex(table, 1), "%s");
    ASSERT_EQ_FMT(value3, KxHashTable_ValAtIndex(table, 2), "%s");
    ASSERT_EQ_FMT(value3, KxHashTable_ValAtIndex(table, 3), "%s");

    ASSERT_EQm("destroy ptr", KxHashTable_Destroy(table), 1);
    PASS();
}


TEST Table_UpdateKeyVal(void) {
    KxHashTable* table = KxHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";
    char* value4 = "stdint.h";

    KxHashTable_PushKv(table, "abc", value1);
    KxHashTable_PushKv(table, "efg", value2);

    ASSERT_EQ_FMT(value1, KxHashTable_AtKey(table, "abc"), "%s");

    KxHashTable_SetKeyAtIndex(table, 0, "bcd");
    
    ASSERT_EQ_FMT(value1, KxHashTable_AtKey(table, "bcd"), "%s");
    ASSERT_STR_EQ("bcd", KxHashTable_KeyAtIndex(table, 0));

    KxHashTable_SetValAtIndex(table, 1, value3);
    ASSERT_EQ_FMT(value3, KxHashTable_ValAtIndex(table, 1), "%s");

    KxHashTable_SetKvAtIndex(table, 1, "hij", value4);
    ASSERT_EQ_FMT(value4, KxHashTable_ValAtIndex(table, 1), "%s");
    ASSERT_STR_EQ("hij", KxHashTable_KeyAtIndex(table, 1));


    ASSERT_EQm("destroy ptr", KxHashTable_Destroy(table), 1);
    PASS();
}


TEST Table_DelByKey(void) {
    KxHashTable* table = KxHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";

    KxHashTable_PushKv(table, "abc", value1);
    KxHashTable_PushKv(table, "efg", value2);

    KxHashTable_DelByKey(table, "abc");
    ASSERT_EQ_FMT(KX_HASH_TABLE_NULL, KxHashTable_AtKey(table, "abc"), "%x");
    ASSERT_EQ_FMT(value2, KxHashTable_ValAtIndex(table, 0), "%s");
    PASS();
}

TEST Table_DelByIndex(void) {
    KxHashTable* table = KxHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";

    KxHashTable_PushKv(table, "abc", value1);
    KxHashTable_PushKv(table, "efg", value2);

    KxHashTable_DelByIndex(table, 0);
    ASSERT_EQ_FMT(KX_HASH_TABLE_NULL, KxHashTable_AtKey(table, "abc"), "%x");
    ASSERT_EQ_FMT(value2, KxHashTable_ValAtIndex(table, 0), "%s");
    PASS();
}


SUITE(test_hashtable) {
    RUN_TEST(Table_NewDelete);
    RUN_TEST(Table_HasKey);
    
    RUN_TEST(Table_KeySetKeyGet);
    RUN_TEST(Table_MoreKeySetKeyGet);
    
    RUN_TEST(Table_FirstLast);

    RUN_TEST(Table_PushVal_IndexGet);
    
    RUN_TEST(Table_PushKv_IndexGet);
    // TODO KxHashTable_UnshiftVal KxHashTable_UnshiftKv

    RUN_TEST(Table_PutKv_IndexGet);
    // KxHashTable_SetKeyAtIndex KxHashTable_SetValAtIndex KxHashTable_SetKvAtIndex
    RUN_TEST(Table_UpdateKeyVal);

    RUN_TEST(Table_DelByKey);

    RUN_TEST(Table_DelByIndex);

}
