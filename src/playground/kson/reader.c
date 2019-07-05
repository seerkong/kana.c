#include <stdio.h>
#include <stdlib.h>
#include "../../kon/kon.h"
// #include "./log.h"
#include <tbox/tbox.h>

int main(int argc, char const* argv[])
{
    Kon kstate;
    KON_Init(&kstate);
    // if (!KON_Init(&kstate)) {
    //     fprintf(stderr, "Could not initialize!\n");

    //     return 1;
    // }

    if (!tb_init(tb_null, tb_null)) {
        return 1;
    }
    // 初始化流
    tb_stream_ref_t istream = tb_stream_init_from_url("~/lang/konscript/kon-c/samples/kon/cell.kon");
//    tb_stream_ref_t istream = tb_stream_init_from_file("../../../../samples/kon/array.kon", TB_FILE_MODE_RO);

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
            Kon* root = KSON_Parse(reader);
            // TODO 打印为字符串
            printf("parse success\n");
            Kon* formated = KON_ToFormatString(&kstate, root, true, 0, "  ");
            //  Kon* formated = KON_ToFormatString(&kstate, root, false, 0, " ");
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

     tb_exit();
//    KON_Finish(&kstate);
    return 0;
}
