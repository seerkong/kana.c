#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "../greatest.h"
#include "core/kana.h"

SUITE(test_list);

TEST List_NewDelete(void) {
    KnList* list = KnList_Init();

    ASSERT_EQ_FMT(1, (int)KnList_Destroy(list), "%d");
    ASSERT_EQ_FMT(-1, (int)KnList_Destroy(NULL), "%d");
    PASS();
}


TEST List_Push(void) {
    KnList* list = KnList_Init();

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";
    char* value4 = "stdint.h";

    KnList_Push(list, value1);
    KnList_Push(list, value2);

    ASSERT_STR_EQ(value2, KnList_Tail(list));
    KnList_Destroy(list);
    PASS();
}

TEST List_Pop(void) {
    KnList* list = KnList_Init();

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";
    char* value4 = "stdint.h";

    KnList_Push(list, value1);
    KnList_Push(list, value2);
    char* poped = (char*)KnList_Pop(list);
    ASSERT_STR_EQ(value2, poped);
    ASSERT_STR_EQ(value1, KnList_Tail(list));
    KnList_Destroy(list);
    PASS();
}


TEST List_Unshift(void) {
    KnList* list = KnList_Init();

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";
    char* value4 = "stdint.h";

    KnList_Unshift(list, value1);
    KnList_Unshift(list, value2);

    ASSERT_STR_EQ(value2, KnList_Head(list));
    KnList_Destroy(list);
    PASS();
}

TEST List_Shift(void) {
    KnList* list = KnList_Init();

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";
    char* value4 = "stdint.h";

    KnList_Unshift(list, value1);
    KnList_Unshift(list, value2);
    char* poped = (char*)KnList_Shift(list);
    ASSERT_STR_EQ(value2, poped);
    ASSERT_STR_EQ(value1, KnList_Head(list));
    KnList_Destroy(list);
    PASS();
}

SUITE(test_list) {
    RUN_TEST(List_NewDelete);
    RUN_TEST(List_Push);
    RUN_TEST(List_Pop);
    RUN_TEST(List_Unshift);
    RUN_TEST(List_Shift);
}



GREATEST_MAIN_DEFS();

int main(int argc, char const* argv[])
{
    if (!tb_init(tb_null, tb_null)) {
        exit(1);
    }

    GREATEST_MAIN_BEGIN();
    greatest_set_verbosity(1);
    
    RUN_SUITE(test_list);

    GREATEST_MAIN_END();
    tb_exit();
}
