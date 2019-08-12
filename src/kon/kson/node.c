#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include "node.h"
#include "../utils/number_utils.h"

#include <pwd.h>
#include <sys/types.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdint.h>


KxStringBuffer* KON_ReadFileContent(const char* filePathOrigin)
{
    int originPathStrLen = strlen(filePathOrigin);
    if ((originPathStrLen + 1) > PATH_MAX) {
        printf("file path exceeded");
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
    
    char* absoluteFilePath = (char*)tb_nalloc0(PATH_MAX, sizeof(char));
    char *realpathRes = realpath(replaceHomePath, absoluteFilePath);
    
    tb_free(replaceHomePath);
    if (realpathRes == NULL) {
        printf("abs file path exceeded");
        exit(3);
    }

    // printf("absoluteFilePath %s\n", absoluteFilePath);


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


const char* KON_HumanFormatTime()
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


KN KON_AllocTagged(KonState* kstate, size_t size, kon_uint_t tag)
{
    KN res = (KN)tb_allocator_malloc0(kstate->Allocator, size);

    // TODO add to heap ptr store

    if (res && ! KON_IS_EXCEPTION(res)) {
        ((KonBase*)res)->Tag = tag;
        // set dispatcher id
        if (tag == KON_T_FIXNUM || tag == KON_T_FLONUM || tag == KON_T_BIGNUM) {
            ((KonBase*)res)->MsgDispatcherId = KON_MAKE_FIXNUM(KON_T_NUMBER);
        }
        else if (tag == KON_T_NIL || tag == KON_T_PAIR) {
            ((KonBase*)res)->MsgDispatcherId = KON_MAKE_FIXNUM(KON_T_PAIRLIST);
        }
        else {
            ((KonBase*)res)->MsgDispatcherId = KON_MAKE_FIXNUM(tag);
        }
        
    }

    return res;
}


KN KON_ToFormatString(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    if (KON_IS_FIXNUM(source)) {
        return KON_FixnumStringify(kstate, source);
    }
    else if (KON_IS_FLONUM(source)) {
        return KON_FlonumStringify(kstate, source);
    }
    else if (KON_IS_CHAR(source)) {
        return KON_CharStringify(kstate, source);
    }
    else if (KON_IS_STRING(source)) {
        return KON_StringStringify(kstate, source);
    }
    else if (KON_IS_SYMBOL(source)) {
        return KON_SymbolStringify(kstate, source);
    }
    else if (KON_IS_SYNTAX_MARKER(source)) {
        return KON_SyntaxMarkerStringify(kstate, source);
    }
    else if (KON_IS_QUOTE(source)) {
        return KON_QuoteStringify(kstate, source, newLine, depth, padding);
    }
    else if (KON_IS_QUASIQUOTE(source)) {
        return KON_QuasiquoteStringify(kstate, source, newLine, depth, padding);
    }
    else if (KON_IS_EXPAND(source)) {
        return KON_ExpandStringify(kstate, source, newLine, depth, padding);
    }
    else if (KON_IS_UNQUOTE(source)) {
        return KON_UnquoteStringify(kstate, source, newLine, depth, padding);
    }
    else if (KON_IS_VECTOR(source)) {
        return KON_VectorStringify(kstate, source, newLine, depth, padding);
    }
    else if (KON_IsPairList(source)) {
        return KON_PairListStringify(kstate, source, newLine, depth, padding);
    }
    else if (KON_IS_TABLE(source)) {
        return KON_TableStringify(kstate, source, newLine, depth, padding);
    }
    else if (KON_IS_CELL(source)) {
        return KON_CellStringify(kstate, source, newLine, depth, padding);
    }
    else if (source == KON_UKN) {
        return KON_MakeString(kstate, "#ukn;");
    }
    else if (source == KON_TRUE) {
        return KON_MakeString(kstate, "#t;");
    }
    else if (source == KON_FALSE) {
        return KON_MakeString(kstate, "#f;");
    }
    else if (source == KON_UNDEF) {
        return KON_MakeString(kstate, "#undef;");
    }
    else if (KON_IS_CONTINUATION(source)) {
        return KON_MakeString(kstate, "<continuation>");
    }
    else if (KON_IS_PROCEDURE(source)) {
        return KON_MakeString(kstate, "<procedure>");
    }
    // TODO other data types
    else {
        return KON_MakeEmptyString(kstate);
    }
}

KN KON_Stringify(KonState* kstate, KN source)
{
    return KON_ToFormatString(kstate, source, false, 0, " ");
}

bool IsContainerOrWrapper()
{

}

// number related
KN KON_FixnumStringify(KonState* kstate, KN source)
{
    if (!KON_IS_FIXNUM(source)) {
        return KON_MakeEmptyString(kstate);
    }
    char buf[128] = { '\0' };
    kon_int_t num = KON_UNBOX_FIXNUM(source);
    itoa(num, buf, 10);

    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->String, buf);

    return value;
}

KN KON_MakeFlonum(KonState* kstate, double f)
{
  return KON_MAKE_FLONUM(kstate, f);
}

KN KON_FlonumStringify(KonState* kstate, KN source)
{
    if (!KON_IS_FLONUM(source)) {
        return KON_MakeEmptyString(kstate);
    }
    char buf[128] = {'\0'};
    double num = KON_FLONUM_VALUE(source);
    double_to_str(num, 2, buf);

    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->String, buf);
    return value;
}

