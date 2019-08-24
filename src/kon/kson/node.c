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
#include "../utils/realpath.h"
#include "../gc.h"


KxStringBuffer* KN_ReadFileContent(const char* filePathOrigin)
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


KN KN_AllocTagged(KonState* kstate, size_t size, kon_uint_t tag)
{
    KN res = (KN)tb_allocator_malloc0(kstate->Allocator, size);

    // add to heap ptr store
    KN_RecordNewKonNode(kstate, res);

    if (res && ! KN_IS_EXCEPTION(res)) {
        ((KonBase*)res)->Tag = tag;
        // set dispatcher id
        if (tag == KN_T_FIXNUM || tag == KN_T_FLONUM || tag == KN_T_BIGNUM) {
            ((KonBase*)res)->MsgDispatcherId = KN_T_NUMBER;
        }
        else if (tag == KN_T_NIL || tag == KN_T_PAIR) {
            ((KonBase*)res)->MsgDispatcherId = KN_T_PAIRLIST;
        }
        else {
            ((KonBase*)res)->MsgDispatcherId = tag;
        }
        
    }

    return res;
}

unsigned int KN_NodeDispacherId(KonState* kstate, KN obj)
{
    if (KN_IS_BOOLEAN(obj)) {
        return KN_T_BOOLEAN;
    }
    else if (obj == KN_UKN) {
        return KN_T_UKN;
    }
    else if (obj == KN_UNDEF) {
        return KN_T_UNDEF;
    }
    else if (obj == KN_NIL) {
        return KN_T_PAIRLIST;
    }
    else if (KN_IS_FIXNUM(obj)) {
        return KN_T_NUMBER;
    }
    else if (KN_IS_IMDT_SYMBOL(obj)) {
        return KN_T_SYMBOL;
    }
    else if (KN_IS_CHAR(obj)) {
        return KN_T_CHAR;
    }
    else if (KN_IS_POINTER(obj)) {
        return ((KonBase*)obj)->MsgDispatcherId;
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
    else if (KN_IS_EXPAND(source)) {
        return KN_ExpandStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_UNQUOTE(source)) {
        return KN_UnquoteStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_VECTOR(source)) {
        return KN_VectorStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IsPairList(source)) {
        return KN_PairListStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IsBlock(source)) {
        return KN_BlockStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_TABLE(source)) {
        return KN_TableStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_PARAM(source)) {
        return KN_ParamStringify(kstate, source, newLine, depth, padding);
    }
    else if (KN_IS_CELL(source)) {
        return KN_CellStringify(kstate, source, newLine, depth, padding);
    }
    else if (source == KN_UKN) {
        return KN_MakeString(kstate, "#ukn;");
    }
    else if (source == KN_TRUE) {
        return KN_MakeString(kstate, "#t;");
    }
    else if (source == KN_FALSE) {
        return KN_MakeString(kstate, "#f;");
    }
    else if (source == KN_UNDEF) {
        return KN_MakeString(kstate, "#undef;");
    }
    else if (KN_IS_CONTINUATION(source)) {
        return KN_MakeString(kstate, "#{continuation}");
    }
    else if (KN_IS_PROCEDURE(source)) {
        return KN_MakeString(kstate, "#{procedure}");
    }
    else if (KN_IS_CPOINTER(source)) {
        return KN_MakeString(kstate, "#{cpointer}");
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
    value->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->String, buf);

    return (KN)value;
}

KN KN_MakeFlonum(KonState* kstate, double f)
{
  return KN_MAKE_FLONUM(kstate, f);
}

KN KN_FlonumStringify(KonState* kstate, KN source)
{
    if (!KN_IS_FLONUM(source)) {
        return KN_MakeEmptyString(kstate);
    }
    char buf[128] = {'\0'};
    double num = KN_FLONUM_VALUE(source);
    double_to_str(num, 2, buf);

    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->String, buf);
    return (KN)value;
}

// 转换char为 #c,A;
KN KN_CharStringify(KonState* kstate, KN source)
{
    if (!KN_IS_CHAR(source)) {
        return KN_MakeEmptyString(kstate);
    }

    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->String, "#c,");
    int charcode = KN_UNBOX_CHAR(source);
    char buf[10] = {'\0'};
    snprintf(buf, 10, "%c", charcode);
    KxStringBuffer_AppendCstr(value->String, buf);
    KxStringBuffer_AppendCstr(value->String, ";");
    return (KN)value;
}

