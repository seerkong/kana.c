#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <tbox/tbox.h>

#include "prefix.h"
#include "kson/reader.h"
#include "utils/number_utils.h"
#include "interpreter/cps_interpreter.h"

int ENABLE_DEBUG = 1;

KonState* KON_Init()
{
    KonState* kstate = (KonState*)calloc(1, sizeof(KonState));
    if (kstate == NULL) {
        return NULL;
    }
    kstate->Base.Tag = KON_T_STATE;

    kstate->LastMsgDispatcherId = 1000;

    // init root env
    // KN env = KON_MakeRootEnv(kstate);
    // KON_DEBUG("root env addr %x", env);     
    // kstate->Value.Context.RootEnv = env;
    // if (!tb_init(tb_null, tb_null)) {
    //     return NULL;
    // }
    return kstate;
}


int KON_Finish(KonState* kstate)
{
    // exit tbox
    // tb_exit();
    return 0;
}

KN KON_EvalFile(KonState* kstate, char* filePath)
{
    KON_DEBUG("KON_EvalFile enter");
    
    // 初始化流
    tb_stream_ref_t istream = tb_stream_init_from_url(filePath);
    // tb_stream_ref_t istream = tb_stream_init_from_file("../../../../samples/kon/array.kon", TB_FILE_MODE_RO);

    KonReader* reader = KSON_ReaderInit(&kstate);
    if (!reader) {
        KON_DEBUG("KON_EvalFile init failed");
        exit(1);
    }

    KN result = KON_NULL;

    if (istream) {
        bool openRes = KSON_ReaderOpenStream(reader, istream, false);
        if (openRes) {
            KN root = KSON_Parse(reader);
            if (KON_IsPairList(root)) {

                KN env = KON_MakeRootEnv(kstate);

                // DefineReservedDispatcher(kstate, env);

                // KN result = KON_ProcessSentences(kstate, root, kstate->Value.Context.RootEnv);
                result = KON_ProcessSentences(kstate, root, env);
                
                
                KON_DEBUG("eval sentences success");
                KN formated = KON_ToFormatString(kstate, result, true, 0, "  ");
                //  KN formated = KON_ToFormatString(&kstate, root, false, 0, " ");
                KON_DEBUG("%s", KON_StringToCstr(formated));
            }
            
        }
        else {
            KON_DEBUG("open stream failed");
        }
        KSON_ReaderCloseStream(reader);
        // 释放读取器 
        KSON_ReaderExit(reader);
        // KSON_ReaderOpen的最后一个参数，如果是false，需要主动释放流
        tb_stream_exit(istream);
    }

    return result;
}
