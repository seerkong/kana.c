#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include "node.h"
#include "../utils/number_utils.h"


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

tb_void_t kon_hash_item_ptr_free(tb_element_ref_t element, tb_pointer_t buff)
{
    // tb_assert_and_check_return(element && buff);
    // KN data = (KN)(*((tb_pointer_t*)buff));

    // TODO release
    // if (data) {
    //     tb_free(data);
    // }

    // TODO clear
    // *((tb_pointer_t*)buff) = tb_null;

}

tb_void_t kon_vector_item_ptr_free(tb_element_ref_t element, tb_pointer_t buff)
{
    // tb_assert_and_check_return(element && buff);

    // KN data = (KN)(*((tb_pointer_t*)buff));

    // TODO release
    // if (data) {
    //     tb_free(data);
    // }

    // TODO clear
    // *((tb_pointer_t*)buff) = tb_null;
}

KN KON_AllocTagged(KonState* kstate, size_t size, kon_uint_t tag)
{
    KN res = (KN) KON_GC_MALLOC(size);
    if (res && ! kon_is_exception(res)) {
        ((KonBase*)res)->Tag = tag;
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
    else if (kon_is_char(source)) {
        return KON_CharStringify(kstate, source);
    }
    else if (kon_is_string(source)) {
        return KON_StringStringify(kstate, source);
    }
    else if (kon_is_symbol(source)) {
        return KON_SymbolStringify(kstate, source);
    }
    else if (kon_is_syntax_marker(source)) {
        return KON_SyntaxMarkerStringify(kstate, source);
    }
    else if (kon_is_quote(source)) {
        return KON_QuoteStringify(kstate, source, newLine, depth, padding);
    }
    else if (kon_is_quasiquote(source)) {
        return KON_QuasiquoteStringify(kstate, source, newLine, depth, padding);
    }
    else if (kon_is_expand(source)) {
        return KON_ExpandStringify(kstate, source, newLine, depth, padding);
    }
    else if (kon_is_unquote(source)) {
        return KON_UnquoteStringify(kstate, source, newLine, depth, padding);
    }
    else if (kon_is_vector(source)) {
        return KON_VectorStringify(kstate, source, newLine, depth, padding);
    }
    else if (KON_IsList(source)) {
        return KON_ListStringify(kstate, source, newLine, depth, padding);
    }
    else if (kon_is_table(source)) {
        return KON_TableStringify(kstate, source, newLine, depth, padding);
    }
    else if (kon_is_cell(source)) {
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
    else if (source == KON_NULL) {
        return KON_MakeString(kstate, "#null;");
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
    kon_int_t num = kon_unbox_fixnum(source);
    itoa(num, buf, 10);

    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KonStringBuffer_New();
    KonStringBuffer_AppendCstr(value->String, buf);

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
    double num = kon_flonum_value(source);
    double_to_str(num, 2, buf);

    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KonStringBuffer_New();
    KonStringBuffer_AppendCstr(value->String, buf);
    return value;
}

// 转换char为 #c,A;
KN KON_CharStringify(KonState* kstate, KN source)
{
    if (!kon_is_char(source)) {
        return KON_MakeEmptyString(kstate);
    }

    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KonStringBuffer_New();
    KonStringBuffer_AppendCstr(value->String, "#c,");
    int charcode = kon_unbox_character(source);
    char buf[10] = {'\0'};
    snprintf(buf, 10, "%c", charcode);
    KonStringBuffer_AppendCstr(value->String, buf);
    KonStringBuffer_AppendCstr(value->String, ";");
    return value;
}

KN KON_StringStringify(KonState* kstate, KN source)
{
    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KonStringBuffer_New();
    KonStringBuffer_AppendCstr(value->String, "\"");
    KonStringBuffer_AppendStringBuffer(value->String, KON_UNBOX_STRING(source));
    KonStringBuffer_AppendCstr(value->String, "\"");
    return value;
}

KN KON_MakeString(KonState* kstate, const char* str)
{
    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KonStringBuffer_New();
    KonStringBuffer_AppendCstr(value->String, str);
    return value;
}

KN KON_MakeEmptyString(KonState* kstate)
{
    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KonStringBuffer_New();
    return value;
}

const char* KON_StringToCstr(KN str)
{
    if (!kon_is_string(str)) {
        return NULL;
    }
    return KonStringBuffer_Cstr(KON_UNBOX_STRING(str));
}

void AddLeftPadding(KonStringBuffer* str, int depth, char* padding)
{
    for (int i = 0; i < depth; i++) {
        KonStringBuffer_AppendCstr(str, padding);
    }
}

KN KON_SymbolStringify(KonState* kstate, KN source)
{
    KonSymbolType type = CAST_Kon(Symbol, source)->Type;
    const char* data = KON_UNBOX_SYMBOL(source);

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KonStringBuffer_New();

    switch (type) {
        case KON_SYM_IDENTIFER: {
            KonStringBuffer_AppendCstr(result->String, data);
            break;
        }
        case KON_SYM_STRING: {
            KonStringBuffer_AppendCstr(result->String, "\'");
            KonStringBuffer_AppendCstr(result->String, data);
            KonStringBuffer_AppendCstr(result->String, "\'");
            break;
        }
        case KON_SYM_VAR: {
            KonStringBuffer_AppendCstr(result->String, "$");
            KonStringBuffer_AppendCstr(result->String, data);
            break;
        }
        case KON_SYM_PREFIX_MARCRO: {
            KonStringBuffer_AppendCstr(result->String, "!");
            KonStringBuffer_AppendCstr(result->String, data);
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
    result->String = KonStringBuffer_New();

    switch (type) {
        case KON_SYNTAX_MARKER_APPLY: {
            KonStringBuffer_AppendCstr(result->String, "%");
            break;
        }
        case KON_SYNTAX_MARKER_EXEC_MSG: {
            KonStringBuffer_AppendCstr(result->String, ".");
            break;
        }
        case KON_SYNTAX_MARKER_PIPE: {
            KonStringBuffer_AppendCstr(result->String, "|");
            break;
        }
        case KON_SYNTAX_MARKER_CLAUSE_END: {
            KonStringBuffer_AppendCstr(result->String, ";");
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
    result->String = KonStringBuffer_New();

    KonStringBuffer_AppendCstr(result->String, "$");

    KN innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerToKonStr));

    return result;
}

KN KON_QuasiquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonQuasiquoteType type = CAST_Kon(Quasiquote, source)->Type;
    KN inner = CAST_Kon(Quasiquote, source)->Inner;

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KonStringBuffer_New();

    KonStringBuffer_AppendCstr(result->String, "@");

    KN innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerToKonStr));

    return (KN)result;
}

KN KON_ExpandStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonExpandType type = CAST_Kon(Expand, source)->Type;
    KN inner = CAST_Kon(Expand, source)->Inner;

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KonStringBuffer_New();

    KonStringBuffer_AppendCstr(result->String, "$");

    switch (type) {
        case KON_EXPAND_REPLACE: {
            break;
        }
        case KON_EXPAND_KV: {
            KonStringBuffer_AppendCstr(result->String, "%");
            break;
        }
        case KON_EXPAND_SEQ: {
            KonStringBuffer_AppendCstr(result->String, "~");
            break;
        }
    }

    KonStringBuffer_AppendCstr(result->String, ".");

    KN innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerToKonStr));

    return result;
}

