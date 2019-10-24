

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#include <pwd.h>
#include <sys/types.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdint.h>

#include "value.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

KxStringBuffer* KN_ReadFileContent(const char* filePathOrigin)
{
    int originPathStrLen = strlen(filePathOrigin);
    if ((originPathStrLen + 1) > PATH_MAX) {
        printf("file path exceeded\n");
        exit(1);
    }
    char* replaceHomePath = (char*)tb_nalloc0(PATH_MAX, sizeof(char));
    // check if has user home prefix
    if (filePathOrigin != NULL
        && originPathStrLen > 2
        && filePathOrigin[0] == '~'
        && filePathOrigin[1] == '/'
    ) {
        struct passwd *pw = getpwuid(getuid());
        const char *homeDir = pw->pw_dir;
        int homeDirLen = strlen(homeDir);
        // printf("the home path is %s\n", homeDir);

        if (homeDirLen + originPathStrLen > PATH_MAX) {
            // printf("file path exceeded");
            exit(2);
        }

        memcpy(replaceHomePath, homeDir, homeDirLen);
        // copy rest
        memcpy(replaceHomePath + homeDirLen, filePathOrigin + 1, originPathStrLen - 1);
    }
    else {
        strncpy(replaceHomePath, filePathOrigin, originPathStrLen);
    }
    // printf("replaceHomePath %s\n", replaceHomePath);
    
    char* absoluteFilePath = (char*)tb_nalloc0(1024, sizeof(char));
    char *realpathRes = realpath_safe(replaceHomePath, absoluteFilePath, 1024);
    
    tb_free(replaceHomePath);
    if (realpathRes == NULL) {
        printf("abs file path exceeded\n");
        exit(3);
    }

    // printf("absoluteFilePath %s, %x\n", absoluteFilePath, realpathRes);


    FILE* fp = fopen(absoluteFilePath, "r");
    tb_free(absoluteFilePath);
    // open readonly file path
    if (fp == NULL) {
        printf("The file <%s> can not be opened.\n", absoluteFilePath);
        return 1;
    }
    KxStringBuffer* sb = KxStringBuffer_New();

    int ch;

    ch = fgetc(fp);
    while (ch != EOF) {
        KxStringBuffer_NAppendChar(sb, ch, 1);
        ch = fgetc(fp);
    }
    fclose(fp);
    return sb;
}


const char* KN_HumanFormatTime()
{
    struct timeval tv;
    static char buf[24];
    
    memset(buf, 0, sizeof(buf));
    
    // clock time
    if (gettimeofday(&tv, NULL) == -1) {
        return buf;
    }
    
    // to calendar time
    struct tm* tm;
    if ((tm = localtime((const time_t*)&tv.tv_sec)) == NULL) {
        return buf;
    }
    
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%03d",
             tm->tm_hour, tm->tm_min, tm->tm_sec,
             (int)(tv.tv_usec / 1000));
    
    buf[sizeof(buf) - 1] = 0;
    
    return buf;
}




unsigned int KN_NodeDispacherId(KonState* kstate, KN obj)
{
    if (KN_IS_BOOLEAN(obj)) {
        return KN_T_BOOLEAN;
    }
    else if (obj.asU64 == KNBOX_UKN) {
        return KN_T_UKN;
    }
    else if (obj.asU64 == KNBOX_UNDEF) {
        return KN_T_UNDEF;
    }
    else if (obj.asU64 == KNBOX_NIL) {
        return KN_T_PAIRLIST;
    }
    else if (KN_IS_FIXNUM(obj)) {
        return KN_T_NUMBER;
    }
    else if (KN_IS_CHAR(obj)) {
        return KN_T_CHAR;
    }
    else if (KN_IS_POINTER(obj)) {
        return KN_FIELD(obj, Base, msgDispatcherId);
    }
    else {
        // not handled
        return 0;
    }
}


