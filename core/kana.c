#include "kana.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "prefix.h"


int ENABLE_DEBUG = 1;

KonState* KN_Init()
{
    KonState* kstate = (KonState*)calloc(1, sizeof(KonState));
    if (kstate == NULL) {
        return NULL;
    }
    // kstate->base.tag = KN_T_STATE;
    kstate->nextMsgDispatcherId = 100;
    kstate->msgDispatchers = KxVector_InitWithSize(200);

    GcState* gcState = (GcState*)calloc(1, sizeof(GcState));
    kstate->gcState = gcState;

    // init root env
    // KN env = KN_MakeRootEnv(kstate);
    // KN_DEBUG("root env addr %x", env);     
    // kstate->value.Context.rootEnv = env;

    KN_InitGc(kstate);

    return kstate;
}


int KN_Finish(KonState* kstate)
{
    
    KN_DestroyGc(kstate);

    tb_exit();
    free(kstate);
    return 0;
}