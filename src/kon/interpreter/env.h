#ifndef KON_INTERPRETER_ENV_H
#define KON_INTERPRETER_ENV_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../prefix.h"
#include "types.h"
#include "primary.h"

KN KON_MakeRootEnv(KonState* kstate);
KN KON_MakeChildEnv(KonState* kstate, KonEnv* parentEnv);
KN KON_EnvDefine(KonState* kstate, KonEnv* env, const char* key, KN value);
KN KON_EnvLookup(KonState* kstate, KonEnv* env, const char* key);

KN KON_EnvDispatcherDefine(KonState* kstate, KonEnv* env, const char* key, KN value);
KN KON_EnvDispatcherLookup(KonState* kstate, KonEnv* env, const char* key);
KN KON_EnvDispatcherLookupSet(KonState* kstate, KonEnv* env, const char* key, KN value);

#ifdef __cplusplus
}
#endif

#endif