KN KN_StringStringify(KonState* kstate, KN source)
{
    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->String, "\"");
    KxStringBuffer_AppendStringBuffer(value->String, KN_UNBOX_STRING(source));
    KxStringBuffer_AppendCstr(value->String, "\"");
    return (KN)value;
}

KN KN_MakeString(KonState* kstate, const char* str)
{
    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->String, str);
    return (KN)value;
}

KN KN_MakeEmptyString(KonState* kstate)
{
    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->String = KxStringBuffer_New();
    return (KN)value;
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
    KonSymbolType type = CAST_Kon(Symbol, source)->Type;
    const char* data = KN_UNBOX_SYMBOL(source);

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->String = KxStringBuffer_New();

    switch (type) {
        case KN_SYM_PREFIX_WORD: {
            KxStringBuffer_AppendCstr(result->String, "!");
            KxStringBuffer_AppendCstr(result->String, data);
            break;
        }
        case KN_SYM_SUFFIX_WORD: {
            KxStringBuffer_AppendCstr(result->String, "^");
            KxStringBuffer_AppendCstr(result->String, data);
            break;
        }

        case KN_SYM_WORD: {
            KxStringBuffer_AppendCstr(result->String, data);
            break;
        }
        case KN_SYM_VARIABLE: {
            KxStringBuffer_AppendCstr(result->String, "@");
            KxStringBuffer_AppendCstr(result->String, data);
            break;
        }
        case KN_SYM_IDENTIFIER: {
            KxStringBuffer_AppendCstr(result->String, "$");
            KxStringBuffer_AppendCstr(result->String, data);
            break;
        }
        case KN_SYM_STRING: {
            KxStringBuffer_AppendCstr(result->String, "$\'");
            KxStringBuffer_AppendCstr(result->String, data);
            KxStringBuffer_AppendCstr(result->String, "\'");
            break;
        }
    }
    return (KN)result;
}

const char* KN_SymbolToCstr(KN sym)
{
    return KN_UNBOX_SYMBOL(sym);
}

KN KN_SyntaxMarkerStringify(KonState* kstate, KN source)
{
    KonSyntaxMarkerType type = CAST_Kon(SyntaxMarker, source)->Type;

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->String = KxStringBuffer_New();

    switch (type) {
        case KN_SYNTAX_MARKER_APPLY: {
            KxStringBuffer_AppendCstr(result->String, "%");
            break;
        }
        case KN_SYNTAX_MARKER_EQUAL: {
            KxStringBuffer_AppendCstr(result->String, "=");
            break;
        }
        case KN_SYNTAX_MARKER_ASSIGN: {
            KxStringBuffer_AppendCstr(result->String, ":=");
            break;
        }
        case KN_SYNTAX_MARKER_MSG_SIGNAL: {
            KxStringBuffer_AppendCstr(result->String, ".");
            break;
        }
        case KN_SYNTAX_MARKER_GET_SLOT: {
            KxStringBuffer_AppendCstr(result->String, "/");
            break;
        }
        case KN_SYNTAX_MARKER_PROC_PIPE: {
            KxStringBuffer_AppendCstr(result->String, "|");
            break;
        }
        case KN_SYNTAX_MARKER_CLAUSE_END: {
            KxStringBuffer_AppendCstr(result->String, ";");
            break;
        }
    }
    return (KN)result;
}

KN KN_QuoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonQuoteType type = CAST_Kon(Quote, source)->Type;
    KN inner = CAST_Kon(Quote, source)->Inner;

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->String = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->String, "$");

    KN innerToKonStr = KN_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(innerToKonStr));

    return (KN)result;
}

KN KN_QuasiquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonQuasiquoteType type = CAST_Kon(Quasiquote, source)->Type;
    KN inner = CAST_Kon(Quasiquote, source)->Inner;

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->String = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->String, "@");

    KN innerToKonStr = KN_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(innerToKonStr));

    return (KN)result;
}

