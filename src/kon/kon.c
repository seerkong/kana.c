#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <tbox/tbox.h>

#include "prefix.h"
#include "kson/reader.h"
#include "utils/number_utils.h"
#include "interpreter/cps_interpreter.h"



int KON_Init(Kon* kstate)
{
    kon_debug("offsetof(struct KonStruct, value) %lu", offsetof(struct KonStruct, Value));
    // if (!tb_init(tb_null, tb_null)) {
    //     return 1;
    // }
    return 0;
}


int KON_Finish(Kon* kstate)
{
    // exit tbox
    // tb_exit();
    return 0;
}

Kon* KON_EvalFile(Kon* kstate, char* filePath)
{
    printf("KON_EvalFile enter");
    
    // 初始化流
    tb_stream_ref_t istream = tb_stream_init_from_url(filePath);
    // tb_stream_ref_t istream = tb_stream_init_from_file("../../../../samples/kon/array.kon", TB_FILE_MODE_RO);

    KonReader* reader = KSON_ReaderInit(&kstate);
    if (!reader) {
        printf("KON_EvalFile init failed\n");
        exit(1);
    }

    if (istream) {
        bool openRes = KSON_ReaderOpenStream(reader, istream, false);
        if (openRes) {
            Kon* root = KSON_Parse(reader);
            if (KON_IsList(root)) {
                Kon* result = KON_ProcessSentences(kstate, root);
                printf("eval sentences success\n");
                Kon* formated = KON_ToFormatString(kstate, result, true, 0, "  ");
                //  Kon* formated = KON_ToFormatString(&kstate, root, false, 0, " ");
                printf("%s\n", KON_StringToCstr(formated));
            }
            
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
}