// 转换char为 #c,A;
KN KON_CharStringify(KonState* kstate, KN source)
{
    if (!KON_IS_CHAR(source)) {
        return KON_MakeEmptyString(kstate);
    }

    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->String, "#c,");
    int charcode = KON_UNBOX_CHAR(source);
    char buf[10] = {'\0'};
    snprintf(buf, 10, "%c", charcode);
    KxStringBuffer_AppendCstr(value->String, buf);
    KxStringBuffer_AppendCstr(value->String, ";");
    return value;
}

KN KON_StringStringify(KonState* kstate, KN source)
{
    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->String, "\"");
    KxStringBuffer_AppendStringBuffer(value->String, KON_UNBOX_STRING(source));
    KxStringBuffer_AppendCstr(value->String, "\"");
    return value;
}

KN KON_MakeString(KonState* kstate, const char* str)
{
    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->String, str);
    return value;
}

KN KON_MakeEmptyString(KonState* kstate)
{
    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KxStringBuffer_New();
    return value;
}

const char* KON_StringToCstr(KN str)
{
    if (!KON_IS_STRING(str)) {
        return NULL;
    }
    return KxStringBuffer_Cstr(KON_UNBOX_STRING(str));
}

void AddLeftPadding(KxStringBuffer* str, int depth, char* padding)
{
    for (int i = 0; i < depth; i++) {
        KxStringBuffer_AppendCstr(str, padding);
    }
}

KN KON_SymbolStringify(KonState* kstate, KN source)
{
    KonSymbolType type = CAST_Kon(Symbol, source)->Type;
    const char* data = KON_UNBOX_SYMBOL(source);

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KxStringBuffer_New();

    switch (type) {
        case KON_SYM_PREFIX_WORD: {
            KxStringBuffer_AppendCstr(result->String, "!");
            KxStringBuffer_AppendCstr(result->String, data);
            break;
        }
        case KON_SYM_SUFFIX_WORD: {
            KxStringBuffer_AppendCstr(result->String, "^");
            KxStringBuffer_AppendCstr(result->String, data);
            break;
        }

        case KON_SYM_WORD: {
            KxStringBuffer_AppendCstr(result->String, data);
            break;
        }
        case KON_SYM_VARIABLE: {
            KxStringBuffer_AppendCstr(result->String, "@");
            KxStringBuffer_AppendCstr(result->String, data);
            break;
        }
        case KON_SYM_IDENTIFIER: {
            KxStringBuffer_AppendCstr(result->String, "$");
            KxStringBuffer_AppendCstr(result->String, data);
            break;
        }
        case KON_SYM_STRING: {
            KxStringBuffer_AppendCstr(result->String, "$\'");
            KxStringBuffer_AppendCstr(result->String, data);
            KxStringBuffer_AppendCstr(result->String, "\'");
            break;
        }
        case KON_QUERY_PATH: {
            KxStringBuffer_AppendCstr(result->String, "/");
            KxStringBuffer_AppendCstr(result->String, data);
            break;
        }
        case KON_MSG_SIGNAL: {
            KxStringBuffer_AppendCstr(result->String, ".");
            KxStringBuffer_AppendCstr(result->String, data);
            break;
        }
    }
    return result;
}

