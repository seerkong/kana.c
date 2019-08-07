#include "kon_string.h"
#include "../../string/kx_stringbuffer.h"

KN KonString_Init(KonState* kstate, KN args)
{
    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KxStringBuffer_New();
    return value;
}

KN KonString_Length(KonState* kstate, KN args)
{
    KxStringBuffer* value = KON_UNBOX_STRING(KON_CAR(args));
    return KON_MAKE_FIXNUM(KxStringBuffer_Length(value));
}

KN KonString_Clear(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KxStringBuffer* value = KON_UNBOX_STRING(self);
    KxStringBuffer_Clear(value);
    return self;
}

KN KonString_AppendStr(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KxStringBuffer* value = KON_UNBOX_STRING(self);
    KxStringBuffer* other = KON_UNBOX_STRING(KON_CADR(args));
    KxStringBuffer_AppendStringBuffer(value, other);
    return self;
}

KonAttrSlot* KonString_Export(KonState* kstate, KonEnv* env)
{
    KON_EnvDefine(kstate, env, "str-init",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonString_Init)
    );

    KON_EnvDefine(kstate, env, "str-length",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonString_Length)
    );

    KON_EnvDefine(kstate, env, "str-clear",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonString_Clear)
    );

    KON_EnvDefine(kstate, env, "str-append-str",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonString_AppendStr)
    );

    KonAttrSlot* slot = (KonAttrSlot*)MakeAttrSlotFolder(kstate, "");
    return slot;
}