KN KON_UnquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonUnquoteType type = CAST_Kon(Unquote, source)->Type;
    KN inner = CAST_Kon(Unquote, source)->Inner;

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KonStringBuffer_New();

    KonStringBuffer_AppendCstr(result->String, "@");

    switch (type) {
        case KON_UNQUOTE_SEQ: {
            KonStringBuffer_AppendCstr(result->String, "~");
            break;
        }
        case KON_UNQUOTE_KV: {
            KonStringBuffer_AppendCstr(result->String, "%");
            break;
        }
    }

    KonStringBuffer_AppendCstr(result->String, ".");

    KN innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerToKonStr));

    return result;
}

////
// the parent node add the first left padding
// don't add newline when stringify sub container types.
// add newline in parent node


KN KON_VectorStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KonStringBuffer_New();

    tb_vector_ref_t items = CAST_Kon(Vector, source)->Vector;
    tb_size_t itor = tb_iterator_head(items);
    tb_size_t tail = tb_iterator_tail(items);
    
    
    
    if (newLine) {
        KonStringBuffer_AppendCstr(result->String, "[\n");

        for (; itor != tail; itor = tb_iterator_next(items, itor)) {
            KN item = (KN)tb_iterator_item(items, itor);
            KN itemToKonStr = KON_ToFormatString(kstate, item, true, depth + 1, padding);

            AddLeftPadding(result->String, depth, padding);
            KonStringBuffer_AppendCstr(result->String, padding);
            KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(itemToKonStr));
            KonStringBuffer_AppendCstr(result->String, "\n");
        }

        AddLeftPadding(result->String, depth, padding);
        KonStringBuffer_AppendCstr(result->String, "]");
    }
    else {
        KonStringBuffer_AppendCstr(result->String, "[");
        
        while (itor != tb_iterator_tail(items)) {
            tb_size_t next = tb_iterator_next(items, itor);
            
            KN item = (KN)tb_iterator_item(items, itor);
            KN itemToKonStr = KON_ToFormatString(kstate, item, false, depth + 1, padding);
            KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(itemToKonStr));

            if (next != tb_iterator_tail(items)) {
                KonStringBuffer_AppendCstr(result->String, " ");
            }
            
            itor = next;
        }

        KonStringBuffer_AppendCstr(result->String, "]");
    }

    return result;
}