const char* KON_SymbolToCstr(KN sym)
{
    return KON_UNBOX_SYMBOL(sym);
}

KN KON_SyntaxMarkerStringify(KonState* kstate, KN source)
{
    KonSyntaxMarkerType type = CAST_Kon(SyntaxMarker, source)->Type;

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KxStringBuffer_New();

    switch (type) {
        case KON_SYNTAX_MARKER_APPLY: {
            KxStringBuffer_AppendCstr(result->String, "%");
            break;
        }
        case KON_SYNTAX_MARKER_PROC_PIPE: {
            KxStringBuffer_AppendCstr(result->String, "|");
            break;
        }
        case KON_SYNTAX_MARKER_CLAUSE_END: {
            KxStringBuffer_AppendCstr(result->String, ";");
            break;
        }
    }
    return result;
}

KN KON_QuoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonQuoteType type = CAST_Kon(Quote, source)->Type;
    KN inner = CAST_Kon(Quote, source)->Inner;

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->String, "$");

    KN innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerToKonStr));

    return result;
}

KN KON_QuasiquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonQuasiquoteType type = CAST_Kon(Quasiquote, source)->Type;
    KN inner = CAST_Kon(Quasiquote, source)->Inner;

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->String, "@");

    KN innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerToKonStr));

    return (KN)result;
}

KN KON_ExpandStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonExpandType type = CAST_Kon(Expand, source)->Type;
    KN inner = CAST_Kon(Expand, source)->Inner;

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->String, "$");

    switch (type) {
        case KON_EXPAND_REPLACE: {
            break;
        }
        case KON_EXPAND_KV: {
            KxStringBuffer_AppendCstr(result->String, "%");
            break;
        }
        case KON_EXPAND_SEQ: {
            KxStringBuffer_AppendCstr(result->String, "~");
            break;
        }
    }

    KxStringBuffer_AppendCstr(result->String, ".");

    KN innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerToKonStr));

    return result;
}

KN KON_UnquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonUnquoteType type = CAST_Kon(Unquote, source)->Type;
    KN inner = CAST_Kon(Unquote, source)->Inner;

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->String, "@");

    switch (type) {
        case KON_UNQUOTE_SEQ: {
            KxStringBuffer_AppendCstr(result->String, "~");
            break;
        }
        case KON_UNQUOTE_KV: {
            KxStringBuffer_AppendCstr(result->String, "%");
            break;
        }
    }

    KxStringBuffer_AppendCstr(result->String, ".");

    KN innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerToKonStr));

    return result;
}

////
// the parent node add the first left padding
// don't add newline when stringify sub container types.
// add newline in parent node


