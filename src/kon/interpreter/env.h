#ifndef KON_INTERPRETER_ENV_H
#define KON_INTERPRETER_ENV_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../prefix.h"
#include "types.h"
#include "primary.h"

KN KON_MakeRootEnv(KonState* kstate);
KN KON_MakeChildEnv(KonState* kstate, KN parentEnv);
KN KON_EnvDefine(KonState* kstate, KN env, char* key, KN value);
KN KON_EnvLookup(KonState* kstate, KN env, char* key);

#ifdef __cplusplus
}
#endif

#endif