bool KON_IsList(KN source)
{
    KonListNode* iter = source;
    bool isList = true;
    do {
        if (iter == KON_NIL) {
            break;
        }
        if (!kon_is_list_node(iter)) {
            isList = false;
            break;
        }
        iter = kon_cdr(iter);
    } while (iter != KON_NIL);
    return isList;
}

KN KON_ListStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    assert(KON_IsList(source));
    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KonStringBuffer_New();

    
    if (newLine) {
        KonStringBuffer_AppendCstr(result->String, "{\n");
        if (source != KON_NIL && kon_is_list_node(source)) {
            KonListNode* iter = source;

            while (iter != KON_NIL) {
                KN item = kon_car(iter);
                KN next = kon_cdr(iter);

                KN itemToKonStr = KON_ToFormatString(kstate, item, true, depth + 1, padding);
                
                AddLeftPadding(result->String, depth, padding);
                KonStringBuffer_AppendCstr(result->String, padding);
                KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(itemToKonStr));
                KonStringBuffer_AppendCstr(result->String, "\n");

                iter = next;
            }
        }

        AddLeftPadding(result->String, depth, padding);
        KonStringBuffer_AppendCstr(result->String, "}");
    }
    else {
        KonStringBuffer_AppendCstr(result->String, "{");
        
        if (source != KON_NIL && kon_is_list_node(source)) {
            KonListNode* iter = source;

            while (iter != KON_NIL) {
                KN item = kon_car(iter);
                KN next = kon_cdr(iter);
                
                KN itemToKonStr = KON_ToFormatString(kstate, item, false, depth + 1, padding);
                KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(itemToKonStr));
                if (next != KON_NIL) {
                    KonStringBuffer_AppendCstr(result->String, " ");
                }
                iter = next;
            }
        }

        KonStringBuffer_AppendCstr(result->String, "}");
    }

    return result;
}

KN Kon_ListRevert(KonState* kstate, KN source)
{
    KN result = KON_NIL;
    if (source != KON_NIL && kon_is_list_node(source)) {
        KonListNode* iter = source;
        while (iter != KON_NIL) {
            KN item = kon_car(iter);
            KN next = kon_cdr(iter);

            result = kon_cons(kstate, item, result);

            iter = next;
        }
    }
    return result;
}

KN KON_Cons(KonState* kstate, KN self, kon_int_t n, KN head, KN tail)
{
  KonListNode* node = KON_ALLOC_TYPE_TAG(kstate, KonListNode, KON_T_LIST_NODE);
  if (kon_is_exception(node)) {
      return node;
  }
  node->Prev = KON_NIL;
  if (kon_is_list_node(tail)) {
      CAST_Kon(ListNode, tail)->Prev = node;
  }
  kon_car(node) = head;
  kon_cdr(node) = tail;
  return node;
}

KN KON_List2(KonState* kstate, KN a, KN b)
{
  KN res = kon_cons(kstate, b, KON_NIL);
  res = kon_cons(kstate, a, res);
  return res;
}

KN KON_List3(KonState* kstate, KN a, KN b, KN c)
{
  KN res = KON_List2(kstate, b, c);
  res = kon_cons(kstate, a, res);
  return res;
}



KN KON_TableStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KonStringBuffer_New();

    KonHashTable* hashTable = CAST_Kon(Table, source)->Table;
    KonHashTableIter iter = KonHashTable_IterHead(hashTable);

    if (newLine) {
        KonStringBuffer_AppendCstr(result->String, "(\n");


        while (iter != KON_HASH_TABLE_NIL) {
            KonHashTableIter next = KonHashTable_IterNext(hashTable, iter);
            char* itemKey = KonHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KonHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KON_ToFormatString(kstate, itemValue, true, depth + 1, padding);

            AddLeftPadding(result->String, depth, padding);
            KonStringBuffer_AppendCstr(result->String, ":'");
            KonStringBuffer_AppendCstr(result->String, itemKey);
            KonStringBuffer_AppendCstr(result->String, "'\n");

            AddLeftPadding(result->String, depth, padding);
            KonStringBuffer_AppendCstr(result->String, padding);
            KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(itemToKonStr));
            KonStringBuffer_AppendCstr(result->String, "\n");

            iter = next;
        }

        AddLeftPadding(result->String, depth, padding);
        KonStringBuffer_AppendCstr(result->String, ")");
    }
    else {
        KonStringBuffer_AppendCstr(result->String, "(");

        while (iter != KON_HASH_TABLE_NIL) {
            KonHashTableIter next = KonHashTable_IterNext(hashTable, iter);
            char* itemKey = KonHashTable_IterGetKey(hashTable, iter);
            KN itemValue = (KN)KonHashTable_IterGetVal(hashTable, iter);

            KN itemToKonStr = KON_ToFormatString(kstate, itemValue, false, depth + 1, padding);

            KonStringBuffer_AppendCstr(result->String, ":'");
            KonStringBuffer_AppendCstr(result->String, itemKey);
            KonStringBuffer_AppendCstr(result->String, "' ");

            KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(itemToKonStr));
            
            if (next != NULL) {
                KonStringBuffer_AppendCstr(result->String, " ");
            }

            iter = next;
        }
        KonStringBuffer_AppendCstr(result->String, ")");
    }

    return result;
}