KN KON_VectorStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KxStringBuffer_New();

    KxVector* items = CAST_Kon(Vector, source)->Vector;

    int vecLen = KxVector_Length(items);
    
    if (newLine) {
        KxStringBuffer_AppendCstr(result->String, "{\n");

        
        for (int i = 0; i < vecLen; i++) {
            KN item = KxVector_AtIndex(items, i);
            KN itemToKonStr = KON_ToFormatString(kstate, item, true, depth + 1, padding);

            AddLeftPadding(result->String, depth, padding);
            KxStringBuffer_AppendCstr(result->String, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(itemToKonStr));
            KxStringBuffer_AppendCstr(result->String, "\n");
        }

        AddLeftPadding(result->String, depth, padding);
        KxStringBuffer_AppendCstr(result->String, "}");
    }
    else {
        KxStringBuffer_AppendCstr(result->String, "{");
        
        for (int i = 0; i < vecLen; i++) {
            KN item = KxVector_AtIndex(items, i);
            
            KN itemToKonStr = KON_ToFormatString(kstate, item, false, depth + 1, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(itemToKonStr));

            if (i != vecLen - 1) {
                KxStringBuffer_AppendCstr(result->String, " ");
            }
            
        }

        KxStringBuffer_AppendCstr(result->String, "}");
    }

    return result;
}

bool KON_IsPairList(KN source)
{
    KonPair* iter = source;
    bool isList = true;
    do {
        if (iter == KON_NIL) {
            break;
        }
        if (!KON_IS_PAIR(iter)) {
            isList = false;
            break;
        }
        iter = KON_CDR(iter);
    } while (iter != KON_NIL);
    return isList;
}

KN KON_PairListStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    assert(KON_IsPairList(source));
    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KxStringBuffer_New();

    
    if (newLine) {
        KxStringBuffer_AppendCstr(result->String, "[\n");
        if (source != KON_NIL && KON_IS_PAIR(source)) {
            KonPair* iter = source;

            while (iter != KON_NIL) {
                KN item = KON_CAR(iter);
                KN next = KON_CDR(iter);

                KN itemToKonStr = KON_ToFormatString(kstate, item, true, depth + 1, padding);
                
                AddLeftPadding(result->String, depth, padding);
                KxStringBuffer_AppendCstr(result->String, padding);
                KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(itemToKonStr));
                KxStringBuffer_AppendCstr(result->String, "\n");

                iter = next;
            }
        }

        AddLeftPadding(result->String, depth, padding);
        KxStringBuffer_AppendCstr(result->String, "]");
    }
    else {
        KxStringBuffer_AppendCstr(result->String, "[");
        
        if (source != KON_NIL && KON_IS_PAIR(source)) {
            KonPair* iter = source;

            while (iter != KON_NIL) {
                KN item = KON_CAR(iter);
                KN next = KON_CDR(iter);
                
                KN itemToKonStr = KON_ToFormatString(kstate, item, false, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(itemToKonStr));
                if (next != KON_NIL) {
                    KxStringBuffer_AppendCstr(result->String, " ");
                }
                iter = next;
            }
        }

        KxStringBuffer_AppendCstr(result->String, "]");
    }

    return result;
}

KN KON_PairListRevert(KonState* kstate, KN source)
{
    KN result = KON_NIL;
    if (source != KON_NIL && KON_IS_PAIR(source)) {
        KonPair* iter = source;
        while (iter != KON_NIL) {
            KN item = KON_CAR(iter);
            KN next = KON_CDR(iter);

            result = KON_CONS(kstate, item, result);

            iter = next;
        }
    }
    return result;
}

KN KON_PairListLength(KonState* kstate, KN source)
{
    int length = 0;
    if (source != KON_NIL && KON_IS_PAIR(source)) {
        KonPair* iter = source;
        while (iter != KON_NIL) {
            KN item = KON_CAR(iter);
            KN next = KON_CDR(iter);

            length += 1;

            iter = next;
        }
    }
    return KON_MAKE_FIXNUM(length);
}


KN KON_Cons(KonState* kstate, KN self, kon_int_t n, KN head, KN tail)
{
  KonPair* node = KON_ALLOC_TYPE_TAG(kstate, KonPair, KON_T_PAIR);
  if (KON_IS_EXCEPTION(node)) {
      return node;
  }
  node->Prev = KON_NIL;
  if (KON_IS_PAIR(tail)) {
      CAST_Kon(Pair, tail)->Prev = node;
  }
  KON_CAR(node) = head;
  KON_CDR(node) = tail;
  return node;
}

