#include "kon_string.h"
#include "../string/kx_stringbuffer.h"

KN KonString_Init(KonState* kstate)
{
    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->string = KxStringBuffer_New();
    return value;
}

KN KonString_Length(KonState* kstate, KN self)
{
    KxStringBuffer* value = KN_UNBOX_STRING(self);
    return KN_MAKE_FIXNUM(KxStringBuffer_Length(value));
}

KN KonString_Clear(KonState* kstate, KN self)
{
    KxStringBuffer* value = KN_UNBOX_STRING(self);
    KxStringBuffer_Clear(value);
    return self;
}

KN KonString_AppendStr(KonState* kstate, KN self, KN right)
{
    KxStringBuffer* value = KN_UNBOX_STRING(self);
    KxStringBuffer* other = KN_UNBOX_STRING(right);
    KxStringBuffer_AppendStringBuffer(value, other);
    return self;
}

KN KonString_SubStr(KonState* kstate, KN self, KN start, KN end)
{
    int startNum = KN_UNBOX_FIXNUM(start);
    int endNum = KN_UNBOX_FIXNUM(end);
    KxStringBuffer* value = KN_UNBOX_STRING(self);
    int len = KxStringBuffer_Length(value);

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    if (startNum < 0 || endNum < 0
        || startNum > len - 1 || endNum > len - 1
        || startNum >= endNum
    ) {
        return result;
    }

    const char* startPtr = KxStringBuffer_OffsetPtr(value, startNum);
    KxStringBuffer_NAppendCstr(result->string, startPtr, (endNum - startNum));
    return result;
}

KonAccessor* KonString_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* slot = (KonAccessor*)KN_MakeDirAccessor(kstate, "dr", NULL);
    
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "init",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonString_Init, 0, 0, 0),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "length",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonString_Length, 1, 0, 0),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "clear",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonString_Clear, 1, 0, 0),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "append-str",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonString_AppendStr, 2, 0, 0),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "sub-str",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonString_SubStr, 3, 0, 0),
        "r",
        NULL
    );
    
    return slot;
}