KN KN_ToFormatString(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    if (KN_IS_FIXNUM(source)) {
        return KN_FixnumStringify(kstate, source);
    }
    else if (KN_IS_FLONUM(source)) {
        return KN_FlonumStringify(kstate, source);
    }
    else if (KN_IS_CHAR(source)) {
        return KN_CharStringify(kstate, source);
    }
    else if (KN_IS_STRING(source)) {
        return KN_StringStringify(kstate, source);
    }
    else if (KN_IS_SYMBOL(source)) {
        return KN_SymbolStringify(kstate, source);
    }
    else if (KN_IS_SYNTAX_MARKER(source)) {
        return KN_SyntaxMarkerStringify(kstate, source);
    }
    else if (KN_IS_QUOTE(source)) {
        return KN_QuoteStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_QUASIQUOTE(source)) {
        return KN_QuasiquoteStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_UNQUOTE(source)) {
        return KN_UnquoteStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_PREFIX(source)) {
        return KN_PrefixStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_SUFFIX(source)) {
        return KN_SuffixStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_TXT_MARCRO(source)) {
        printf("KN_IS_TXT_MARCRO\n");
        return KN_TxtMarcroStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_OBJ_BUILDER(source)) {
        printf("KN_IS_OBJ_BUILDER\n");
        return KN_ObjBuilderStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_VECTOR(source)) {
        return KN_VectorStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IsPairList(source)) {
        return KN_PairListStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_TABLE(source)) {
        return KN_TableStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_CELL(source)) {
        return KN_CellStringify(kstate, source, newLine, depth, padding);
    }
    else if (source.asU64 == KNBOX_UKN) {
        return KN_MakeString(kstate, "ukn");
    }
    else if (source.asU64 == KNBOX_TRUE) {
        return KN_MakeString(kstate, "true");
    }
    else if (source.asU64 == KNBOX_FALSE) {
        return KN_MakeString(kstate, "false");
    }
    else if (source.asU64 == KNBOX_UNDEF) {
        return KN_MakeString(kstate, "undefined");
    }
    else if (KN_IS_CONTINUATION(source)) {
        return KN_MakeString(kstate, "#{continuation}");
    }
    else if (KN_IS_PROCEDURE(source)) {
        return KN_MakeString(kstate, "#{procedure}");
    }
    else if (KN_IS_EXT_POINTER(source)) {
        return KN_MakeString(kstate, "#{extpointer}");
    }
    else if (KN_IS_ACCESSOR(source)) {
        return KN_AccessorStringify(kstate, source, newLine, depth, padding);
    }
    // TODO other data types
    else {
        return KN_MakeEmptyString(kstate);
    }
}

KN KN_Stringify(KonState* kstate, KN source)
{
    return KN_ToFormatString(kstate, source, false, 0, " ");
}

bool IsContainerOrWrapper()
{
    // TODO
    return true;
}

// number related
KN KN_FixnumStringify(KonState* kstate, KN source)
{
    if (!KN_IS_FIXNUM(source)) {
        return KN_MakeEmptyString(kstate);
    }
    char buf[128] = { '\0' };
    kon_int_t num = KN_UNBOX_FIXNUM(source);
    itoa(num, buf, 10);

    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->string = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->string, buf);

    return KON_2_KN(value);
}

KN KN_MakeFlonum(KonState* kstate, double f)
{
  return KN_MAKE_FLONUM(f);
}

KN KN_FlonumStringify(KonState* kstate, KN source)
{
    if (!KN_IS_FLONUM(source)) {
        return KN_MakeEmptyString(kstate);
    }
    char buf[128] = {'\0'};
    double num = KN_UNBOX_DOUBLE(source);
    double_to_str(num, 2, buf);

    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->string = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->string, buf);
    return KON_2_KN(value);
}

// stringify char to ^c,X;
KN KN_CharStringify(KonState* kstate, KN source)
{
    if (!KN_IS_CHAR(source)) {
        return KN_MakeEmptyString(kstate);
    }

    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->string = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->string, "^c,");
    int charcode = KN_UNBOX_CHAR(source);
    char buf[10] = {'\0'};
    snprintf(buf, 10, "%c", charcode);
    KxStringBuffer_AppendCstr(value->string, buf);
    KxStringBuffer_AppendCstr(value->string, ";");
    return KON_2_KN(value);
}

KN KN_StringStringify(KonState* kstate, KN source)
{
    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->string = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->string, "\"");
    KxStringBuffer_AppendStringBuffer(value->string, KN_UNBOX_STRING(source));
    KxStringBuffer_AppendCstr(value->string, "\"");
    return KON_2_KN(value);
}

KN KN_MakeString(KonState* kstate, const char* str)
{
    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->string = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->string, str);
    return KON_2_KN(value);
}

KN KN_MakeEmptyString(KonState* kstate)
{
    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->string = KxStringBuffer_New();
    return KON_2_KN(value);
}

const char* KN_StringToCstr(KN str)
{
    if (!KN_IS_STRING(str)) {
        return NULL;
    }
    return KxStringBuffer_Cstr(KN_UNBOX_STRING(str));
}

void AddLeftPadding(KxStringBuffer* str, int depth, char* padding)
{
    for (int i = 0; i < depth; i++) {
        KxStringBuffer_AppendCstr(str, padding);
    }
}

KN KN_SymbolStringify(KonState* kstate, KN source)
{
    KonSymbolType type = KN_FIELD(source, Symbol, type);
    const char* data = KN_UNBOX_SYMBOL(source);

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    switch (type) {
        case KN_SYM_MARCRO: {
            KxStringBuffer_AppendCstr(result->string, data);
            break;
        }
        case KN_SYM_CELL_SEG_END: {
            KxStringBuffer_AppendCstr(result->string, "^");
            KxStringBuffer_AppendCstr(result->string, data);
            break;
        }

        case KN_SYM_WORD: {
            KxStringBuffer_AppendCstr(result->string, data);
            break;
        }
        case KN_SYM_VARIABLE: {
            KxStringBuffer_AppendCstr(result->string, "@.");
            KxStringBuffer_AppendCstr(result->string, data);
            break;
        }
        case KN_SYM_IDENTIFIER: {
            KxStringBuffer_AppendCstr(result->string, "$.");
            KxStringBuffer_AppendCstr(result->string, data);
            break;
        }
        case KN_SYM_STRING: {
            KxStringBuffer_AppendCstr(result->string, "$\'");
            KxStringBuffer_AppendCstr(result->string, data);
            KxStringBuffer_AppendCstr(result->string, "\'");
            break;
        }
    }
    return KON_2_KN(result);
}

