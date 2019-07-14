#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <tbox/tbox.h>

#include "prefix.h"
#include "kson/reader.h"
#include "utils/number_utils.h"
#include "interpreter/cps_interpreter.h"



int KON_Init(KonState* kstate)
{
    kstate->Base.Tag = KON_T_STATE;

    // init root env
    // KN env = KON_MakeRootEnv(kstate);
    // kon_debug("root env addr %x", env);     
    // kstate->Value.Context.RootEnv = env;
    // if (!tb_init(tb_null, tb_null)) {
    //     return 1;
    // }
    return 0;
}


int KON_Finish(KonState* kstate)
{
    // exit tbox
    // tb_exit();
    return 0;
}

KN KON_EvalFile(KonState* kstate, char* filePath)
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
            KN root = KSON_Parse(reader);
            if (KON_IsList(root)) {

                KN env = KON_MakeRootEnv(kstate);

                // KN result = KON_ProcessSentences(kstate, root, kstate->Value.Context.RootEnv);
                KN result = KON_ProcessSentences(kstate, root, env);
                
                
                printf("eval sentences success\n");
                KN formated = KON_ToFormatString(kstate, result, true, 0, "  ");
                //  KN formated = KON_ToFormatString(&kstate, root, false, 0, " ");
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