KN KON_CellStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding)
{
    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    result->String = KonStringBuffer_New();

    KN name = CAST_Kon(Cell, source)->Name;
    KonVector* innerVector = CAST_Kon(Cell, source)->Vector;
    KonTable* innerTable = CAST_Kon(Cell, source)->Table;
    KonListNode* innerList = CAST_Kon(Cell, source)->List;
    
    if (newLine) {
        KonStringBuffer_AppendCstr(result->String, "<");
        
        if (name != KON_NULL) {
            KN nameToKonStr = KON_ToFormatString(kstate, name, true, depth, padding);
            KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(nameToKonStr));
        }
        KonStringBuffer_AppendCstr(result->String, "\n");

        if (innerTable != KON_NULL) {
            AddLeftPadding(result->String, depth, padding);
            KonStringBuffer_AppendCstr(result->String, padding);

            KN innerTableToKonStr = KON_ToFormatString(kstate, innerTable, true, depth + 1, padding);

            KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerTableToKonStr));
            KonStringBuffer_AppendCstr(result->String, "\n");
        }
        
        if (innerVector != KON_NULL) {
            AddLeftPadding(result->String, depth, padding);
            KonStringBuffer_AppendCstr(result->String, padding);

            KN innerVectorToKonStr = KON_ToFormatString(kstate, innerVector, true, depth + 1, padding);
            KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerVectorToKonStr));
            KonStringBuffer_AppendCstr(result->String, "\n");
        }
        
        if (innerList != KON_NULL) {
            AddLeftPadding(result->String, depth, padding);
            KonStringBuffer_AppendCstr(result->String, padding);

            KN innerListToKonStr = KON_ToFormatString(kstate, innerList, true, depth + 1, padding);
            KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerListToKonStr));
            KonStringBuffer_AppendCstr(result->String, "\n");
        }
        

        AddLeftPadding(result->String, depth, padding);
        KonStringBuffer_AppendCstr(result->String, ">");
    }
    else {
        KonStringBuffer_AppendCstr(result->String, "<");

        if (name != KON_NULL) {
            KN nameToKonStr = KON_ToFormatString(kstate, name, true, depth, padding);
            KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(nameToKonStr));
        }

        
        if (innerTable != KON_NULL) {
            KonStringBuffer_AppendCstr(result->String, " ");
            KN innerTableToKonStr = KON_ToFormatString(kstate, innerTable, false, depth + 1, padding);
            KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerTableToKonStr));
        }

        
        if (innerVector != KON_NULL) {
            KonStringBuffer_AppendCstr(result->String, " ");
            KN innerVectorToKonStr = KON_ToFormatString(kstate, innerVector, false, depth + 1, padding);
            KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerVectorToKonStr));
        }

        if (innerList != KON_NULL) {
            KonStringBuffer_AppendCstr(result->String, " ");
            KN innerListToKonStr = KON_ToFormatString(kstate, innerList, false, depth + 1, padding);
            KonStringBuffer_AppendStringBuffer(result->String, KON_UNBOX_STRING(innerListToKonStr));
        }

        KonStringBuffer_AppendCstr(result->String, ">");
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


KN TbVectorToKonList(KonState* kstate, tb_vector_ref_t clauseWords)
{
    KN clause = KON_NIL;
    
    tb_size_t clauseHead = tb_iterator_head(clauseWords);
    tb_size_t clauseItor = tb_iterator_tail(clauseWords);
    do {
        // the previous item
        clauseItor = tb_iterator_prev(clauseWords, clauseItor);
        
        KN item = tb_iterator_item(clauseWords, clauseItor);
        if (item == NULL) {
            break;
        }
        clause = kon_cons(kstate, item, clause);
        
    } while (clauseItor != clauseHead);
    return clause;
}