const char* KN_SymbolToCstr(KN sym)
{
    return KN_UNBOX_SYMBOL(sym);
}

KN KN_SyntaxMarkerStringify(KonState* kstate, KN source)
{
    KonSyntaxMarkerType type = KN_FIELD(source, SyntaxMarker, type);

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    switch (type) {
        case KN_SYNTAX_MARKER_APPLY: {
            KxStringBuffer_AppendCstr(result->string, "%");
            break;
        }
        case KN_SYNTAX_MARKER_EQUAL: {
            KxStringBuffer_AppendCstr(result->string, "=");
            break;
        }
        case KN_SYNTAX_MARKER_ASSIGN: {
            KxStringBuffer_AppendCstr(result->string, ":=");
            break;
        }
        case KN_SYNTAX_MARKER_MSG_SIGNAL: {
            KxStringBuffer_AppendCstr(result->string, ".");
            break;
        }
        case KN_SYNTAX_MARKER_GET_LVALUE: {
            KxStringBuffer_AppendCstr(result->string, "\\:");
            break;
        }
        case KN_SYNTAX_MARKER_GET_RVALUE: {
            KxStringBuffer_AppendCstr(result->string, "\\");
            break;
        }
        case KN_SYNTAX_MARKER_PROC_PIPE: {
            KxStringBuffer_AppendCstr(result->string, "|");
            break;
        }
        case KN_SYNTAX_MARKER_CLAUSE_END: {
            KxStringBuffer_AppendCstr(result->string, ";");
            break;
        }
    }
    return KON_2_KN(result);
}

KN KN_QuoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonQuoteType type = KN_FIELD(source, Quote, type);
    KN inner = KN_FIELD(source, Quote, inner);
    KN name = KN_FIELD(source, Quote, name);
    const char* nameCstr = KN_UNBOX_STR(name);

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->string, "$");
    KxStringBuffer_AppendCstr(result->string, nameCstr);
    KxStringBuffer_AppendCstr(result->string, ".");

    KN innerToKonStr = KN_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerToKonStr));

    return KON_2_KN(result);
}

KN KN_QuasiquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonQuasiquoteType type = KN_FIELD(source, Quasiquote, type);
    KN inner = KN_FIELD(source, Quasiquote, inner);
    KN name = KN_FIELD(source, Quote, name);
    const char* nameCstr = KN_UNBOX_STR(name);

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->string, "@");
    KxStringBuffer_AppendCstr(result->string, nameCstr);
    KxStringBuffer_AppendCstr(result->string, ".");

    KN innerToKonStr = KN_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerToKonStr));

    return KON_2_KN(result);
}

KN KN_UnquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonUnquoteType type = KN_FIELD(source, Unquote, type);
    KN inner = KN_FIELD(source, Unquote, inner);

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->string, "$");

    switch (type) {
        case KN_UNQUOTE_SEQ: {
            KxStringBuffer_AppendCstr(result->string, "~");
            break;
        }
        case KN_UNQUOTE_KV: {
            KxStringBuffer_AppendCstr(result->string, "%");
            break;
        }
        case KN_UNQUOTE_REPLACE: {
            break;
        }
    }

    KxStringBuffer_AppendCstr(result->string, ".");

    KN innerToKonStr = KN_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerToKonStr));

    return KON_2_KN(result);
}



KN KN_PrefixStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KN inner = KN_UNBOX_PREFIX(source);
    printf("KN_PrefixStringify\n");

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->string, "!");

    KN innerToKonStr = KN_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerToKonStr));

    return KON_2_KN(result);
}

KN KN_SuffixStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KN inner = KN_UNBOX_SUFFIX(source);

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->string, "~");

    KN innerToKonStr = KN_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerToKonStr));

    return KON_2_KN(result);
}

KN KN_TxtMarcroStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KN inner = KN_UNBOX_TXT_MARCRO(source);

    KN name = KN_FIELD(source, TxtMarcro, name);
    const char* nameCstr = KN_UNBOX_STR(name);

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->string, "^");
    KxStringBuffer_AppendCstr(result->string, nameCstr);
    KxStringBuffer_AppendCstr(result->string, ".");

    KN innerToKonStr = KN_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerToKonStr));

    return KON_2_KN(result);
}

KN KN_ObjBuilderStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KN inner = KN_UNBOX_OBJ_BUILDER(source);

    KN name = KN_FIELD(source, ObjBuilder, name);
    const char* nameCstr = KN_UNBOX_STR(name);

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->string, "#");
    if (strlen(nameCstr) > 0) {
        KxStringBuffer_AppendCstr(result->string, nameCstr);
        KxStringBuffer_AppendCstr(result->string, ".");
    }

    KN innerToKonStr = KN_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerToKonStr));

    return KON_2_KN(result);
}

