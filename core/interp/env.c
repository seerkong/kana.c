

#include "env.h"

KonEnv* KN_MakeRootEnv(Kana* kana)
{
    KonEnv* env = KN_NEW_DYNAMIC_OBJ(kana, KonEnv, KN_T_ENV);
    env->parent = (KonEnv*)KNBOX_NIL;
    env->bindings = KnHashTable_Init(4);
    return env;
}

KonEnv* KN_MakeChildEnv(Kana* kana, KonEnv* parentEnv)
{
    KonEnv* env = KN_NEW_DYNAMIC_OBJ(kana, KonEnv, KN_T_ENV);
    env->parent = parentEnv;
    env->bindings = KnHashTable_Init(4);
    return env;
}

KN KN_EnvDefine(Kana* kana, KonEnv* env, const char* key, KN value)
{
    KnHashTable_PutKv(env->bindings, key, value.asU64);
    return KN_TRUE;
}

KN KN_EnvLookup(Kana* kana, KonEnv* env, const char* key)
{
    KN value = (KN)KnHashTable_AtKey(env->bindings, key);
    if (value.asU64 && value.asU64 != KNBOX_UNDEF) {
        return value;
    }
    else if (env->parent == KNBOX_NIL) {
        return KN_UKN;
    }
    else {
        return KN_EnvLookup(kana, env->parent, key);
    }
}

KN KN_EnvLookupSet(Kana* kana, KonEnv* env, const char* key, KN value)
{
    KN slot = (KN)KnHashTable_AtKey(env->bindings, key);
    if (slot.asU64) {
        KnHashTable_PutKv(env->bindings, key, value.asU64);
        return KN_TRUE;
    }
    else if (env->parent == KNBOX_NIL) {
        return KN_FALSE;
    }
    else {
        return KN_EnvLookupSet(kana, env->parent, key, value);
    }
}

void KN_EnvImportNative(Kana* kana, KonEnv* env, NativeExportConf conf)
{
    NativeExportItem* items = conf.items;
    for (int i = 0; i < conf.len; i++) {
        NativeExportItem item = items[i];
        KN value;
        switch (item.type) {
            case KN_NATIVE_EXPORT_PROC: {
                value = MakeNativeProcedure(
                    kana,
                    item.proc.type,
                    item.proc.funcRef,
                    item.proc.paramNum,
                    item.proc.hasVAList,
                    item.proc.hasVAMap
                );
            }
        }
        
        KN_EnvDefine(kana, env, item.name, value);
    }
}