KN KN_ExpandStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonExpandType type = CAST_Kon(Expand, source)->Type;
    KN inner = CAST_Kon(Expand, source)->Inner;

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->String = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->String, "$");

    switch (type) {
        case KN_EXPAND_REPLACE: {
            break;
        }
        case KN_EXPAND_KV: {
            KxStringBuffer_AppendCstr(result->String, "%");
            break;
        }
        case KN_EXPAND_SEQ: {
            KxStringBuffer_AppendCstr(result->String, "~");
            break;
        }
    }

    KxStringBuffer_AppendCstr(result->String, ".");

    KN innerToKonStr = KN_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(innerToKonStr));

    return (KN)result;
}

KN KN_UnquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonUnquoteType type = CAST_Kon(Unquote, source)->Type;
    KN inner = CAST_Kon(Unquote, source)->Inner;

    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->String = KxStringBuffer_New();

    KxStringBuffer_AppendCstr(result->String, "@");

    switch (type) {
        case KN_UNQUOTE_SEQ: {
            KxStringBuffer_AppendCstr(result->String, "~");
            break;
        }
        case KN_UNQUOTE_KV: {
            KxStringBuffer_AppendCstr(result->String, "%");
            break;
        }
    }

    KxStringBuffer_AppendCstr(result->String, ".");

    KN innerToKonStr = KN_ToFormatString(kstate, inner, newLine, depth, padding);
    KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(innerToKonStr));

    return (KN)result;
}

////
// the parent node add the first left padding
// don't add newline when stringify sub container types.
// add newline in parent node


KN KN_VectorStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->String = KxStringBuffer_New();

    KxVector* items = CAST_Kon(Vector, source)->Vector;

    int vecLen = KxVector_Length(items);
    
    if (newLine) {
        KxStringBuffer_AppendCstr(result->String, "#<\n");

        
        for (int i = 0; i < vecLen; i++) {
            KN item = KxVector_AtIndex(items, i);
            KN itemToKonStr = KN_ToFormatString(kstate, item, true, depth + 1, padding);

            AddLeftPadding(result->String, depth, padding);
            KxStringBuffer_AppendCstr(result->String, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(itemToKonStr));
            KxStringBuffer_AppendCstr(result->String, "\n");
        }

        AddLeftPadding(result->String, depth, padding);
        KxStringBuffer_AppendCstr(result->String, ">");
    }
    else {
        KxStringBuffer_AppendCstr(result->String, "#< ");
        
        for (int i = 0; i < vecLen; i++) {
            KN item = KxVector_AtIndex(items, i);
            
            KN itemToKonStr = KN_ToFormatString(kstate, item, false, depth + 1, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(itemToKonStr));

            // if (i != vecLen - 1) {
                KxStringBuffer_AppendCstr(result->String, " ");
            // }
            
        }

        KxStringBuffer_AppendCstr(result->String, ">");
    }

    return (KN)result;
}

bool KN_IsPairList(KN source)
{
    KonPair* iter = source;
    bool isList = true;
    while ((KN)iter != KN_NIL) {
        if (!KN_IS_PAIR(iter)) {
            isList = false;
            break;
        }
        iter = (KonPair*)KN_CDR(iter);
    }
    return isList;
}

bool KN_IsBlock(KN source)
{
    KonPair* iter = source;
    bool isList = true;
    while ((KN)iter != KN_NIL) {
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
    result->String = KxStringBuffer_New();

    if (source == KN_NIL) {
        KxStringBuffer_AppendCstr(result->String, "#nil;");
        return (KN)result;
    }
    
    if (newLine) {
        KxStringBuffer_AppendCstr(result->String, "[\n");
        if (source != KN_NIL && KN_IS_PAIR(source)) {
            KonPair* iter = source;

            while ((KN)iter != KN_NIL && KN_IS_PAIR(iter)) {
                KN item = KN_CAR(iter);
                KN next = KN_CDR(iter);

                KN itemToKonStr = KN_ToFormatString(kstate, item, true, depth + 1, padding);
                
                AddLeftPadding(result->String, depth, padding);
                KxStringBuffer_AppendCstr(result->String, padding);
                KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(itemToKonStr));
                KxStringBuffer_AppendCstr(result->String, "\n");

                iter = next;
            }
        }

        AddLeftPadding(result->String, depth, padding);
        KxStringBuffer_AppendCstr(result->String, "]");
    }
    else {
        KxStringBuffer_AppendCstr(result->String, "[");
        
        if (source != KN_NIL && KN_IS_PAIR(source)) {
            KonPair* iter = source;

            while ((KN)iter != KN_NIL && KN_IS_PAIR(iter)) {
                KN item = KN_CAR(iter);
                KN next = KN_CDR(iter);
                
                KN itemToKonStr = KN_ToFormatString(kstate, item, false, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(itemToKonStr));
                // if (next != KN_NIL) {
                    KxStringBuffer_AppendCstr(result->String, " ");
                // }
                iter = next;
            }
        }

        KxStringBuffer_AppendCstr(result->String, "]");
    }

    return (KN)result;
}