////
// the parent node add the first left padding
// don't add newline when stringify sub container types.
// add newline in parent node


KN KN_VectorStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    KxVector* items = KN_FIELD(source, Vector, vector);

    int vecLen = KxVector_Length(items);
    
    if (newLine) {
        KxStringBuffer_AppendCstr(result->string, "<\n");

        
        for (int i = 0; i < vecLen; i++) {
            KN item = (KN)KxVector_AtIndex(items, i);
            KN itemToKonStr = KN_ToFormatString(kstate, item, true, depth + 1, padding);

            AddLeftPadding(result->string, depth, padding);
            KxStringBuffer_AppendCstr(result->string, padding);
            KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(itemToKonStr));
            KxStringBuffer_AppendCstr(result->string, "\n");
        }

        AddLeftPadding(result->string, depth, padding);
        KxStringBuffer_AppendCstr(result->string, ">");
    }
    else {
        KxStringBuffer_AppendCstr(result->string, "<");
        
        for (int i = 0; i < vecLen; i++) {
            KN item = (KN)KxVector_AtIndex(items, i);
            
            KN itemToKonStr = KN_ToFormatString(kstate, item, false, depth + 1, padding);
            KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(itemToKonStr));

            // if (i != vecLen - 1) {
                KxStringBuffer_AppendCstr(result->string, " ");
            // }
            
        }

        KxStringBuffer_AppendCstr(result->string, ">");
    }

    return KON_2_KN(result);
}

bool KN_IsPairList(KN source)
{
    KN iter = source;
    bool isList = true;
    while (iter.asU64 != KNBOX_NIL) {
        if (!KN_IS_PAIR(iter)) {
            isList = false;
            break;
        }
        iter = KN_CDR(iter);
    }
    return isList;
}

bool KN_IsBlock(KN source)
{
    KN iter = source;
    bool isList = true;
    while (iter.asU64 != KNBOX_NIL) {
        if (!KN_IS_PAIR(iter) && !KN_IS_BLOCK(iter)) {
            isList = false;
            break;
        }
        iter = KN_CDR(iter);
    }
    return isList;
}

KN KN_PairListStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    assert(KN_IsPairList(source));
    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    if (source.asU64 == KNBOX_NIL) {
        KxStringBuffer_AppendCstr(result->string, "nil");
        return KON_2_KN(result);
    }
    
    if (newLine) {
        KxStringBuffer_AppendCstr(result->string, "{\n");
        if (KN_IS_PAIR(source)) {
            KN iter = source;

            while (iter.asU64 != KNBOX_NIL && KN_IS_PAIR(iter)) {
                KN item = KN_CAR(iter);
                KN next = KN_CDR(iter);

                KN itemToKonStr = KN_ToFormatString(kstate, item, true, depth + 1, padding);
                
                AddLeftPadding(result->string, depth, padding);
                KxStringBuffer_AppendCstr(result->string, padding);
                KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(itemToKonStr));
                KxStringBuffer_AppendCstr(result->string, "\n");

                iter = next;
            }
        }

        AddLeftPadding(result->string, depth, padding);
        KxStringBuffer_AppendCstr(result->string, "}");
    }
    else {
        KxStringBuffer_AppendCstr(result->string, "{");
        
        if (KN_IS_PAIR(source)) {
            KN iter = source;

            while (iter.asU64 != KNBOX_NIL && KN_IS_PAIR(iter)) {
                KN item = KN_CAR(iter);
                KN next = KN_CDR(iter);
                
                KN itemToKonStr = KN_ToFormatString(kstate, item, false, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(itemToKonStr));
                // if (next != KN_NIL) {
                    KxStringBuffer_AppendCstr(result->string, " ");
                // }
                iter = next;
            }
        }

        KxStringBuffer_AppendCstr(result->string, "}");
    }

    return KON_2_KN(result);
}


KN KN_PairListRevert(KonState* kstate, KN source)
{
    KN result = KN_NIL;
    if (source.asU64 != KNBOX_NIL && KN_IS_PAIR(source)) {
        KN iter = source;
        while (iter.asU64 != KNBOX_NIL && KN_IS_PAIR(iter)) {
            KN item = KN_CAR(iter);
            KN next = KN_CDR(iter);

            result = KN_CONS(kstate, item, result);

            iter = next;
        }
    }
    return result;
}

KN KN_PairListLength(KonState* kstate, KN source)
{
    int length = 0;
    if (source.asU64 != KNBOX_NIL && KN_IS_PAIR(source)) {
        KN iter = source;
        while (iter.asU64 != KNBOX_NIL && KN_IS_PAIR(iter)) {
            KN item = KN_CAR(iter);
            KN next = KN_CDR(iter);

            length += 1;

            iter = next;
        }
    }
    return KN_MAKE_FIXNUM(length);
}


