#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "../greatest.h"
#include "../../src/kon/kon.h"
#include <tbox/tbox.h>

KonState* kstate;
SUITE(suite);

TEST Reader_Cell(void) {
    
    // 初始化流
    tb_stream_ref_t istream = tb_stream_init_from_url("~/lang/konscript/kon-c/samples/kon/cell.kon");

    KonReader* reader = KSON_ReaderInit(&kstate);
    if (!reader) {
        printf("reader init failed\n");
        exit(1);
    }
    printf("istream addr %x\n", istream);
    if (istream) {
        bool openRes = KSON_ReaderOpenStream(reader, istream, false);
        if (openRes) {
            printf("open stream success\n");
            KN root = KSON_Parse(reader);
            // TODO 打印为字符串
            printf("parse success\n");
            KN formated = KON_ToFormatString(&kstate, root, true, 0, "  ");
            //  KN formated = KON_ToFormatString(&kstate, root, false, 0, " ");
            printf("%s\n", KON_StringToCstr(formated));
        }
        else {
            printf("open stream failed\n");
        }
        KSON_ReaderCloseStream(reader);
        // 释放读取器 
        KSON_ReaderExit(reader);
        // KSON_ReaderOpen的最后一个参数，如果是false，需要主动释放流
        tb_stream_exit(istream);
    }

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
    if (!tb_init(tb_null, tb_null)) {
        return NULL;
    }
    kstate = KON_Init();
    RUN_SUITE(suite);

    KON_Finish(kstate);
    tb_exit();
    GREATEST_MAIN_END();
}
