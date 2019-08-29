#ifndef KN_INTERPRETER_ENV_H
#define KN_INTERPRETER_ENV_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../prefix.h"
#include "types.h"
#include "primary.h"

KonEnv* KN_MakeRootEnv(KonState* kstate);
KonEnv* KN_MakeChildEnv(KonState* kstate, KonEnv* parentEnv);
KN KN_EnvDefine(KonState* kstate, KonEnv* env, const char* key, KN value);
KN KN_EnvLookup(KonState* kstate, KonEnv* env, const char* key);

#ifdef __cplusplus
}
#endif

#endif