KN KN_Cons(KonState* kstate, KN head, KN tail)
{
  KonPair* node = KN_ALLOC_TYPE_TAG(kstate, KonPair, KN_T_PAIR);
  node->prev = KN_NIL;
  KN result = KON_2_KN(node);
  if (KN_IS_PAIR(tail)) {
      KN_FIELD(tail, Pair, prev) = KON_2_KN(node);
  }
  KN_CAR(result) = head;
  KN_CDR(result) = tail;
  return result;
}

KN KN_PairList2(KonState* kstate, KN a, KN b)
{
  KN res = KN_CONS(kstate, b, KN_NIL);
  res = KN_CONS(kstate, a, res);
  return res;
}

KN KN_PairList3(KonState* kstate, KN a, KN b, KN c)
{
  KN res = KN_PairList2(kstate, b, c);
  res = KN_CONS(kstate, a, res);
  return res;
}


KN KN_TableStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    KxHashTable* hashTable = KN_FIELD(source, Table, table);
    KxHashTableIter iter = KxHashTable_IterHead(hashTable);

    if (newLine) {
        KxStringBuffer_AppendCstr(result->string, "(\n");


        while (iter != KNBOX_NIL) {
            KxHashTableIter next = KxHashTable_IterNext(hashTable, iter);
            const char* itemKey = KxHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KxHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KN_ToFormatString(kstate, itemValue, true, depth + 1, padding);
            if (itemKey != NULL) {
                AddLeftPadding(result->string, depth, padding);
                KxStringBuffer_AppendCstr(result->string, ":'");
                KxStringBuffer_AppendCstr(result->string, itemKey);
                KxStringBuffer_AppendCstr(result->string, "'=\n");
            }

            AddLeftPadding(result->string, depth, padding);
            KxStringBuffer_AppendCstr(result->string, padding);
            KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(itemToKonStr));
            KxStringBuffer_AppendCstr(result->string, "\n");

            iter = next;
        }

        AddLeftPadding(result->string, depth, padding);
        KxStringBuffer_AppendCstr(result->string, ")");
    }
    else {
        KxStringBuffer_AppendCstr(result->string, "(");

        while (iter != KNBOX_NIL) {
            KxHashTableIter next = KxHashTable_IterNext(hashTable, iter);
            const char* itemKey = KxHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KxHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KN_ToFormatString(kstate, itemValue, false, depth + 1, padding);

            if (itemKey != NULL) {
                KxStringBuffer_AppendCstr(result->string, ":'");
                KxStringBuffer_AppendCstr(result->string, itemKey);
                KxStringBuffer_AppendCstr(result->string, "'= ");
            }

            KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(itemToKonStr));
            
            // if (next != KN_NIL) {
                KxStringBuffer_AppendCstr(result->string, " ");
            // }

            iter = next;
        }
        KxStringBuffer_AppendCstr(result->string, ")");
    }

    return KON_2_KN(result);
}

KN KN_ParamTableToList(KonState* kstate, KN source)
{
    KxHashTable* hashTable = KN_FIELD(source, Param, table);
    KxHashTableIter iter = KxHashTable_IterHead(hashTable);
    KN result = KN_NIL;
    while (iter != KNBOX_NIL) {
        KxHashTableIter next = KxHashTable_IterNext(hashTable, iter);
        const char* itemKey = KxHashTable_IterGetKey(hashTable, iter);
        KN itemValue = (KN)KxHashTable_IterGetVal(hashTable, iter);
        result = KN_CONS(kstate, itemValue, result);
        iter = next;
    }
    return KN_PairListRevert(kstate, result);
}

