#ifndef kn_interp_env_h
#define kn_interp_env_h 1

#include "prefix.h"

KonEnv* KN_MakeRootEnv(KonState* kstate);
KonEnv* KN_MakeChildEnv(KonState* kstate, KonEnv* parentEnv);
KN KN_EnvDefine(KonState* kstate, KonEnv* env, const char* key, KN value);
KN KN_EnvLookup(KonState* kstate, KonEnv* env, const char* key);


#endif