KN KON_PairList2(KonState* kstate, KN a, KN b)
{
  KN res = KON_CONS(kstate, b, KON_NIL);
  res = KON_CONS(kstate, a, res);
  return res;
}

KN KON_PairList3(KonState* kstate, KN a, KN b, KN c)
{
  KN res = KON_PairList2(kstate, b, c);
  res = KON_CONS(kstate, a, res);
  return res;
}


KN KON_TableStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KxStringBuffer_New();

    KxHashTable* hashTable = CAST_Kon(Table, source)->Table;
    KxHashTableIter iter = KxHashTable_IterHead(hashTable);

    if (newLine) {
        KxStringBuffer_AppendCstr(result->String, "(\n");


        while (iter != KON_NIL) {
            KxHashTableIter next = KxHashTable_IterNext(hashTable, iter);
            const char* itemKey = KxHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KxHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KON_ToFormatString(kstate, itemValue, true, depth + 1, padding);

            AddLeftPadding(result->String, depth, padding);
            KxStringBuffer_AppendCstr(result->String, ":'");
            KxStringBuffer_AppendCstr(result->String, itemKey);
            KxStringBuffer_AppendCstr(result->String, "'\n");

            AddLeftPadding(result->String, depth, padding);
            KxStringBuffer_AppendCstr(result->String, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(itemToKonStr));
            KxStringBuffer_AppendCstr(result->String, "\n");

            iter = next;
        }

        AddLeftPadding(result->String, depth, padding);
        KxStringBuffer_AppendCstr(result->String, ")");
    }
    else {
        KxStringBuffer_AppendCstr(result->String, "(");

        while (iter != KON_NIL) {
            KxHashTableIter next = KxHashTable_IterNext(hashTable, iter);
            const char* itemKey = KxHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KxHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KON_ToFormatString(kstate, itemValue, false, depth + 1, padding);

            KxStringBuffer_AppendCstr(result->String, ":'");
            KxStringBuffer_AppendCstr(result->String, itemKey);
            KxStringBuffer_AppendCstr(result->String, "' ");

            KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(itemToKonStr));
            
            if (next != NULL) {
                KxStringBuffer_AppendCstr(result->String, " ");
            }

            iter = next;
        }
        KxStringBuffer_AppendCstr(result->String, ")");
    }

    return result;
}



KN KON_CellStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KxStringBuffer_New();

    KN name = CAST_Kon(Cell, source)->Core;
    KonVector* innerVector = CAST_Kon(Cell, source)->Vector;
    KonTable* innerTable = CAST_Kon(Cell, source)->Table;
    KonPair* innerList = CAST_Kon(Cell, source)->List;
    
    if (newLine) {
        KxStringBuffer_AppendCstr(result->String, "<");
        
        if (name != KON_UNDEF) {
            KN nameToKonStr = KON_ToFormatString(kstate, name, true, depth, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(nameToKonStr));
        }
        KxStringBuffer_AppendCstr(result->String, "\n");

        if (innerTable != KON_UNDEF) {
            AddLeftPadding(result->String, depth, padding);
            KxStringBuffer_AppendCstr(result->String, padding);

            KN innerTableToKonStr = KON_ToFormatString(kstate, innerTable, true, depth + 1, padding);

            KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerTableToKonStr));
            KxStringBuffer_AppendCstr(result->String, "\n");
        }
        
        if (innerVector != KON_UNDEF) {
            AddLeftPadding(result->String, depth, padding);
            KxStringBuffer_AppendCstr(result->String, padding);

            KN innerVectorToKonStr = KON_ToFormatString(kstate, innerVector, true, depth + 1, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerVectorToKonStr));
            KxStringBuffer_AppendCstr(result->String, "\n");
        }
        
        if (innerList != KON_UNDEF) {
            AddLeftPadding(result->String, depth, padding);
            KxStringBuffer_AppendCstr(result->String, padding);

            KN innerListToKonStr = KON_ToFormatString(kstate, innerList, true, depth + 1, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerListToKonStr));
            KxStringBuffer_AppendCstr(result->String, "\n");
        }
        

        AddLeftPadding(result->String, depth, padding);
        KxStringBuffer_AppendCstr(result->String, ">");
    }
    else {
        KxStringBuffer_AppendCstr(result->String, "<");

        if (name != KON_UNDEF) {
            KN nameToKonStr = KON_ToFormatString(kstate, name, true, depth, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(nameToKonStr));
        }

        
        if (innerTable != KON_UNDEF) {
            KxStringBuffer_AppendCstr(result->String, " ");
            KN innerTableToKonStr = KON_ToFormatString(kstate, innerTable, false, depth + 1, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerTableToKonStr));
        }

        
        if (innerVector != KON_UNDEF) {
            KxStringBuffer_AppendCstr(result->String, " ");
            KN innerVectorToKonStr = KON_ToFormatString(kstate, innerVector, false, depth + 1, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerVectorToKonStr));
        }

        if (innerList != KON_UNDEF) {
            KxStringBuffer_AppendCstr(result->String, " ");
            KN innerListToKonStr = KON_ToFormatString(kstate, innerList, false, depth + 1, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerListToKonStr));
        }

        KxStringBuffer_AppendCstr(result->String, ">");
    }

    return result;
}

