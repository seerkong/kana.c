#include "kmod_string.h"
#include "../string/kn_stringbuffer.h"

KN KonString_Init(Kana* kana)
{
    KonString* value = KN_NEW_DYNAMIC_OBJ(kana, KonString, KN_T_STRING);
    value->string = KxStringBuffer_New();
    return KON_2_KN(value);
}

KN KonString_Length(Kana* kana, KN self)
{
    KxStringBuffer* value = KN_UNBOX_STRING(self);
    return KN_MAKE_FIXNUM(KxStringBuffer_Length(value));
}

KN KonString_Clear(Kana* kana, KN self)
{
    KxStringBuffer* value = KN_UNBOX_STRING(self);
    KxStringBuffer_Clear(value);
    return self;
}

KN KonString_AppendStr(Kana* kana, KN self, KN right)
{
    KxStringBuffer* value = KN_UNBOX_STRING(self);
    KxStringBuffer* other = KN_UNBOX_STRING(right);
    KxStringBuffer_AppendStringBuffer(value, other);
    return self;
}

KN KonString_SubStr(Kana* kana, KN self, KN start, KN end)
{
    int startNum = KN_UNBOX_FIXNUM(start);
    int endNum = KN_UNBOX_FIXNUM(end);
    KxStringBuffer* value = KN_UNBOX_STRING(self);
    int len = KxStringBuffer_Length(value);

    KonString* result = KN_NEW_DYNAMIC_OBJ(kana, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    if (startNum < 0 || endNum < 0
        || startNum > len - 1 || endNum > len - 1
        || startNum >= endNum
    ) {
        return KON_2_KN(result);
    }

    const char* startPtr = KxStringBuffer_OffsetPtr(value, startNum);
    KxStringBuffer_NAppendCstr(result->string, startPtr, (endNum - startNum));
    return KON_2_KN(result);
}

NativeExportConf Kmod_String_Export(Kana* kana)
{
    NativeExportItem exportArr[] = {
        { .type = KN_NATIVE_EXPORT_PROC, .name = "init", .proc = { KN_NATIVE_FUNC, KonString_Init, 0, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "length", .proc = { KN_NATIVE_FUNC, KonString_Length, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "clear", .proc = { KN_NATIVE_FUNC, KonString_Clear, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "append-str", .proc = { KN_NATIVE_FUNC, KonString_AppendStr, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "sub-str", .proc = { KN_NATIVE_FUNC, KonString_SubStr, 3, 0, 0, 0 } },
    };

    int len = 5;
    NativeExportItem* items = (NativeExportItem*)calloc(len, sizeof(NativeExportItem));
    memcpy(items, exportArr, len * sizeof(NativeExportItem));
    NativeExportConf res = {
        .len = len,
        .items = items
    };
    
    return res;
}
