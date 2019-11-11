#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "../greatest.h"
#include "core/kana.h"

Kana* kana;
SUITE(suite);

TEST Reader_Tokens(void) {
    char* filePathOrigin = "~/lang/kana/kana_nunbox/examples/token/instant.kon";

    KonReader* reader = KSON_ReaderInit(kana);
    if (!reader) {
        KN_DEBUG("KN_EvalFile init failed");
        exit(1);
    }

    KN result = KN_UNDEF;

    bool openRes = KSON_ReaderFromFile(reader, filePathOrigin);
    if (openRes) {
        KN root = KSON_Parse(reader);
        KN_DEBUG("eval sentences success");
        KN formated = KN_ToFormatString(kana, root, true, 0, "  ");
        KN_DEBUG("%s", KN_StringToCstr(formated));
    }
    else {
        KN_DEBUG("open stream failed");
    }
    KSON_ReaderCloseStream(reader);
    // 释放读取器 
    KSON_ReaderExit(reader);
    

    PASS();
}

TEST Reader_Cell(void) {
    char* filePathOrigin = "~/lang/kana/kana.c/examples/kon/cell.kon";

    KonReader* reader = KSON_ReaderInit(kana);
    if (!reader) {
        KN_DEBUG("KN_EvalFile init failed");
        exit(1);
    }

    KN result = KN_UNDEF;

    bool openRes = KSON_ReaderFromFile(reader, filePathOrigin);
    if (openRes) {
        KN root = KSON_Parse(reader);
        KN_DEBUG("eval sentences success");
        KN formated = KN_ToFormatString(kana, root, true, 0, "  ");
        KN_DEBUG("%s", KN_StringToCstr(formated));
    }
    else {
        KN_DEBUG("open stream failed");
    }
    KSON_ReaderCloseStream(reader);
    // 释放读取器 
    KSON_ReaderExit(reader);
    

    PASS();
}

TEST Reader_Map(void) {
    char* filePathOrigin = "~/lang/kana/kana.c/examples/kon/map.kon";
    
    KonReader* reader = KSON_ReaderInit(kana);
    if (!reader) {
        KN_DEBUG("KN_EvalFile init failed");
        exit(1);
    }

    KN result = KN_UNDEF;

    bool openRes = KSON_ReaderFromFile(reader, filePathOrigin);
    if (openRes) {
        KN root = KSON_Parse(reader);
        KN_DEBUG("eval sentences success");
        KN formated = KN_ToFormatString(kana, root, true, 0, "  ");
        KN_DEBUG("%s", KN_StringToCstr(formated));
    }
    else {
        KN_DEBUG("open stream failed");
    }
    KSON_ReaderCloseStream(reader);
    // 释放读取器 
    KSON_ReaderExit(reader);
    

    PASS();
}

SUITE(suite) {
    RUN_TEST(Reader_Tokens);
    // RUN_TEST(Reader_Cell);
    // RUN_TEST(Reader_Map);


}

GREATEST_MAIN_DEFS();

int main(int argc, char const* argv[])
{
    GREATEST_MAIN_BEGIN();
    greatest_set_verbosity(1);

    kana = KN_Init();
    RUN_SUITE(suite);

    KN_Finish(kana);

    GREATEST_MAIN_END();

    return 0;
}