KN KN_CellStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    KonCell* head = KN_2_KON(source, Cell);
    KonCell* iter = head;

    if (newLine) {
        KxStringBuffer_AppendCstr(result->string, "[");

        while ((void*)iter != KNBOX_NIL) {
            KN core = iter->core;
            KonMap* innerMap = iter->map;
            KonTable* innerTable = iter->table;
            KonPair* innerList = iter->list;
            KonVector* innerVector = iter->vector;
            KonSuffix* innerSuffix = iter->suffix;
            
            
            if (core.asU64 != KNBOX_UNDEF) {
                KN coreToKonStr = KN_ToFormatString(kstate, core, true, depth + 1, padding);
                if (iter != head) {
                    KxStringBuffer_AppendCstr(result->string, "\n");
                    AddLeftPadding(result->string, depth, padding);
                    KxStringBuffer_AppendCstr(result->string, padding);
                }
                KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(coreToKonStr));
            }

            if (innerMap != KNBOX_UNDEF) {
                KxHashTable* unboxedMap = innerMap->map;
                KxHashTableIter mapIter = KxHashTable_IterHead(unboxedMap);
                while (mapIter != KNBOX_NIL) {
                    KxHashTableIter mapIterNext = KxHashTable_IterNext(unboxedMap, mapIter);
                    const char* mapKey = KxHashTable_IterGetKey(unboxedMap, mapIter);
                    KN mapValue = (KN)KxHashTable_IterGetVal(unboxedMap, mapIter);
                    KxStringBuffer_AppendCstr(result->string, "\n");
                    AddLeftPadding(result->string, depth, padding);
                    KxStringBuffer_AppendCstr(result->string, padding);

                    KxStringBuffer_AppendCstr(result->string, ":'");
                    KxStringBuffer_AppendCstr(result->string, mapKey);
                    KxStringBuffer_AppendCstr(result->string, "'");

                    if (mapValue.asU64 != NULL && mapValue.asU64 != KNBOX_UKN) {
                        KN mapValKonStr = KN_ToFormatString(kstate, mapValue, true, depth + 1, padding);
                        KxStringBuffer_AppendCstr(result->string, "= ");
                        KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(mapValKonStr));
                    }
                    mapIter = mapIterNext;
                }
            }

            if (innerSuffix != KNBOX_UNDEF) {
                KxStringBuffer_AppendCstr(result->string, "\n");
                AddLeftPadding(result->string, depth, padding);
                KxStringBuffer_AppendCstr(result->string, padding);

                KN innerSuffixToKonStr = KN_SuffixStringify(kstate, KON_2_KN(innerSuffix), true, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerSuffixToKonStr));
            }

            if (innerVector != KNBOX_UNDEF) {
                KxStringBuffer_AppendCstr(result->string, "\n");
                AddLeftPadding(result->string, depth, padding);
                KxStringBuffer_AppendCstr(result->string, padding);

                KN innerVectorToKonStr = KN_VectorStringify(kstate, KON_2_KN(innerVector), true, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerVectorToKonStr));
            }
            
            if (innerTable != KNBOX_UNDEF) {
                KxStringBuffer_AppendCstr(result->string, "\n");
                AddLeftPadding(result->string, depth, padding);
                KxStringBuffer_AppendCstr(result->string, padding);

                KN innerTableToKonStr = KN_TableStringify(kstate, KON_2_KN(innerTable), true, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerTableToKonStr));
            }

            if (innerList != KNBOX_UNDEF) {
                 KxStringBuffer_AppendCstr(result->string, "\n");
                 AddLeftPadding(result->string, depth, padding);
                 KxStringBuffer_AppendCstr(result->string, padding);

                 KN innerListToKonStr = KN_PairListStringify(kstate, KON_2_KN(innerList), true, depth + 1, padding);
                 KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerListToKonStr));
            }

            iter = iter->next;
        }

        if (head != KNBOX_NIL
            || head->table != KNBOX_UNDEF
            || head->list != KNBOX_UNDEF
            || head->next != KNBOX_NIL
        ) {
            KxStringBuffer_AppendCstr(result->string, "\n");
            AddLeftPadding(result->string, depth, padding);
        }

        KxStringBuffer_AppendCstr(result->string, "]");
    }
    else {
        KxStringBuffer_AppendCstr(result->string, "[");

        while (iter != KNBOX_NIL) {
            KN core = iter->core;
            KonMap* innerMap = iter->map;
            KonTable* innerTable = iter->table;
            KonPair* innerList = iter->list;
            KonVector* innerVector = iter->vector;
            KonSuffix* innerSuffix = iter->suffix;

            if (core.asU64 != KNBOX_UNDEF) {
                KN coreToKonStr = KN_ToFormatString(kstate, core, false, 0, "  ");
                if (iter != head) {
                    KxStringBuffer_AppendCstr(result->string, " ");
                }
                KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(coreToKonStr));
            }

            if (innerMap != KNBOX_UNDEF) {
                KxHashTable* unboxedMap = innerMap->map;

                KxHashTableIter mapIter = KxHashTable_IterHead(unboxedMap);
                while (mapIter != KNBOX_NIL) {
                    KxHashTableIter mapIterNext = KxHashTable_IterNext(unboxedMap, mapIter);
                    const char* mapKey = KxHashTable_IterGetKey(unboxedMap, mapIter);
                    KN mapValue = (KN)KxHashTable_IterGetVal(unboxedMap, mapIter);
                    KxStringBuffer_AppendCstr(result->string, " ");

                    KxStringBuffer_AppendCstr(result->string, ":'");
                    KxStringBuffer_AppendCstr(result->string, mapKey);
                    KxStringBuffer_AppendCstr(result->string, "'");

                    if (mapValue.asU64 != NULL && mapValue.asU64 != KNBOX_UKN) {
                        KN mapValKonStr = KN_ToFormatString(kstate, mapValue, false, 0, "  ");
                        KxStringBuffer_AppendCstr(result->string, "= ");
                        KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(mapValKonStr));
                    }
                    mapIter = mapIterNext;
                }
            }
            
            if (innerSuffix != KNBOX_UNDEF) {
                KxStringBuffer_AppendCstr(result->string, " ");
                AddLeftPadding(result->string, depth, padding);
                KxStringBuffer_AppendCstr(result->string, padding);

                KN innerSuffixToKonStr = KN_SuffixStringify(kstate, KON_2_KN(innerSuffix), false, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerSuffixToKonStr));
            }

            if (innerVector != KNBOX_UNDEF) {
                KxStringBuffer_AppendCstr(result->string, " ");
                AddLeftPadding(result->string, depth, padding);
                KxStringBuffer_AppendCstr(result->string, padding);

                KN innerVectorToKonStr = KN_VectorStringify(kstate, KON_2_KN(innerVector), false, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerVectorToKonStr));
            }
            
            if (innerTable != KNBOX_UNDEF) {
                KxStringBuffer_AppendCstr(result->string, " ");
                KN innerTableToKonStr = KN_TableStringify(kstate, KON_2_KN(innerTable), false, 0, "  ");
                KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerTableToKonStr));
            }

            if (innerList != KNBOX_UNDEF) {
                KxStringBuffer_AppendCstr(result->string, " ");
                KN innerListToKonStr = KN_PairListStringify(kstate, KON_2_KN(innerList), true, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(innerListToKonStr));
            }

            iter = iter->next;
        }
        KxStringBuffer_AppendCstr(result->string, "]");
    }

    return KON_2_KN(result);
}

