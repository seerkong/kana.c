#include "kana.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "prefix.h"


int ENABLE_DEBUG = 1;

Kana* KN_Init()
{
    Kana* kana = (Kana*)calloc(1, sizeof(Kana));
    if (kana == NULL) {
        return NULL;
    }
    // kana->base.tag = KN_T_STATE;
    kana->nextMsgDispatcherId = 100;
    kana->msgDispatchers = KnVector_InitWithSize(200);


    GcState* gcState = (GcState*)calloc(1, sizeof(GcState));
    kana->gcState = gcState;

    // init root env
    // KN env = KN_MakeRootEnv(kana);
    // KN_DEBUG("root env addr %x", env);
    // kana->value.Context.rootEnv = env;

    KN_InitGc(kana);
    return kana;
}


int KN_Finish(Kana* kana)
{
    KN_DestroyGc(kana);

    tb_exit();
    free(kana);
    return 0;
}
