#include "kon_file.h"
#include <stdio.h>
#include <stdlib.h>

KN KonFile_Init(KonState* kstate, KN args)
{
    KN path = KN_CAR(args);
    KN mode = KN_CADR(args);
    const char* modeStr = KxStringBuffer_Cstr(KN_UNBOX_STRING(mode));
    const char* pathCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(path));
    FILE* fp = fopen(pathCstr, modeStr);
    return KN_MakeCpointer(kstate, fp);
}

KN KonFile_Close(KonState* kstate, KN args)
{
    KN fh = KN_CAR(args);
    FILE* fp = CAST_Kon(Cpointer, fh)->Pointer;
    fclose(fp);
    return KN_TRUE;
}

KN KonFile_ReadAll(KonState* kstate, KN args)
{
    KN fh = KN_CAR(args);
    KxStringBuffer* sb = KxStringBuffer_New();
    FILE* fp = CAST_Kon(Cpointer, fh)->Pointer;
    int ch;
    ch = fgetc(fp);
    while (ch != EOF) {
        KxStringBuffer_NAppendChar(sb, ch, 1);
        ch = fgetc(fp);
    }
    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->String = sb;
    return value;
}

KN KonFile_Puts(KonState* kstate, KN args)
{
    KN fh = KN_CAR(args);
    KN data = KN_CADR(args);
    FILE* fp = CAST_Kon(Cpointer, fh)->Pointer;
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
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonFile_Init),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "close",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonFile_Close),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "read-all",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonFile_ReadAll),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "puts",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonFile_Puts),
        "r",
        NULL
    );

    return slot;
}
