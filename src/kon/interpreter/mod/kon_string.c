#include "kon_string.h"
#include "../../string/kx_stringbuffer.h"

KN KonString_Init(KonState* kstate, KN args)
{
    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->String = KxStringBuffer_New();
    return value;
}

KN KonString_Length(KonState* kstate, KN args)
{
    KxStringBuffer* value = KN_UNBOX_STRING(KN_CAR(args));
    return KN_MAKE_FIXNUM(KxStringBuffer_Length(value));
}

KN KonString_Clear(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KxStringBuffer* value = KN_UNBOX_STRING(self);
    KxStringBuffer_Clear(value);
    return self;
}

KN KonString_AppendStr(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KxStringBuffer* value = KN_UNBOX_STRING(self);
    KxStringBuffer* other = KN_UNBOX_STRING(KN_CADR(args));
    KxStringBuffer_AppendStringBuffer(value, other);
    return self;
}

KonAccessor* KonString_Export(KonState* kstate, KonEnv* env)
{
    KN_EnvDefine(kstate, env, "str-init",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonString_Init)
    );

    KN_EnvDefine(kstate, env, "str-length",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonString_Length)
    );

    KN_EnvDefine(kstate, env, "str-clear",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonString_Clear)
    );

    KN_EnvDefine(kstate, env, "str-append-str",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonString_AppendStr)
    );

    KonAccessor* slot = (KonAccessor*)KN_MakeDirAccessor(kstate, "dr", NULL);
    return slot;
}