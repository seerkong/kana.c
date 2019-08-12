#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "prefix.h"
#include "kson/reader.h"
#include "utils/number_utils.h"
#include "interpreter/cps_interpreter.h"
#include <tbox/tbox.h>

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

    if (!tb_init(tb_null, tb_null)) {
        return NULL;
    }

    
    kstate->LargeAllocator = tb_large_allocator_init(tb_null, 0);
    kstate->DefaultAllocator = tb_default_allocator_init(kstate->LargeAllocator);
    kstate->SmallAllocator = tb_small_allocator_init(tb_null);

    return kstate;
}


int KON_Finish(KonState* kstate)
{
    // exit allocator
    if (kstate->DefaultAllocator) {
        tb_allocator_exit(kstate->DefaultAllocator);
    }
    kstate->DefaultAllocator = tb_null;

    if (kstate->LargeAllocator) {
        tb_allocator_exit(kstate->LargeAllocator);
    }
    kstate->LargeAllocator = tb_null;

    if (kstate->SmallAllocator) {
        tb_allocator_exit(kstate->SmallAllocator);
    }
    kstate->SmallAllocator = tb_null;
    
    tb_exit();
    return 0;
}

KN KON_EvalFile(KonState* kstate, char* filePath)
{
    KON_DEBUG("KON_EvalFile enter");

    KonReader* reader = KSON_ReaderInit(&kstate);
    if (!reader) {
        KON_DEBUG("KON_EvalFile init failed");
        exit(1);
    }

    KN result = KON_UNDEF;

    bool openRes = KSON_ReaderFromFile(reader, filePath);
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

    return result;
}
