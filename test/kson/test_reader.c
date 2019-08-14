#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "../greatest.h"
#include "kon/kon.h"

KonState* kstate;
SUITE(suite);

TEST Reader_Cell(void) {
    char* filePathOrigin = "~/lang/konscript/kon-c/samples/kon/cell.kon";
    
    KonReader* reader = KSON_ReaderInit(kstate);
    if (!reader) {
        KON_DEBUG("KON_EvalFile init failed");
        exit(1);
    }

    KN result = KON_UNDEF;

    bool openRes = KSON_ReaderFromFile(reader, filePathOrigin);
    if (openRes) {
        KN root = KSON_Parse(reader);
        KON_DEBUG("eval sentences success");
        KN formated = KON_ToFormatString(kstate, root, true, 0, "  ");
        KON_DEBUG("%s", KON_StringToCstr(formated));
    }
    else {
        KON_DEBUG("open stream failed");
    }
    KSON_ReaderCloseStream(reader);
    // 释放读取器 
    KSON_ReaderExit(reader);
    

    PASS();
}


SUITE(suite) {
    RUN_TEST(Reader_Cell);

}

GREATEST_MAIN_DEFS();

int main(int argc, char const* argv[])
{
    GREATEST_MAIN_BEGIN();
    greatest_set_verbosity(1);

    kstate = KON_Init();
    RUN_SUITE(suite);

    KON_Finish(kstate);

    GREATEST_MAIN_END();

    return 0;
}