KN MakeNativeProcedure(KonState* kstate, KonProcedureType type, KonNativeFuncRef funcRef)
{
    KonProcedure* result = KON_ALLOC_TYPE_TAG(kstate, KonProcedure, KON_T_PROCEDURE);
    result->Type = type;
    result->NativeFuncRef = funcRef;
    return result;
}

KN MakeMsgDispatcher(KonState* kstate)
{
    KonMsgDispatcher* result = KON_ALLOC_TYPE_TAG(kstate, KonMsgDispatcher, KON_T_MSG_DISPATCHER);
    result->Name = KON_UNDEF;
    result->OnApplyArgs = KON_UNDEF;
    result->OnSelectPath = KON_UNDEF;
    result->OnMethodCall = KON_UNDEF;
    result->OnVisitVector = KON_UNDEF;
    result->OnVisitTable = KON_UNDEF;
    result->OnVisitCell = KON_UNDEF;
    result->Config = KON_UNDEF;
    return result;
}

KN MakeAttrSlotLeaf(KonState* kstate, KN value, char* mod)
{
    KonAttrSlot* result = KON_ALLOC_TYPE_TAG(kstate, KonAttrSlot, KON_T_ATTR_SLOT);
    result->IsDir = false;
    result->OpenToChildren = true;
    result->OpenToSibling = true;
    result->CanWrite = true;
    result->IsProc = false;
    result->IsMethod = false;
    result->Value = value;
    result->Folder = KON_NIL;
    // set mod
    if (mod != NULL) {
        if (strchr(mod, 'p')) {
            result->IsProc = true;
        }
        else if (strchr(mod, 'm')) {
            result->IsMethod = true;
        }
    }
    return result;
}

KN MakeAttrSlotFolder(KonState* kstate, char* mod)
{
    KonAttrSlot* result = KON_ALLOC_TYPE_TAG(kstate, KonAttrSlot, KON_T_ATTR_SLOT);
    result->IsDir = true;
    result->OpenToChildren = true;
    result->OpenToSibling = true;
    result->CanWrite = true;
    result->IsProc = false;
    result->IsMethod = false;
    result->Value = KON_NIL;
    result->Folder = KxHashTable_Init(4);
    return result;
}

KN KON_VectorToKonPairList(KonState* kstate, KxVector* vector)
{
    KN list = KON_NIL;

    int len = KxVector_Length(vector);
    for (int i = len - 1; i >= 0; i--) {
        KN item = KxVector_AtIndex(vector, i);
        list = KON_CONS(kstate, item, list);
    }

    return list;
}