KN KN_BlockStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(result->String, "#");
    if (source == KN_NIL) {
        KxStringBuffer_AppendCstr(result->String, "[]");
    }
    else {
        // change first element tag to list pair
        ((KonBase*)source)->Tag = KN_T_PAIR;
        KN formated = KN_PairListStringify(kstate, source, true, depth, padding);
        KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(formated));
    }
    return (KN)result;
}
KN KN_PairListRevert(KonState* kstate, KN source)
{
    KN result = KN_NIL;
    if (source != KN_NIL && KN_IS_PAIR(source)) {
        KonPair* iter = source;
        while ((KN)iter != KN_NIL && KN_IS_PAIR(iter)) {
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
    if (source != KN_NIL && KN_IS_PAIR(source)) {
        KonPair* iter = source;
        while ((KN)iter != KN_NIL && KN_IS_PAIR(iter)) {
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
  if (KN_IS_EXCEPTION(node)) {
      return (KN)node;
  }
  node->Prev = KN_NIL;
  if (KN_IS_PAIR(tail)) {
      CAST_Kon(Pair, tail)->Prev = (KN)node;
  }
  KN_CAR(node) = head;
  KN_CDR(node) = tail;
  return (KN)node;
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
    result->String = KxStringBuffer_New();

    KxHashTable* hashTable = CAST_Kon(Table, source)->Table;
    KxHashTableIter iter = KxHashTable_IterHead(hashTable);

    if (newLine) {
        KxStringBuffer_AppendCstr(result->String, "#(\n");


        while ((KN)iter != KN_NIL) {
            KxHashTableIter next = KxHashTable_IterNext(hashTable, iter);
            const char* itemKey = KxHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KxHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KN_ToFormatString(kstate, itemValue, true, depth + 1, padding);
            if (itemKey != NULL) {
                AddLeftPadding(result->String, depth, padding);
                KxStringBuffer_AppendCstr(result->String, ":'");
                KxStringBuffer_AppendCstr(result->String, itemKey);
                KxStringBuffer_AppendCstr(result->String, "'=\n");
            }

            AddLeftPadding(result->String, depth, padding);
            KxStringBuffer_AppendCstr(result->String, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(itemToKonStr));
            KxStringBuffer_AppendCstr(result->String, "\n");

            iter = next;
        }

        AddLeftPadding(result->String, depth, padding);
        KxStringBuffer_AppendCstr(result->String, ")");
    }
    else {
        KxStringBuffer_AppendCstr(result->String, "#(");

        while ((KN)iter != KN_NIL) {
            KxHashTableIter next = KxHashTable_IterNext(hashTable, iter);
            const char* itemKey = KxHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KxHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KN_ToFormatString(kstate, itemValue, false, depth + 1, padding);

            if (itemKey != NULL) {
                KxStringBuffer_AppendCstr(result->String, ":'");
                KxStringBuffer_AppendCstr(result->String, itemKey);
                KxStringBuffer_AppendCstr(result->String, "'= ");
            }

            KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(itemToKonStr));
            
            // if (next != KN_NIL) {
                KxStringBuffer_AppendCstr(result->String, " ");
            // }

            iter = next;
        }
        KxStringBuffer_AppendCstr(result->String, ")");
    }

    return (KN)result;
}

KN KN_ParamStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->String = KxStringBuffer_New();

    KxHashTable* hashTable = CAST_Kon(Param, source)->Table;
    KxHashTableIter iter = KxHashTable_IterHead(hashTable);

    if (newLine) {
        KxStringBuffer_AppendCstr(result->String, "(\n");


        while ((KN)iter != KN_NIL) {
            KxHashTableIter next = KxHashTable_IterNext(hashTable, iter);
            const char* itemKey = KxHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KxHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KN_ToFormatString(kstate, itemValue, true, depth + 1, padding);
            if (itemKey != NULL) {
                AddLeftPadding(result->String, depth, padding);
                KxStringBuffer_AppendCstr(result->String, ":'");
                KxStringBuffer_AppendCstr(result->String, itemKey);
                KxStringBuffer_AppendCstr(result->String, "'\n");
            }

            AddLeftPadding(result->String, depth, padding);
            KxStringBuffer_AppendCstr(result->String, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(itemToKonStr));
            KxStringBuffer_AppendCstr(result->String, "\n");

            iter = next;
        }

        AddLeftPadding(result->String, depth, padding);
        KxStringBuffer_AppendCstr(result->String, ")");
    }
    else {
        KxStringBuffer_AppendCstr(result->String, "(");

        while ((KN)iter != KN_NIL) {
            KxHashTableIter next = KxHashTable_IterNext(hashTable, iter);
            const char* itemKey = KxHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KxHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KN_ToFormatString(kstate, itemValue, false, depth + 1, padding);

            if (itemKey != NULL) {
                KxStringBuffer_AppendCstr(result->String, ":'");
                KxStringBuffer_AppendCstr(result->String, itemKey);
                KxStringBuffer_AppendCstr(result->String, "' ");
            }

            KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(itemToKonStr));
            
            // if (next != KN_NIL) {
                KxStringBuffer_AppendCstr(result->String, " ");
            // }

            iter = next;
        }
        KxStringBuffer_AppendCstr(result->String, ")");
    }

    return (KN)result;
}

KN KN_ParamTableToList(KonState* kstate, KN source)
{
    KxHashTable* hashTable = CAST_Kon(Param, source)->Table;
    KxHashTableIter iter = KxHashTable_IterHead(hashTable);
    KN result = KN_NIL;
    while ((KN)iter != KN_NIL) {
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
    result->String = KxStringBuffer_New();

    KonCell* head = CAST_Kon(Cell, source);
    KonCell* iter = head;

    if (newLine) {
        KxStringBuffer_AppendCstr(result->String, "{");

        while ((KN)iter != KN_NIL) {
            KN name = iter->Core;
            KonVector* innerVector = iter->Vector;
            KonTable* innerTable = iter->Table;
            KonPair* innerList = iter->List;
            KxHashTable* innerMap = iter->Map;
            
            if (name != KN_UNDEF) {
                KN nameToKonStr = KN_ToFormatString(kstate, name, true, depth + 1, padding);
                if (iter != head) {
                    KxStringBuffer_AppendCstr(result->String, "\n");
                    AddLeftPadding(result->String, depth, padding);
                    KxStringBuffer_AppendCstr(result->String, padding);
                }
                KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(nameToKonStr));
            }

            // cell map section stringify
            KxHashTableIter mapIter = KxHashTable_IterHead(innerMap);
            while ((KN)mapIter != KN_NIL) {
                KxHashTableIter mapIterNext = KxHashTable_IterNext(innerMap, mapIter);
                const char* mapKey = KxHashTable_IterGetKey(innerMap, mapIter);
                KN mapValue = (KN)KxHashTable_IterGetVal(innerMap, mapIter);
                KxStringBuffer_AppendCstr(result->String, "\n");
                AddLeftPadding(result->String, depth, padding);
                KxStringBuffer_AppendCstr(result->String, padding);

                KxStringBuffer_AppendCstr(result->String, ":'");
                KxStringBuffer_AppendCstr(result->String, mapKey);
                KxStringBuffer_AppendCstr(result->String, "'");

                if (mapValue != NULL && mapValue != KN_UKN) {
                    KN mapValKonStr = KN_ToFormatString(kstate, mapValue, true, depth + 1, padding);
                    KxStringBuffer_AppendCstr(result->String, "= ");
                    KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(mapValKonStr));
                }
                mapIter = mapIterNext;
            }

            if ((KN)innerTable != KN_UNDEF) {
                KxStringBuffer_AppendCstr(result->String, "\n");
                AddLeftPadding(result->String, depth, padding);
                KxStringBuffer_AppendCstr(result->String, padding);

                KN innerTableToKonStr = KN_ParamStringify(kstate, (KN)innerTable, true, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(innerTableToKonStr));
            }
            
            if ((KN)innerVector != KN_UNDEF) {
                KxStringBuffer_AppendCstr(result->String, "\n");
                AddLeftPadding(result->String, depth, padding);
                KxStringBuffer_AppendCstr(result->String, padding);

                KN innerVectorToKonStr = KN_ToFormatString(kstate, (KN)innerVector, true, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(innerVectorToKonStr));
            }
            
             if ((KN)innerList != KN_UNDEF) {
                 KxStringBuffer_AppendCstr(result->String, "\n");
                 AddLeftPadding(result->String, depth, padding);
                 KxStringBuffer_AppendCstr(result->String, padding);

                 KN innerListToKonStr = KN_BlockStringify(kstate, (KN)innerList, true, depth + 1, padding);
                 KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(innerListToKonStr));
             }
            
            iter = iter->Next;
        }

        if ((KN)head != KN_NIL
            || (KN)head->Table != KN_UNDEF
            || (KN)head->List != KN_UNDEF
            || (KN)head->Next != KN_NIL
        ) {
            KxStringBuffer_AppendCstr(result->String, "\n");
            AddLeftPadding(result->String, depth, padding);
        }

        KxStringBuffer_AppendCstr(result->String, "}");
    }
    else {
        KxStringBuffer_AppendCstr(result->String, "{");

        while ((KN)iter != KN_NIL) {
            KN name = iter->Core;
            KonVector* innerVector = iter->Vector;
            KonTable* innerTable = iter->Table;
            KonPair* innerList = iter->List;
            KxHashTable* innerMap = iter->Map;

            if (name != KN_UNDEF) {
                KN nameToKonStr = KN_ToFormatString(kstate, name, true, depth, padding);
                if (iter != head) {
                    KxStringBuffer_AppendCstr(result->String, " ");
                }
                KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(nameToKonStr));
            }

            // cell map section stringify
            KxHashTableIter mapIter = KxHashTable_IterHead(innerMap);
            while ((KN)mapIter != KN_NIL) {
                KxHashTableIter mapIterNext = KxHashTable_IterNext(innerMap, mapIter);
                const char* mapKey = KxHashTable_IterGetKey(innerMap, mapIter);
                KN mapValue = (KN)KxHashTable_IterGetVal(innerMap, mapIter);
                KxStringBuffer_AppendCstr(result->String, " ");

                KxStringBuffer_AppendCstr(result->String, ":'");
                KxStringBuffer_AppendCstr(result->String, mapKey);
                KxStringBuffer_AppendCstr(result->String, "'");

                if (mapValue != NULL && mapValue != KN_UKN) {
                    KN mapValKonStr = KN_ToFormatString(kstate, mapValue, true, depth + 1, padding);
                    KxStringBuffer_AppendCstr(result->String, "= ");
                    KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(mapValKonStr));
                }
                mapIter = mapIterNext;
            }
            
            if ((KN)innerTable != KN_UNDEF) {
                KxStringBuffer_AppendCstr(result->String, " ");
                KN innerTableToKonStr = KN_ParamStringify(kstate, (KN)innerTable, false, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(innerTableToKonStr));
            }

            
            if ((KN)innerVector != KN_UNDEF) {
                KxStringBuffer_AppendCstr(result->String, " ");
                KN innerVectorToKonStr = KN_ToFormatString(kstate, (KN)innerVector, false, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(innerVectorToKonStr));
            }

            if ((KN)innerList != KN_UNDEF) {
                KxStringBuffer_AppendCstr(result->String, " ");
                KN innerListToKonStr = KN_BlockStringify(kstate, (KN)innerList, false, depth + 1, padding);
                KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(innerListToKonStr));
            }

            iter = iter->Next;
        }
        KxStringBuffer_AppendCstr(result->String, "}");
    }

    return (KN)result;
}

