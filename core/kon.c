#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "prefix.h"
#include "kson/reader.h"
#include "utils/number_utils.h"
#include "interpreter/cps_interpreter.h"
#include "../lib/tbox/tbox.h"

int ENABLE_DEBUG = 1;

KonState* KN_Init()
{
    KonState* kstate = (KonState*)calloc(1, sizeof(KonState));
    if (kstate == NULL) {
        return NULL;
    }
    kstate->base.tag = KN_T_STATE;

    kstate->nextMsgDispatcherId = 100;

    // init root env
    // KN env = KN_MakeRootEnv(kstate);
    // KN_DEBUG("root env addr %x", env);     
    // kstate->value.Context.rootEnv = env;

    kstate->largeAllocator = tb_large_allocator_init(tb_null, 0);
    kstate->allocator = tb_default_allocator_init(kstate->largeAllocator);
    
    if (!tb_init(tb_null, kstate->allocator)) {
        printf("tb_init failed\n");
        return NULL;
    }

    KN_InitGc(kstate);

    kstate->msgDispatchers = KxVector_InitWithSize(200);

    return kstate;
}


int KN_Finish(KonState* kstate)
{
    // exit allocator
    if (kstate->allocator) {
        tb_allocator_exit(kstate->allocator);
    }
    kstate->allocator = tb_null;

    if (kstate->largeAllocator) {
        tb_allocator_exit(kstate->largeAllocator);
    }
    kstate->largeAllocator = tb_null;


    KN_DestroyGc(kstate);

    tb_exit();
    free(kstate);
    return 0;
}

KN KN_EvalFile(KonState* kstate, char* filePath)
{
    KN_DEBUG("KN_EvalFile enter");

    KonReader* reader = KSON_ReaderInit(kstate);
    if (!reader) {
        KN_DEBUG("KN_EvalFile init failed");
        exit(1);
    }

    KN result = KN_UNDEF;

    bool openRes = KSON_ReaderFromFile(reader, filePath);
    KN_DEBUG("---eval file %s\n", filePath);
    
    if (openRes) {
        KN root = KSON_Parse(reader);
        KN_ShowGcStatics(kstate);
        KSON_ReaderCloseStream(reader);
        // 释放读取器 
        KSON_ReaderExit(reader);
        if (KN_IsPairList(root)) {

            KN rootEnv = KN_MakeRootEnv(kstate);

            // DefineReservedDispatcher(kstate, env);

            // KN result = KN_ProcessSentences(kstate, root, kstate->value.Context.rootEnv);
            KN processEnv = KN_MakeChildEnv(kstate, rootEnv);
            result = KN_ProcessSentences(kstate, root, processEnv);
            
            
            KN_DEBUG("eval sentences success");
            KN formated = KN_ToFormatString(kstate, result, true, 0, "  ");
            KN_DEBUG("%s", KN_StringToCstr(formated));

            KN_ShowGcStatics(kstate);
            KN_DEBUG("---------\n");
        }
        
    }
    else {
        KN_DEBUG("open stream failed");
    }
    

    return result;
}
