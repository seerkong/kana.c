

#include "env.h"

KonEnv* KN_MakeRootEnv(KonState* kstate)
{
    KonEnv* env = KN_NEW_DYNAMIC_OBJ(kstate, KonEnv, KN_T_ENV);
    env->parent = (KonEnv*)KNBOX_NIL;
    env->bindings = KxHashTable_Init(4);

    // kon module
    // KN_EnvDefine(kstate, env, "kn",
    //     KonModule_Export(kstate, env)
    // );

    KN_PrimaryOpExport(kstate, env);

    return env;
}

KonEnv* KN_MakeChildEnv(KonState* kstate, KonEnv* parentEnv)
{
    KonEnv* env = KN_NEW_DYNAMIC_OBJ(kstate, KonEnv, KN_T_ENV);
    env->parent = parentEnv;
    env->bindings = KxHashTable_Init(4);
    return env;
}

KN KN_EnvDefine(KonState* kstate, KonEnv* env, const char* key, KN value)
{
    KxHashTable_PutKv(env->bindings, key, value.asU64);
    return KN_TRUE;
}

KN KN_EnvLookup(KonState* kstate, KonEnv* env, const char* key)
{
    KN value = (KN)KxHashTable_AtKey(env->bindings, key);
    if (value.asU64 && value.asU64 != KNBOX_UNDEF) {
        return value;
    }
    else if (env->parent == KNBOX_NIL) {
        return KN_UKN;
    }
    else {
        return KN_EnvLookup(kstate, env->parent, key);
    }
}

KN KN_EnvLookupSet(KonState* kstate, KonEnv* env, const char* key, KN value)
{
    KN slot = (KN)KxHashTable_AtKey(env->bindings, key);
    if (slot.asU64) {
        KxHashTable_PutKv(env->bindings, key, value.asU64);
        return KN_TRUE;
    }
    else if (env->parent == KNBOX_NIL) {
        return KN_FALSE;
    }
    else {
        return KN_EnvLookupSet(kstate, env->parent, key, value);
    }
}
