#include "kon_string.h"
#include "../string/kx_stringbuffer.h"

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

KN KonString_SubStr(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN start = KN_CADR(args);
    KN end = KN_CADDR(args);

    int startNum = KN_UNBOX_FIXNUM(start);
    int endNum = KN_UNBOX_FIXNUM(end);
    KxStringBuffer* value = KN_UNBOX_STRING(self);
    int len = KxStringBuffer_Length(value);

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->String = KxStringBuffer_New();

    if (startNum < 0 || endNum < 0
        || startNum > len - 1 || endNum > len - 1
        || startNum >= endNum
    ) {
        return result;
    }

    const char* startPtr = KxStringBuffer_OffsetPtr(value, startNum);
    KxStringBuffer_NAppendCstr(result->String, startPtr, (endNum - startNum));
    return result;
}

KonAccessor* KonString_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* slot = (KonAccessor*)KN_MakeDirAccessor(kstate, "dr", NULL);
    
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "init",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonString_Init),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "length",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonString_Length),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "clear",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonString_Clear),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "append-str",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonString_AppendStr),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "sub-str",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonString_SubStr),
        "r",
        NULL
    );
    
    return slot;
}
