#include "kmod_file.h"
#include <stdio.h>
#include <stdlib.h>

KN KonFile_Init(Kana* kana, KN path, KN mode)
{
    const char* modeStr = KxStringBuffer_Cstr(KN_UNBOX_STRING(mode));
    const char* pathCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(path));
    FILE* fp = fopen(pathCstr, modeStr);
    return KN_BOX_EXT_PTR(fp);
}

KN KonFile_Close(Kana* kana, KN fh)
{
    FILE* fp = KN_UNBOX_EXT_PTR(fh);
    fclose(fp);
    return KN_TRUE;
}

KN KonFile_ReadAll(Kana* kana, KN fh)
{
    KxStringBuffer* sb = KxStringBuffer_New();
    FILE* fp = KN_UNBOX_EXT_PTR(fh);
    int ch;
    ch = fgetc(fp);
    while (ch != EOF) {
        KxStringBuffer_NAppendChar(sb, ch, 1);
        ch = fgetc(fp);
    }
    KonString* value = KN_NEW_DYNAMIC_OBJ(kana, KonString, KN_T_STRING);
    value->string = sb;
    return KON_2_KN(value);
}

KN KonFile_Puts(Kana* kana, KN fh, KN data)
{
    FILE* fp = (FILE*)KN_UNBOX_EXT_PTR(fh);
    const char* dataCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(data));
    // fprintf(fp, "");
    fprintf(fp, "%s", dataCstr);
    return KN_TRUE;
}

NativeExportConf Kmod_File_Export(Kana* kana)
{
    NativeExportItem exportArr[] = {
        { .type = KN_NATIVE_EXPORT_PROC, .name = "init", .proc = { KN_NATIVE_FUNC, KonFile_Init, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "close", .proc = { KN_NATIVE_FUNC, KonFile_Close, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "close", .proc = { KN_NATIVE_FUNC, KonFile_Close, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "read-all", .proc = { KN_NATIVE_FUNC, KonFile_ReadAll, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "puts", .proc = { KN_NATIVE_FUNC, KonFile_Puts, 2, 0, 0, 0 } },
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