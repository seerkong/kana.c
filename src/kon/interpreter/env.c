#include "env.h"

Kon* KON_MakeRootEnv(Kon* kstate)
{
    Kon* env = KON_AllocTagged(kstate, sizeof(KonEnv), KON_ENV);
    env->Value.Env.Parent = KON_NULL;
    env->Value.Env.Bindings = tb_hash_map_init(8, tb_element_str(tb_true), tb_element_ptr(kon_hash_item_ptr_free, "ValueBuilderType"));

    KON_EnvDefine(kstate, env, "writeln",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryWriteln)
    );
    return env;
}

Kon* KON_MakeChildEnv(Kon* kstate, Kon* parentEnv)
{
    Kon* env = KON_AllocTagged(kstate, sizeof(KonEnv), KON_ENV);
    env->Value.Env.Parent = parentEnv;
    env->Value.Env.Bindings = tb_hash_map_init(8, tb_element_str(tb_true), tb_element_ptr(kon_hash_item_ptr_free, "ValueBuilderType"));
    return env;
}

Kon* KON_EnvDefine(Kon* kstate, Kon* env, char* key, Kon* value)
{
    tb_hash_map_insert(env->Value.Env.Bindings, key, (tb_pointer_t)value);
    return KON_TRUE;
}

Kon* KON_EnvLookup(Kon* kstate, Kon* env, char* key)
{
    Kon* value = (Kon*)tb_hash_map_get(env->Value.Env.Bindings, key);
    if (value) {
        return value;
    }
    else if (env->Value.Env.Parent == KON_NULL) {
        return KON_NULL;
    }
    else {
        return KON_EnvLookup(kstate, env->Value.Env.Parent, key);
    }
}