KN KN_CellCoresToList(KonState* kstate, KN source)
{
    KonCell* head = CAST_Kon(Cell, source);
    KonCell* iter = head;
    KN result = KN_NIL;
    while ((KN)iter != KN_NIL) {
        KN core = iter->Core;
        result = KN_CONS(kstate, core, result);
        iter = iter->Next;
    }
    return KN_PairListRevert(kstate, result);
}

KN KN_AccessorStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    result->String = KxStringBuffer_New();

    KonAccessor* accessor = CAST_Kon(Accessor, source);
    if (!accessor->IsDir) {
        KN value = accessor->Value;
        // get accessor inner value
        while (KN_IS_ACCESSOR(value)
            && !CAST_Kon(Accessor, value)->IsDir
        ) {
            value = CAST_Kon(Accessor, value)->Value;
        }
        return KN_ToFormatString(kstate, value, true, depth, padding);
    }
    KxHashTable* hashTable = accessor->Dir;
    KxHashTableIter iter = KxHashTable_IterHead(hashTable);

    if (newLine) {
        KxStringBuffer_AppendCstr(result->String, "#accesor.(\n");

        while ((KN)iter != KN_NIL) {
            KxHashTableIter next = KxHashTable_IterNext(hashTable, iter);
            const char* itemKey = KxHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KxHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KN_ToFormatString(kstate, itemValue, true, depth + 1, padding);

            AddLeftPadding(result->String, depth, padding);
            KxStringBuffer_AppendCstr(result->String, ":'");
            KxStringBuffer_AppendCstr(result->String, itemKey);
            KxStringBuffer_AppendCstr(result->String, "'\n");

            AddLeftPadding(result->String, depth, padding);
            KxStringBuffer_AppendCstr(result->String, padding);
            KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(itemToKonStr));
            KxStringBuffer_AppendCstr(result->String, "\n");

            iter = next;
        }

        AddLeftPadding(result->String, depth, padding);
        KxStringBuffer_AppendCstr(result->String, ")");
    }
    else {
        KxStringBuffer_AppendCstr(result->String, "#accesor.(");

        while ((KN)iter != KN_NIL) {
            KxHashTableIter next = KxHashTable_IterNext(hashTable, iter);
            const char* itemKey = KxHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KxHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KN_ToFormatString(kstate, itemValue, false, depth + 1, padding);

            KxStringBuffer_AppendCstr(result->String, ":'");
            KxStringBuffer_AppendCstr(result->String, itemKey);
            KxStringBuffer_AppendCstr(result->String, "' ");

            KxStringBuffer_AppendStringBuffer(result->String, KN_UNBOX_STRING(itemToKonStr));
            
            // if (next != KN_NIL) {
                KxStringBuffer_AppendCstr(result->String, " ");
            // }

            iter = next;
        }
        KxStringBuffer_AppendCstr(result->String, ")");
    }

    return (KN)result;
}


