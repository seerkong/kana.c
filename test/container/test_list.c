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
    KxList* list = KxList_Init();

    ASSERT_EQ_FMT(1, (int)KxList_Destroy(list), "%d");
    ASSERT_EQ_FMT(-1, (int)KxList_Destroy(NULL), "%d");
    PASS();
}


TEST List_Push(void) {
    KxList* list = KxList_Init();

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";
    char* value4 = "stdint.h";

    KxList_Push(list, value1);
    KxList_Push(list, value2);

    ASSERT_STR_EQ(value2, KxList_Tail(list));
    KxList_Destroy(list);
    PASS();
}

TEST List_Pop(void) {
    KxList* list = KxList_Init();

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";
    char* value4 = "stdint.h";

    KxList_Push(list, value1);
    KxList_Push(list, value2);
    char* poped = (char*)KxList_Pop(list);
    ASSERT_STR_EQ(value2, poped);
    ASSERT_STR_EQ(value1, KxList_Tail(list));
    KxList_Destroy(list);
    PASS();
}


TEST List_Unshift(void) {
    KxList* list = KxList_Init();

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";
    char* value4 = "stdint.h";

    KxList_Unshift(list, value1);
    KxList_Unshift(list, value2);

    ASSERT_STR_EQ(value2, KxList_Head(list));
    KxList_Destroy(list);
    PASS();
}

TEST List_Shift(void) {
    KxList* list = KxList_Init();

    char* value1 = "stdlib.h";
    char* value2 = "stdio.h";
    char* value3 = "stdbool.h";
    char* value4 = "stdint.h";

    KxList_Unshift(list, value1);
    KxList_Unshift(list, value2);
    char* poped = (char*)KxList_Shift(list);
    ASSERT_STR_EQ(value2, poped);
    ASSERT_STR_EQ(value1, KxList_Head(list));
    KxList_Destroy(list);
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