KN KN_CellCoresToList(KonState* kstate, KN source)
{
    KonCell* head = KN_2_KON(source, Cell);
    KonCell* iter = head;
    KN result = KN_NIL;
    while (iter != KNBOX_NIL) {
        KN core = iter->core;
        result = KN_CONS(kstate, core, result);
        iter = iter->next;
    }
    return KN_PairListRevert(kstate, result);
}


KN KN_AccessorStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->string = KxStringBuffer_New();

    KonAccessor* accessor = KN_2_KON(source, Accessor);
    if (!accessor->isDir) {
        KN value = accessor->value;
        // get accessor inner value
        while (KN_IS_ACCESSOR(value)
            && !KN_2_KON(value, Accessor)->isDir
        ) {
            value = KN_2_KON(value, Accessor)->value;
        }
        return KN_ToFormatString(kstate, value, true, depth, padding);
    }
    KxHashTable* hashTable = accessor->dir;
    KxHashTableIter iter = KxHashTable_IterHead(hashTable);

    if (newLine) {
        KxStringBuffer_AppendCstr(result->string, "#accesor.(\n");

        while (iter != KNBOX_NIL) {
            KxHashTableIter next = KxHashTable_IterNext(hashTable, iter);
            const char* itemKey = KxHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KxHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KN_ToFormatString(kstate, itemValue, true, depth + 1, padding);

            AddLeftPadding(result->string, depth, padding);
            KxStringBuffer_AppendCstr(result->string, ":'");
            KxStringBuffer_AppendCstr(result->string, itemKey);
            KxStringBuffer_AppendCstr(result->string, "'=\n");

            AddLeftPadding(result->string, depth, padding);
            KxStringBuffer_AppendCstr(result->string, padding);
            KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(itemToKonStr));
            KxStringBuffer_AppendCstr(result->string, "\n");

            iter = next;
        }

        AddLeftPadding(result->string, depth, padding);
        KxStringBuffer_AppendCstr(result->string, ")");
    }
    else {
        KxStringBuffer_AppendCstr(result->string, "#accesor.(");

        while (iter != KNBOX_NIL) {
            KxHashTableIter next = KxHashTable_IterNext(hashTable, iter);
            const char* itemKey = KxHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KxHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KN_ToFormatString(kstate, itemValue, false, depth + 1, padding);

            KxStringBuffer_AppendCstr(result->string, ":'");
            KxStringBuffer_AppendCstr(result->string, itemKey);
            KxStringBuffer_AppendCstr(result->string, "'= ");

            KxStringBuffer_AppendStringBuffer(result->string, KN_UNBOX_STRING(itemToKonStr));
            
            // if (next != KN_NIL) {
                KxStringBuffer_AppendCstr(result->string, " ");
            // }

            iter = next;
        }
        KxStringBuffer_AppendCstr(result->string, ")");
    }

    return KON_2_KN(result);
}


KN MakeNativeProcedure(KonState* kstate, KonProcedureType type, KonNativeFuncRef funcRef, int paramNum, int hasVAList, int hasVAMap)
{
    KonProcedure* result = KN_ALLOC_TYPE_TAG(kstate, KonProcedure, KN_T_PROCEDURE);
    result->type = type;
    result->nativeFuncRef = funcRef;
    result->paramNum = paramNum;
    result->hasVAList = hasVAList != 0 ? 1 : 0;
    result->hasVAMap = hasVAMap != 0 ? 1 : 0;
    return KON_2_KN(result);
}

KonProcedure* MakeDispatchProc(KonState* kstate, KN procAst, KonEnv* env)
{
    if (procAst.asU64 == KNBOX_NIL || procAst.asU64 == KNBOX_UNDEF|| procAst.asU64 == KNBOX_UKN) {
        return NULL;
    }
    KonProcedure* proc = KN_ALLOC_TYPE_TAG(kstate, KonProcedure, KN_T_PROCEDURE);
    
    
    KN param = (KN)KN_DTR(procAst);
    KN funcName = KN_UNDEF;
    KN body = KN_NIL;
    
    if (param.asU64 == KNBOX_UNDEF) {
        funcName = KN_DCNR(procAst);
        param = (KN)KN_DTNR(procAst);
        body = (KN)KN_DLNR(procAst);
    }
    else {
        body = (KN)KN_DLR(procAst);
    }
    
    param = KN_ParamTableToList(kstate, param);
    
    proc->type = KN_COMPOSITE_LAMBDA;
    proc->composite.lexicalEnv = env;
    proc->composite.argList = param;
    proc->composite.body = body;
    proc->composite.captureList = KN_UNDEF;
    return proc;
}


