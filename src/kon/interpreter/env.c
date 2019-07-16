#include "env.h"
#include "../kson/hashmap.h"

KN KON_MakeRootEnv(KonState* kstate)
{
    KonEnv* env = KON_ALLOC_TYPE_TAG(kstate, KonEnv, KON_T_ENV);
    env->Parent = KON_NIL;
    env->Bindings = KON_HashMapInit(20);

    // math
    KON_EnvDefine(kstate, env, "+",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryPlus)
    );
    KON_EnvDefine(kstate, env, "-",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryMinus)
    );
    KON_EnvDefine(kstate, env, "*",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryMultiply)
    );
    KON_EnvDefine(kstate, env, "/",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryDivide)
    );
    KON_EnvDefine(kstate, env, "mod",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryMod)
    );
    KON_EnvDefine(kstate, env, "lt",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryLowerThan)
    );
    KON_EnvDefine(kstate, env, "lte",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryLowerOrEqual)
    );
    KON_EnvDefine(kstate, env, "gt",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryGreaterThan)
    );
    KON_EnvDefine(kstate, env, "gte",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryGreaterOrEqual)
    );
    

    // IO
    KON_EnvDefine(kstate, env, "newline",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryNewline)
    );
    KON_EnvDefine(kstate, env, "display",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryDisplay)
    );
    KON_EnvDefine(kstate, env, "displayln",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryDisplayln)
    );
    KON_EnvDefine(kstate, env, "write",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryWrite)
    );
    KON_EnvDefine(kstate, env, "writeln",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryWriteln)
    );

    KON_EnvDefine(kstate, env, "stringify",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryStringify)
    );

    return env;
}

KN KON_MakeChildEnv(KonState* kstate, KN parentEnv)
{
    KonEnv* env = KON_ALLOC_TYPE_TAG(kstate, KonEnv, KON_T_ENV);
    env->Parent = parentEnv;
    env->Bindings =  KON_HashMapInit(20);
    return env;
}

KN KON_EnvDefine(KonState* kstate, KN env, char* key, KN value)
{
    KON_HashMapPut(CAST_Kon(Env, env)->Bindings, key, value);
    return KON_TRUE;
}

KN KON_EnvLookup(KonState* kstate, KN env, char* key)
{
    KN value = KON_HashMapGet(CAST_Kon(Env, env)->Bindings, key);
    if (value) {
        return value;
    }
    else if (CAST_Kon(Env, env)->Parent == KON_NIL) {
        return KON_UKN;
    }
    else {
        return KON_EnvLookup(kstate, CAST_Kon(Env, env)->Parent, key);
    }
}

KN KON_EnvLookupSet(KonState* kstate, KN env, char* key, KN value)
{
    KN slot = KON_HashMapGet(CAST_Kon(Env, env)->Bindings, key);
    if (slot) {
        KON_HashMapPut(CAST_Kon(Env, env)->Bindings, key, value);
        return KON_TRUE;
    }
    else if (CAST_Kon(Env, env)->Parent == KON_NIL) {
        return KON_FALSE;
    }
    else {
        return KON_EnvLookupSet(kstate, CAST_Kon(Env, env)->Parent, key, value);
    }
}
