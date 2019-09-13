#include "kon_file.h"
#include <stdio.h>
#include <stdlib.h>

KN KonFile_Init(KonState* kstate, KN path, KN mode)
{
    const char* modeStr = KxStringBuffer_Cstr(KN_UNBOX_STRING(mode));
    const char* pathCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(path));
    FILE* fp = fopen(pathCstr, modeStr);
    return KN_MAKE_EXT_POINTER(fp);
}

KN KonFile_Close(KonState* kstate, KN fh)
{
    FILE* fp = KN_UNBOX_EXT_POINTER(fh);
    fclose(fp);
    return KN_TRUE;
}

KN KonFile_ReadAll(KonState* kstate, KN fh)
{
    KxStringBuffer* sb = KxStringBuffer_New();
    FILE* fp = KN_UNBOX_EXT_POINTER(fh);
    int ch;
    ch = fgetc(fp);
    while (ch != EOF) {
        KxStringBuffer_NAppendChar(sb, ch, 1);
        ch = fgetc(fp);
    }
    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->string = sb;
    return value;
}

KN KonFile_Puts(KonState* kstate, KN fh, KN data)
{
    FILE* fp = KN_UNBOX_EXT_POINTER(fh);
    const char* dataCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(data));
    // fprintf(fp, "");
    fprintf(fp, dataCstr);
    return KN_TRUE;
}

KonAccessor* KonFile_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* slot = (KonAccessor*)KN_MakeDirAccessor(kstate, "", NULL);

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "init",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonFile_Init, 2, 0, 0),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "close",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonFile_Close, 1, 0, 0),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "read-all",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonFile_ReadAll, 1, 0, 0),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "puts",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonFile_Puts, 2, 0, 0),
        "r",
        NULL
    );

    return slot;
}
