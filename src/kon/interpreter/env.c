#include "env.h"
#include "mod/kon_module.h"

KN KON_MakeRootEnv(KonState* kstate)
{
    KonEnv* env = KON_ALLOC_TYPE_TAG(kstate, KonEnv, KON_T_ENV);
    env->Parent = KON_NIL;
    env->Bindings = KxHashTable_Init(4);
    env->MsgDispatchers = KxHashTable_Init(4);

    // kon module
    KON_EnvDefine(kstate, env, "kon",
        KonModule_Export(kstate, env)
    );

    KON_PrimaryOpExport(kstate, env);

    return env;
}

KN KON_MakeChildEnv(KonState* kstate, KonEnv* parentEnv)
{
    KonEnv* env = KON_ALLOC_TYPE_TAG(kstate, KonEnv, KON_T_ENV);
    env->Parent = parentEnv;
    env->Bindings = KxHashTable_Init(4);
    env->MsgDispatchers = KxHashTable_Init(4);
    return env;
}

KN KON_EnvDefine(KonState* kstate, KonEnv* env, const char* key, KN value)
{
    KxHashTable_PutKv(CAST_Kon(Env, env)->Bindings, key, value);
    return KON_TRUE;
}

KN KON_EnvLookup(KonState* kstate, KonEnv* env, const char* key)
{
    KN value = KxHashTable_AtKey(CAST_Kon(Env, env)->Bindings, key);
    if (value && value != KON_UNDEF) {
        return value;
    }
    else if (CAST_Kon(Env, env)->Parent == KON_NIL) {
        return KON_UKN;
    }
    else {
        return KON_EnvLookup(kstate, CAST_Kon(Env, env)->Parent, key);
    }
}

KN KON_EnvLookupSet(KonState* kstate, KonEnv* env, const char* key, KN value)
{
    KN slot = KxHashTable_AtKey(CAST_Kon(Env, env)->Bindings, key);
    if (slot) {
        KxHashTable_PutKv(CAST_Kon(Env, env)->Bindings, key, value);
        return KON_TRUE;
    }
    else if (CAST_Kon(Env, env)->Parent == KON_NIL) {
        return KON_FALSE;
    }
    else {
        return KON_EnvLookupSet(kstate, CAST_Kon(Env, env)->Parent, key, value);
    }
}


KN KON_EnvDispatcherDefine(KonState* kstate, KonEnv* env, const char* key, KN value)
{
    KxHashTable_PutKv(CAST_Kon(Env, env)->MsgDispatchers, key, value);
    return KON_TRUE;
}

KN KON_EnvDispatcherLookup(KonState* kstate, KonEnv* env, const char* key)
{
    KN value = KxHashTable_AtKey(CAST_Kon(Env, env)->MsgDispatchers, key);
    if (value && value != KON_UNDEF) {
        return value;
    }
    else if (CAST_Kon(Env, env)->Parent == KON_NIL) {
        return KON_UKN;
    }
    else {
        return KON_EnvDispatcherLookup(kstate, CAST_Kon(Env, env)->Parent, key);
    }
}

KN KON_EnvDispatcherLookupSet(KonState* kstate, KonEnv* env, const char* key, KN value)
{
    KN slot = KxHashTable_AtKey(CAST_Kon(Env, env)->MsgDispatchers, key);
    if (slot) {
        KxHashTable_PutKv(CAST_Kon(Env, env)->MsgDispatchers, key, value);
        return KON_TRUE;
    }
    else if (CAST_Kon(Env, env)->Parent == KON_NIL) {
        return KON_FALSE;
    }
    else {
        return KON_EnvDispatcherLookupSet(kstate, CAST_Kon(Env, env)->Parent, key, value);
    }
}