KN MakeNativeProcedure(KonState* kstate, KonProcedureType type, KonNativeFuncRef funcRef)
{
    KonProcedure* result = KN_ALLOC_TYPE_TAG(kstate, KonProcedure, KN_T_PROCEDURE);
    result->Type = type;
    result->NativeFuncRef = funcRef;
    return (KN)result;
}

KonProcedure* MakeDispatchProc(KonState* kstate, KN procAst, KonEnv* env)
{
    if (procAst == KN_NIL || procAst == KN_UNDEF|| procAst == KN_UKN) {
        return NULL;
    }
    KonProcedure* proc = KN_ALLOC_TYPE_TAG(kstate, KonProcedure, KN_T_PROCEDURE);
    
    
    KN param = (KN)KN_DTR(procAst);
    KN funcName = KN_UNDEF;
    KN body = KN_NIL;
    
    if (param == KN_UNDEF) {
        funcName = KN_DCNR(procAst);
        param = (KN)KN_DTNR(procAst);
        body = (KN)KN_DLNR(procAst);
    }
    else {
        body = (KN)KN_DLR(procAst);
    }
    
    param = KN_ParamTableToList(kstate, param);
    
    proc->Type = KN_COMPOSITE_LAMBDA;
    proc->Composite.LexicalEnv = env;
    proc->Composite.ArgList = param;
    proc->Composite.Body = body;
    return proc;
}


