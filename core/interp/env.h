#ifndef kn_interp_env_h
#define kn_interp_env_h 1

#include "prefix.h"

KonEnv* KN_MakeRootEnv(Kana* kana);
KonEnv* KN_MakeChildEnv(Kana* kana, KonEnv* parentEnv);
KN KN_EnvDefine(Kana* kana, KonEnv* env, const char* key, KN value);
KN KN_EnvLookup(Kana* kana, KonEnv* env, const char* key);
void KN_EnvImportNative(Kana* kana, KonEnv* env, NativeExportConf conf);

#endif