KonMsgDispatcher* MakeMsgDispatcher(KonState* kstate)
{
    KonMsgDispatcher* result = KN_ALLOC_TYPE_TAG(kstate, KonMsgDispatcher, KN_T_MSG_DISPATCHER);
    result->onSymbol = (KonProcedure*)KNBOX_UNDEF;
    result->onSyntaxMarker = (KonProcedure*)KNBOX_UNDEF;
    result->onMethodCall = (KonProcedure*)KNBOX_UNDEF;
    result->onVisitList = (KonProcedure*)KNBOX_UNDEF;
    result->onVisitVector = (KonProcedure*)KNBOX_UNDEF;
    result->onVisitTable = (KonProcedure*)KNBOX_UNDEF;
    result->onVisitCell = (KonProcedure*)KNBOX_UNDEF;
    return result;
}

int KN_SetMsgDispatcher(KonState* kstate, unsigned int dispatcherId, KonMsgDispatcher* dispatcher)
{
    KxVector_SetIndex(kstate->msgDispatchers, dispatcherId, dispatcher);
    return 1;
}

unsigned int KN_SetNextMsgDispatcher(KonState* kstate, KonMsgDispatcher* dispatcher)
{
    int dispacherId = kstate->nextMsgDispatcherId;
    KN_SetMsgDispatcher(kstate, dispacherId, dispatcher);
    kstate->nextMsgDispatcherId += 1;
    return dispacherId;
}

KonMsgDispatcher* KN_GetMsgDispatcher(KonState* kstate, unsigned int dispatcherId)
{
    KN dispatcher = (KN)KxVector_AtIndex(kstate->msgDispatchers, dispatcherId);
    if (dispatcher.asU64 == KNBOX_UKN || dispatcher.asU64 == NULL) {
        return NULL;
    }
    else {
        return KN_2_KON(dispatcher, MsgDispatcher);
    }
}

KonAccessor* KN_InitAccessorWithMod(KonState* kstate, char* mod)
{
    KonAccessor* result = KN_ALLOC_TYPE_TAG(kstate, KonAccessor, KN_T_ACCESSOR);
    result->isDir = false;
    result->openToRef = false;
    result->openToChildren = false;
    result->openToSibling = false;
    result->canWrite = false;
    // result->CanExec = false;
    // result->IsMethod = false;
    result->value = KN_UNDEF;
    result->dir = NULL;
    result->setter = NULL;
    // set mod
    if (mod != NULL) {
        if (strchr(mod, 'd')) {
            result->isDir = true;
        }
        if (strchr(mod, 'r')) {
            result->openToRef = true;
        }
        if (strchr(mod, 'w')) {
            result->canWrite = true;
        }
        else if (strchr(mod, 'x')) {
            // result->CanExec = true;
        }
        else if (strchr(mod, 'm')) {
            // result->IsMethod = true;
        }
    }
    return result;
}

KN KN_MakePropertyAccessor(KonState* kstate, KN value, char* mod, KonProcedure* setter)
{
    KonAccessor* result = KN_InitAccessorWithMod(kstate, mod);
    result->isDir = false;
    result->value = value;
    if (setter != NULL) {
        result->setter = setter;
    }
    return KON_2_KN(result);
}

KN KN_MakeDirAccessor(KonState* kstate, char* mod, KonProcedure* setter)
{
    KonAccessor* result = KN_InitAccessorWithMod(kstate, mod);
    result->isDir = true;
    result->value = KN_NIL;
    result->dir = KxHashTable_Init(4);
    if (setter != NULL) {
        result->setter = setter;
    }
    return KON_2_KN(result);
}

bool KN_DirAccessorPutKeyProperty(KonState* kstate, KN dir, char* key, KN property)
{
    if (!(KN_IS_ACCESSOR(dir) && KN_FIELD(dir, Accessor, isDir) == true)
        || !KN_IS_ACCESSOR(property)) {
        return false;
    }

    KxHashTable_PutKv(
        KN_FIELD(dir, Accessor, dir),
        key,
        property.asU64
    );
    return true;
}

bool KN_DirAccessorPutKeyValue(KonState* kstate, KN dir, char* key, KN value, char* mod, KonProcedure* setter)
{
    return KN_DirAccessorPutKeyProperty(kstate, dir,
        key,
        KN_MakePropertyAccessor(kstate,
            value,
            mod,
            setter
        )
    );
}



KN KN_VectorToKonPairList(KonState* kstate, KxVector* vector)
{
    KN list = KN_NIL;

    int len = KxVector_Length(vector);
    for (int i = len - 1; i >= 0; i--) {
        KN item = (KN)KxVector_AtIndex(vector, i);
        list = KN_CONS(kstate, item, list);
    }

    return list;
}
