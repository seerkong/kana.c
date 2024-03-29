#include "env.h"
#include "../module/kon_module.h"

KonEnv* KN_MakeRootEnv(KonState* kstate)
{
    KonEnv* env = KN_ALLOC_TYPE_TAG(kstate, KonEnv, KN_T_ENV);
    env->parent = KN_NIL;
    env->bindings = KxHashTable_Init(4);

    // kon module
    KN_EnvDefine(kstate, env, "kn",
        KonModule_Export(kstate, env)
    );

    KN_PrimaryOpExport(kstate, env);

    return env;
}

KonEnv* KN_MakeChildEnv(KonState* kstate, KonEnv* parentEnv)
{
    KonEnv* env = KN_ALLOC_TYPE_TAG(kstate, KonEnv, KN_T_ENV);
    env->parent = parentEnv;
    env->bindings = KxHashTable_Init(4);
    return env;
}

KN KN_EnvDefine(KonState* kstate, KonEnv* env, const char* key, KN value)
{
    KxHashTable_PutKv(CAST_Kon(Env, env)->bindings, key, value);
    return KN_TRUE;
}

KN KN_EnvLookup(KonState* kstate, KonEnv* env, const char* key)
{
    KN value = KxHashTable_AtKey(CAST_Kon(Env, env)->bindings, key);
    if (value && value != KN_UNDEF) {
        return value;
    }
    else if ((KN)CAST_Kon(Env, env)->parent == KN_NIL) {
        return KN_UKN;
    }
    else {
        return KN_EnvLookup(kstate, CAST_Kon(Env, env)->parent, key);
    }
}

KN KN_EnvLookupSet(KonState* kstate, KonEnv* env, const char* key, KN value)
{
    KN slot = KxHashTable_AtKey(CAST_Kon(Env, env)->bindings, key);
    if (slot) {
        KxHashTable_PutKv(CAST_Kon(Env, env)->bindings, key, value);
        return KN_TRUE;
    }
    else if ((KN)CAST_Kon(Env, env)->parent == KN_NIL) {
        return KN_FALSE;
    }
    else {
        return KN_EnvLookupSet(kstate, CAST_Kon(Env, env)->parent, key, value);
    }
}
