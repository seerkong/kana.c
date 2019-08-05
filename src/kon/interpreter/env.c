#include "env.h"
#include "mod/kon_module.h"

KN KON_MakeRootEnv(KonState* kstate)
{
    KonEnv* env = KON_ALLOC_TYPE_TAG(kstate, KonEnv, KON_T_ENV);
    env->Parent = KON_NIL;
    env->Bindings = KxHashTable_Init(4);
    env->MsgDispatchers = KxHashTable_Init(4);

    // kon module
    KON_EnvDefine(kstate, (KN)env, "kon",
        KonModule_Init(kstate)
    );

    // math
    KON_EnvDefine(kstate, (KN)env, "+",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryPlus)
    );
    KON_EnvDefine(kstate, (KN)env, "-",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryMinus)
    );
    KON_EnvDefine(kstate, (KN)env, "*",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryMultiply)
    );
    KON_EnvDefine(kstate, (KN)env, "/",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryDivide)
    );
    KON_EnvDefine(kstate, (KN)env, "mod",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryMod)
    );
    KON_EnvDefine(kstate, (KN)env, "lt",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryLowerThan)
    );
    KON_EnvDefine(kstate, (KN)env, "lte",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryLowerOrEqual)
    );
    KON_EnvDefine(kstate, (KN)env, "gt",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryGreaterThan)
    );
    KON_EnvDefine(kstate, (KN)env, "gte",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryGreaterOrEqual)
    );
    

    // IO
    KON_EnvDefine(kstate, (KN)env, "newline",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryNewline)
    );
    KON_EnvDefine(kstate, (KN)env, "display",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryDisplay)
    );
    KON_EnvDefine(kstate, (KN)env, "displayln",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryDisplayln)
    );
    KON_EnvDefine(kstate, (KN)env, "write",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryWrite)
    );
    KON_EnvDefine(kstate, (KN)env, "writeln",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryWriteln)
    );

    KON_EnvDefine(kstate, (KN)env, "stringify",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryStringify)
    );

    return env;
}

KN KON_MakeChildEnv(KonState* kstate, KN parentEnv)
{
    KonEnv* env = KON_ALLOC_TYPE_TAG(kstate, KonEnv, KON_T_ENV);
    env->Parent = parentEnv;
    env->Bindings = KxHashTable_Init(4);
    env->MsgDispatchers = KxHashTable_Init(4);
    return env;
}

KN KON_EnvDefine(KonState* kstate, KN env, const char* key, KN value)
{
    KxHashTable_PutKv(CAST_Kon(Env, env)->Bindings, key, value);
    return KON_TRUE;
}

KN KON_EnvLookup(KonState* kstate, KN env, const char* key)
{
    KN value = KxHashTable_AtKey(CAST_Kon(Env, env)->Bindings, key);
    if (value && value != KON_NULL) {
        return value;
    }
    else if (CAST_Kon(Env, env)->Parent == KON_NIL) {
        return KON_UKN;
    }
    else {
        return KON_EnvLookup(kstate, CAST_Kon(Env, env)->Parent, key);
    }
}

KN KON_EnvLookupSet(KonState* kstate, KN env, const char* key, KN value)
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


KN KON_EnvDispatcherDefine(KonState* kstate, KN env, const char* key, KN value)
{
    KxHashTable_PutKv(CAST_Kon(Env, env)->MsgDispatchers, key, value);
    return KON_TRUE;
}

KN KON_EnvDispatcherLookup(KonState* kstate, KN env, const char* key)
{
    KN value = KxHashTable_AtKey(CAST_Kon(Env, env)->MsgDispatchers, key);
    if (value && value != KON_NULL) {
        return value;
    }
    else if (CAST_Kon(Env, env)->Parent == KON_NIL) {
        return KON_UKN;
    }
    else {
        return KON_EnvDispatcherLookup(kstate, CAST_Kon(Env, env)->Parent, key);
    }
}

KN KON_EnvDispatcherLookupSet(KonState* kstate, KN env, const char* key, KN value)
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