KonMsgDispatcher* MakeMsgDispatcher(KonState* kstate)
{
    KonMsgDispatcher* result = KN_ALLOC_TYPE_TAG(kstate, KonMsgDispatcher, KN_T_MSG_DISPATCHER);
    result->OnSymbol = (KonProcedure*)KN_UNDEF;
    result->OnApplyArgs = (KonProcedure*)KN_UNDEF;
    result->OnSelectPath = (KonProcedure*)KN_UNDEF;
    result->OnMethodCall = (KonProcedure*)KN_UNDEF;
    result->OnVisitVector = (KonProcedure*)KN_UNDEF;
    result->OnVisitTable = (KonProcedure*)KN_UNDEF;
    result->OnVisitCell = (KonProcedure*)KN_UNDEF;
    return result;
}

int KN_SetMsgDispatcher(KonState* kstate, unsigned int dispatcherId, KonMsgDispatcher* dispatcher)
{
    KxVector_SetIndex(kstate->MsgDispatchers, dispatcherId, dispatcher);
    return 1;
}

unsigned int KN_SetNextMsgDispatcher(KonState* kstate, KonMsgDispatcher* dispatcher)
{
    int dispacherId = kstate->NextMsgDispatcherId;
    KN_SetMsgDispatcher(kstate, dispacherId, dispatcher);
    kstate->NextMsgDispatcherId += 1;
    return dispacherId;
}

