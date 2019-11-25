#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "../greatest.h"
#include "core/kana.h"

SUITE(test_hashtable);

TEST Table_NewDelete(void) {
    KnHashTable* table = KnHashTable_Init(4);
    // KnHashTable_Destroy(table);
    ASSERT_EQ_FMT(1, (int)KnHashTable_Destroy(table), "%d");
    ASSERT_EQ_FMT(-1, (int)KnHashTable_Destroy(NULL), "%d");
    PASS();
}


TEST Table_HasKey(void) {
    KnHashTable* table = KnHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";

    KnHashTable_PutKv(table, "writeln", value1);
    KnHashTable_PutKv(table, "a", value2);
    ASSERT_EQ_FMT(true, KnHashTable_HasKey(table, "writeln"), "%d");
    ASSERT_EQ_FMT(false, KnHashTable_HasKey(table, "sss"), "%d");
    PASS();
}

TEST Table_KeySetKeyGet(void) {
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

    ASSERT_EQ_FMT(value1, KnHashTable_AtKey(table, "abc"), "%s");
    ASSERT_EQ_FMT(value2, KnHashTable_AtKey(table, "bcd"), "%s");
    ASSERT_EQ_FMT(value3, KnHashTable_AtKey(table, "cde"), "%s");
    ASSERT_EQ_FMT(value4, KnHashTable_AtKey(table, "efg"), "%s");

    ASSERT_EQm("destroy ptr", KnHashTable_Destroy(table), 1);
    PASS();
}



TEST Table_MoreKeySetKeyGet(void) {
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

    ASSERT_EQ_FMT(value1, KnHashTable_AtKey(table, "abc"), "%s");
    ASSERT_EQ_FMT(value2, KnHashTable_AtKey(table, "bcd"), "%s");
    ASSERT_EQ_FMT(value3, KnHashTable_AtKey(table, "cde"), "%s");
    ASSERT_EQ_FMT(value4, KnHashTable_AtKey(table, "efg"), "%s");

    ASSERT_EQm("destroy ptr", KnHashTable_Destroy(table), 1);
    PASS();
}


TEST Table_FirstLast(void) {
    KnHashTable* table = KnHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";

    KnHashTable_PutKv(table, "abc", value1);
    ASSERT_EQ_FMT(value1, KnHashTable_FirstVal(table), "%s");
    ASSERT_EQ_FMT(value1, KnHashTable_LastVal(table), "%s");
    KnHashTable_PutKv(table, "bcd", value2);
    ASSERT_EQ_FMT(value1, KnHashTable_FirstVal(table), "%s");
    ASSERT_EQ_FMT(value2, KnHashTable_LastVal(table), "%s");
    PASS();
}

TEST Table_PushVal_IndexGet(void) {
    KnHashTable* table = KnHashTable_Init(4);

    for (int i = 0; i < 5; i++) {
        KnHashTable_PushVal(table, (ktable_val_t)i);
    }

    ASSERT_EQ_FMT(5, KnHashTable_Length(table), "%d");

    for (int i = 0; i < 5; i++) {
        ASSERT_EQ_FMT(i, (int)KnHashTable_ValAtIndex(table, i), "%d");
    }

    ASSERT_EQm("destroy ptr", KnHashTable_Destroy(table), 1);
    PASS();
}

TEST Table_PutKv_IndexGet(void) {
    KnHashTable* table = KnHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";

    KnHashTable_PutKv(table, "abc", (ktable_val_t)value1);
    KnHashTable_PutKv(table, "bcd", (ktable_val_t)value2);

    KnHashTable_PushVal(table, value3);

    ASSERT_EQ_FMT(value1, KnHashTable_AtKey(table, "abc"), "%s");
    ASSERT_EQ_FMT(value1, KnHashTable_ValAtIndex(table, 0), "%s");
    ASSERT_EQ_FMT(value2, KnHashTable_ValAtIndex(table, 1), "%s");
    ASSERT_EQ_FMT(value3, KnHashTable_ValAtIndex(table, 2), "%s");

    ASSERT_EQm("destroy ptr", KnHashTable_Destroy(table), 1);
    PASS();
}

