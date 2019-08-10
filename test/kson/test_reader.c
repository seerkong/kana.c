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
    char* filePathOrigin = "~/lang/konscript/kon-c/samples/kon/cell.kon";
    
    KonReader* reader = KSON_ReaderInit(&kstate);
    if (!reader) {
        KON_DEBUG("KON_EvalFile init failed");
        exit(1);
    }

    KN result = KON_NULL;

    bool openRes = KSON_ReaderFromFile(reader, filePathOrigin);
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

    return 0;
}