KonMsgDispatcher* KN_GetMsgDispatcher(KonState* kstate, unsigned int dispatcherId)
{
    KN dispatcher = KxVector_AtIndex(kstate->MsgDispatchers, dispatcherId);
    if (dispatcher == KN_UKN || dispatcher == NULL) {
        return NULL;
    }
    else {
        return (KonMsgDispatcher*)dispatcher;
    }
}

KonCpointer* KN_MakeCpointer(KonState* kstate, void* pointer)
{
    KonCpointer* result = KN_ALLOC_TYPE_TAG(kstate, KonCpointer, KN_T_CPOINTER);
    result->Pointer = pointer;
    return result;
}

KonAccessor* KN_InitAccessorWithMod(KonState* kstate, char* mod)
{
    KonAccessor* result = KN_ALLOC_TYPE_TAG(kstate, KonAccessor, KN_T_ACCESSOR);
    result->IsDir = false;
    result->OpenToRef = false;
    result->OpenToChildren = false;
    result->OpenToSibling = false;
    result->CanWrite = false;
    // result->CanExec = false;
    // result->IsMethod = false;
    result->Value = KN_UNDEF;
    result->Dir = NULL;
    result->Setter = NULL;
    // set mod
    if (mod != NULL) {
        if (strchr(mod, 'd')) {
            result->IsDir = true;
        }
        if (strchr(mod, 'r')) {
            result->OpenToRef = true;
        }
        if (strchr(mod, 'w')) {
            result->CanWrite = true;
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
    result->IsDir = false;
    result->Value = value;
    if (setter != NULL) {
        result->Setter = setter;
    }
    return result;
}

KN KN_MakeDirAccessor(KonState* kstate, char* mod, KonProcedure* setter)
{
    KonAccessor* result = KN_InitAccessorWithMod(kstate, mod);
    result->IsDir = true;
    result->Value = KN_NIL;
    result->Dir = KxHashTable_Init(4);
    if (setter != NULL) {
        result->Setter = setter;
    }
    return (KN)result;
}

bool KN_DirAccessorPutKeyProperty(KonState* kstate, KN dir, char* key, KN property)
{
    if (!(KN_IS_ACCESSOR(dir) && CAST_Kon(Accessor, dir)->IsDir == true)
        || !KN_IS_ACCESSOR(property)) {
        return false;
    }

    KxHashTable_PutKv(
        CAST_Kon(Accessor, dir)->Dir,
        key,
        property
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
        KN item = KxVector_AtIndex(vector, i);
        list = KN_CONS(kstate, item, list);
    }

    return list;
}