TEST Table_PushKv_IndexGet(void) {
    KnHashTable* table = KnHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";

    KnHashTable_PushKv(table, "abc", (ktable_val_t)value1);
    KnHashTable_PushKv(table, "bcd", (ktable_val_t)value2);
    KnHashTable_PushKv(table, "abc", (ktable_val_t)value3);

    KnHashTable_PushVal(table, value3);

    ASSERT_EQ_FMT(value3, KnHashTable_AtKey(table, "abc"), "%s");

    ASSERT_EQ_FMT(value1, KnHashTable_ValAtIndex(table, 0), "%s");
    ASSERT_EQ_FMT(value2, KnHashTable_ValAtIndex(table, 1), "%s");
    ASSERT_EQ_FMT(value3, KnHashTable_ValAtIndex(table, 2), "%s");
    ASSERT_EQ_FMT(value3, KnHashTable_ValAtIndex(table, 3), "%s");

    ASSERT_EQm("destroy ptr", KnHashTable_Destroy(table), 1);
    PASS();
}


TEST Table_UpdateKeyVal(void) {
    KnHashTable* table = KnHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";
    char* value4 = "stdint.h";

    KnHashTable_PushKv(table, "abc", value1);
    KnHashTable_PushKv(table, "efg", value2);

    ASSERT_EQ_FMT(value1, KnHashTable_AtKey(table, "abc"), "%s");

    KnHashTable_SetKeyAtIndex(table, 0, "bcd");
    
    ASSERT_EQ_FMT(value1, KnHashTable_AtKey(table, "bcd"), "%s");
    ASSERT_STR_EQ("bcd", KnHashTable_KeyAtIndex(table, 0));

    KnHashTable_SetValAtIndex(table, 1, value3);
    ASSERT_EQ_FMT(value3, KnHashTable_ValAtIndex(table, 1), "%s");

    KnHashTable_SetKvAtIndex(table, 1, "hij", value4);
    ASSERT_EQ_FMT(value4, KnHashTable_ValAtIndex(table, 1), "%s");
    ASSERT_STR_EQ("hij", KnHashTable_KeyAtIndex(table, 1));


    ASSERT_EQm("destroy ptr", KnHashTable_Destroy(table), 1);
    PASS();
}


TEST Table_DelByKey(void) {
    KnHashTable* table = KnHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";

    KnHashTable_PushKv(table, "abc", value1);
    KnHashTable_PushKv(table, "efg", value2);

    KnHashTable_DelByKey(table, "abc");
    ASSERT_EQ_FMT(KN_HASH_TABLE_UNDEF, KnHashTable_AtKey(table, "abc"), "%x");
    ASSERT_EQ_FMT(value2, KnHashTable_ValAtIndex(table, 0), "%s");
    PASS();
}

TEST Table_DelByIndex(void) {
    KnHashTable* table = KnHashTable_Init(4);

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";

    KnHashTable_PushKv(table, "abc", value1);
    KnHashTable_PushKv(table, "efg", value2);

    KnHashTable_DelByIndex(table, 0);
    ASSERT_EQ_FMT(KN_HASH_TABLE_UNDEF, KnHashTable_AtKey(table, "abc"), "%x");
    ASSERT_EQ_FMT(value2, KnHashTable_ValAtIndex(table, 0), "%s");
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
    // TODO KnHashTable_UnshiftVal KnHashTable_UnshiftKv

    RUN_TEST(Table_PutKv_IndexGet);
    // KnHashTable_SetKeyAtIndex KnHashTable_SetValAtIndex KnHashTable_SetKvAtIndex
    RUN_TEST(Table_UpdateKeyVal);

    RUN_TEST(Table_DelByKey);

    RUN_TEST(Table_DelByIndex);

}
