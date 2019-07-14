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
    else if (source == KON_TRUE) {
        return KON_MakeString(kstate, "#t;");
    }
    else if (source == KON_FALSE) {
        return KON_MakeString(kstate, "#f;");
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
    tb_string_init(&(value->String));
    tb_string_cstrcat(&(value->String), buf);

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
    tb_string_init(&(value->String));
    tb_string_cstrcat(&(value->String), buf);
    return value;
}

// 转换char为 #c,A;
KN KON_CharStringify(KonState* kstate, KN source)
{
    if (!kon_is_char(source)) {
        return KON_MakeEmptyString(kstate);
    }

    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    tb_string_init(&(value->String));
    tb_string_cstrcat(&(value->String), "#c,");
    int charcode = kon_unbox_character(source);
    char buf[10] = {'\0'};
    snprintf(buf, 10, "%c", charcode);
    tb_string_cstrcat(&(value->String), buf);
    tb_string_cstrcat(&(value->String), ";");
    return value;
}

KN KON_StringStringify(KonState* kstate, KN source)
{
    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    tb_string_init(&(value->String));
    tb_string_cstrcat(&(value->String), "\"");
    tb_string_strcat(&(value->String), &(CAST_Kon(String, source)->String));
    tb_string_cstrcat(&(value->String), "\"");
    return (KN)value;
}

KN KON_MakeString(KonState* kstate, const char* str)
{
    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    tb_string_init(&(value->String));
    tb_string_cstrcat(&(value->String), str);
    return (KN)value;
}

KN KON_MakeEmptyString(KonState* kstate)
{
    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    tb_string_init(&(value->String));
    return (KN)value;
}

const char* KON_StringToCstr(KN str)
{
    if (!kon_is_string(str)) {
        return NULL;
    }
    return tb_string_cstr(&KON_UNBOX_STRING(str));
}

void AddLeftPadding(tb_string_ref_t str, int depth, char* padding)
{
    for (int i = 0; i < depth; i++) {
        tb_string_cstrcat(str, padding);
    }
}

KN KON_SymbolStringify(KonState* kstate, KN source)
{
    KonSymbolType type = CAST_Kon(Symbol, source)->Type;
    tb_string_ref_t data = &KON_UNBOX_SYMBOL(source);

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    tb_string_init(&(result->String));

    switch (type) {
        case KON_SYM_IDENTIFER: {
            tb_string_strcat(&(result->String), data);
            break;
        }
        case KON_SYM_STRING: {
            tb_string_cstrcat(&(result->String), "\'");
            tb_string_strcat(&(result->String), data);
            tb_string_cstrcat(&(result->String), "\'");
            break;
        }
        case KON_SYM_VAR: {
            tb_string_cstrcat(&(result->String), "$");
            tb_string_strcat(&(result->String), data);
            break;
        }
        case KON_SYM_FORM_WORD: {
            tb_string_cstrcat(&(result->String), "!");
            tb_string_strcat(&(result->String), data);
            break;
        }
    }
    return result;
}

const char* KON_SymbolToCstr(KN sym)
{
    // TODO assert
    return tb_string_cstr(&KON_UNBOX_SYMBOL(sym));
}

KN KON_SyntaxMarkerStringify(KonState* kstate, KonSyntaxMarker* source)
{
    KonSyntaxMarkerType type = CAST_Kon(SyntaxMarker, source)->Type;

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    tb_string_init(&(result->String));

    switch (type) {
        case KON_SYNTAX_MARKER_APPLY: {
            tb_string_cstrcat(&(result->String), "%");
            break;
        }
        case KON_SYNTAX_MARKER_EXEC_MSG: {
            tb_string_cstrcat(&(result->String), ".");
            break;
        }
        case KON_SYNTAX_MARKER_PIPE: {
            tb_string_cstrcat(&(result->String), "|");
            break;
        }
        case KON_SYNTAX_MARKER_CLAUSE_END: {
            tb_string_cstrcat(&(result->String), ";");
            break;
        }
    }
    return (KN)result;
}

KN KON_QuoteStringify(KonState* kstate, KonQuote* source, bool newLine, int depth, char* padding)
{
    KonQuoteType type = CAST_Kon(Quote, source)->Type;
    KN inner = CAST_Kon(Quote, source)->Inner;

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    tb_string_init(&(result->String));

    tb_string_cstrcat(&(result->String), "@");

    KN innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    tb_string_strcat(&(result->String), &(KON_UNBOX_STRING(innerToKonStr)));

    return result;
}

KN KON_QuasiquoteStringify(KonState* kstate, KonQuasiquote* source, bool newLine, int depth, char* padding)
{
    KonQuasiquoteType type = source->Type;
    KN inner = source->Inner;

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    tb_string_init(&(result->String));

    tb_string_cstrcat(&(result->String), "$");

    KN innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    tb_string_strcat(&(result->String), &(KON_UNBOX_STRING(innerToKonStr)));

    return (KN)result;
}

KN KON_ExpandStringify(KonState* kstate, KonExpand* source, bool newLine, int depth, char* padding)
{
    KonExpandType type = source->Type;
    KN inner = source->Inner;

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    tb_string_init(&(result->String));

    tb_string_cstrcat(&(result->String), "$");

    switch (type) {
        case KON_EXPAND_VECTOR: {
            tb_string_cstrcat(&(result->String), "[]");
            break;
        }
        case KON_EXPAND_LIST: {
            tb_string_cstrcat(&(result->String), "{}");
            break;
        }
        case KON_EXPAND_TABLE: {
            tb_string_cstrcat(&(result->String), "()");
            break;
        }
    }

    tb_string_cstrcat(&(result->String), ".");

    KN innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    tb_string_strcat(&(result->String), &(KON_UNBOX_STRING(innerToKonStr)));

    return result;
}

KN KON_UnquoteStringify(KonState* kstate, KonUnquote* source, bool newLine, int depth, char* padding)
{
    KonUnquoteType type = source->Type;
    KN inner = source->Inner;

    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    tb_string_init(&(result->String));

    tb_string_cstrcat(&(result->String), "$");

    switch (type) {
        case KON_UNQUOTE_VECTOR: {
            tb_string_cstrcat(&(result->String), "[]");
            break;
        }
        case KON_UNQUOTE_LIST: {
            tb_string_cstrcat(&(result->String), "{}");
            break;
        }
        case KON_UNQUOTE_TABLE: {
            tb_string_cstrcat(&(result->String), "()");
            break;
        }
    }

    tb_string_cstrcat(&(result->String), "e.");

    KN innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    tb_string_strcat(&(result->String), &(KON_UNBOX_STRING(innerToKonStr)));

    return result;
}


// the parent node add the first left padding
// don't add newline when stringify sub container types.
// add newline in parent node


KN KON_VectorStringify(KonState* kstate, KonVector* source, bool newLine, int depth, char* padding)
{
    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    tb_string_init(&(result->String));

    tb_vector_ref_t items = source->Vector;
    tb_size_t itor = tb_iterator_head(items);
    tb_size_t tail = tb_iterator_tail(items);
    
    
    
    if (newLine) {
        tb_string_cstrcat(&(result->String), "[");
        tb_string_cstrcat(&(result->String), "\n");

        for (; itor != tail; itor = tb_iterator_next(items, itor)) {
            KN item = (KN)tb_iterator_item(items, itor);
            KN itemToKonStr = KON_ToFormatString(kstate, item, true, depth + 1, padding);

            AddLeftPadding(&(result->String), depth, padding);
            tb_string_cstrcat(&(result->String), padding);
            tb_string_strcat(&(result->String), &(KON_UNBOX_STRING(itemToKonStr)));
            tb_string_cstrcat(&(result->String), "\n");
        }

        AddLeftPadding(&(result->String), depth, padding);
        tb_string_cstrcat(&(result->String), "]");
    }
    else {
        tb_string_cstrcat(&(result->String), "[");
        
        while (itor != tb_iterator_tail(items)) {
            tb_size_t next = tb_iterator_next(items, itor);
            
            KN item = (KN)tb_iterator_item(items, itor);
            KN itemToKonStr = KON_ToFormatString(kstate, item, false, depth + 1, padding);
            tb_string_strcat(&(result->String), &(KON_UNBOX_STRING(itemToKonStr)));

            if (next != tb_iterator_tail(items)) {
                tb_string_cstrcat(&(result->String), " ");
            }
            
            itor = next;
        }

        tb_string_cstrcat(&(result->String), "]");
    }

    return (KN)result;
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
    tb_string_init(&(result->String));

    
    if (newLine) {
        tb_string_cstrcat(&(result->String), "{");
        tb_string_cstrcat(&(result->String), "\n");
        if (source != KON_NIL && kon_is_list_node(source)) {
            KonListNode* iter = kon_cdr(source);
            KN item = kon_car(source);
            
            // print first item
            KN itemToKonStr = KON_ToFormatString(kstate, item, true, depth + 1, padding);
            AddLeftPadding(&(result->String), depth, padding);
            tb_string_cstrcat(&(result->String), padding);
            
            tb_string_strcat(&(result->String), &(KON_UNBOX_STRING(itemToKonStr)));
            tb_string_cstrcat(&(result->String), "\n");
            
            // format rest item
            while (iter != KON_NIL) {
                item = kon_car(iter);
                iter = kon_cdr(iter);
                
                KN itemToKonStr = KON_ToFormatString(kstate, item, true, depth + 1, padding);
                
                AddLeftPadding(&(result->String), depth, padding);
                tb_string_cstrcat(&(result->String), padding);
                tb_string_strcat(&(result->String), &(KON_UNBOX_STRING(itemToKonStr)));
                tb_string_cstrcat(&(result->String), "\n");
            }
        }

        AddLeftPadding(&(result->String), depth, padding);
        tb_string_cstrcat(&(result->String), "}");
    }
    else {
        tb_string_cstrcat(&(result->String), "{");
        
        if (source != KON_NIL && kon_is_list_node(source)) {
            KonListNode* iter = kon_cdr(source);
            KN item = kon_car(source);
            
            KN itemToKonStr = KON_ToFormatString(kstate, item, false, depth + 1, padding);
            tb_string_strcat(&(result->String), &(KON_UNBOX_STRING(itemToKonStr)));
            tb_string_cstrcat(&(result->String), " ");
            
            while (iter != KON_NIL) {
                item = kon_car(iter);
                iter = kon_cdr(iter);
                
                KN itemToKonStr = KON_ToFormatString(kstate, item, false, depth + 1, padding);
                tb_string_strcat(&(result->String), &(KON_UNBOX_STRING(itemToKonStr)));
                if (iter != KON_NIL) {
                    tb_string_cstrcat(&(result->String), " ");
                }
            }
        }

        tb_string_cstrcat(&(result->String), "}");
    }

    return result;
}

KonListNode* Kon_ListRevert(KonState* kstate, KonListNode* source)
{
    KN result = KON_NIL;
    if (source != KON_NIL && kon_is_list_node(source)) {
        KonListNode* iter = source;
        do {
            KN item = kon_car(iter);
            
            result = kon_cons(kstate, item, result);
            
            iter = kon_cdr(iter);
        } while (iter != KON_NIL);
    }
    return result;
}

KonListNode* KON_Cons(KonState* kstate, KN self, kon_int_t n, KN head, KonListNode* tail)
{
  KonListNode* node = KON_ALLOC_TYPE_TAG(kstate, KonListNode, KON_T_LIST_NODE);
  if (kon_is_exception(node)) {
      return node;
  }
  node->Prev = KON_NIL;
  if (kon_is_list_node(tail)) {
      tail->Prev = node;
  }
  kon_car(node) = head;
  kon_cdr(node) = tail;
  return node;
}

KonListNode* KON_List2(KonState* kstate, KN a, KN b)
{
  KN res = kon_cons(kstate, b, KON_NIL);
  res = kon_cons(kstate, a, res);
  return res;
}

KonListNode* KON_List3(KonState* kstate, KN a, KN b, KN c)
{
  KN res = KON_List2(kstate, b, c);
  res = kon_cons(kstate, a, res);
  return res;
}



KN KON_TableStringify(KonState* kstate, KonTable* source, bool newLine, int depth, char* padding)
{
    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    tb_string_init(&(result->String));

    KonHashMap* hashMap = source->Table;
    KonHashMapIter* iter = KON_HashMapIterHead(hashMap);

    if (newLine) {
        tb_string_cstrcat(&(result->String), "(");
        tb_string_cstrcat(&(result->String), "\n");


        while (iter) {
            KonHashMapIter* next = KON_HashMapIterNext(hashMap, iter);
            char* itemKey = KON_HashMapIterItemKey(hashMap, iter);
            KN itemValue = (KN)KON_HashMapIterItemValue(hashMap, iter);

            KN itemToKonStr = KON_ToFormatString(kstate, itemValue, true, depth + 1, padding);

            AddLeftPadding(&(result->String), depth, padding);
            tb_string_cstrcat(&(result->String), ":'");
            tb_string_cstrcat(&(result->String), itemKey);
            tb_string_cstrcat(&(result->String), "'");
            tb_string_cstrcat(&(result->String), "\n");

            AddLeftPadding(&(result->String), depth, padding);
            tb_string_cstrcat(&(result->String), padding);

            tb_string_cstrcat(&(result->String), KON_StringToCstr(itemToKonStr));
            tb_string_cstrcat(&(result->String), "\n");

            iter = next;
        }

        AddLeftPadding(&(result->String), depth, padding);
        tb_string_cstrcat(&(result->String), ")");
    }
    else {
        tb_string_cstrcat(&(result->String), "(");

        while (iter) {
            KonHashMapIter* next = KON_HashMapIterNext(hashMap, iter);
            char* itemKey = KON_HashMapIterItemKey(hashMap, iter);
            KN itemValue = (KN)KON_HashMapIterItemValue(hashMap, iter);

            KN itemToKonStr = KON_ToFormatString(kstate, itemValue, false, depth + 1, padding);

            tb_string_cstrcat(&(result->String), ":'");
            tb_string_cstrcat(&(result->String), itemKey);
            tb_string_cstrcat(&(result->String), "'");
            tb_string_cstrcat(&(result->String), " ");

            tb_string_cstrcat(&(result->String), KON_StringToCstr(itemToKonStr));
            
            if (next != NULL) {
                tb_string_cstrcat(&(result->String), " ");
            }

            iter = next;
        }
        tb_string_cstrcat(&(result->String), ")");
    }

    return result;
}



KN KON_CellStringify(KonState* kstate, KonCell* source, bool newLine, int depth, char* padding)
{
    KonString* result = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    tb_string_init(&(result->String));

    KN name = source->Name;
    KonVector* innerVector = source->Vector;
    KonTable* innerTable = source->Table;
    KonListNode* innerList = source->List;
    
    if (newLine) {
        tb_string_cstrcat(&(result->String), "<");
        
        if (name != KON_NULL) {
            KN nameToKonStr = KON_ToFormatString(kstate, name, true, depth, padding);
            tb_string_strcat(&(result->String), &KON_UNBOX_STRING(nameToKonStr));
        }
        tb_string_cstrcat(&(result->String), "\n");

        AddLeftPadding(&(result->String), depth, padding);
        tb_string_cstrcat(&(result->String), padding);
        if (innerTable != KON_NULL) {
            KN innerTableToKonStr = KON_ToFormatString(kstate, innerTable, true, depth + 1, padding);
            tb_string_cstrcat(&(result->String), KON_StringToCstr(innerTableToKonStr));
            tb_string_cstrcat(&(result->String), "\n");
        }
        

        AddLeftPadding(&(result->String), depth, padding);
        tb_string_cstrcat(&(result->String), padding);
        if (innerVector != KON_NULL) {
            KN innerVectorToKonStr = KON_ToFormatString(kstate, innerVector, true, depth + 1, padding);
            tb_string_cstrcat(&(result->String), KON_StringToCstr(innerVectorToKonStr));
            tb_string_cstrcat(&(result->String), "\n");
        }
        


        AddLeftPadding(&(result->String), depth, padding);
        tb_string_cstrcat(&(result->String), padding);
        if (innerList != KON_NULL) {
            KN innerListToKonStr = KON_ToFormatString(kstate, innerList, true, depth + 1, padding);
            tb_string_cstrcat(&(result->String), KON_StringToCstr(innerListToKonStr));
            tb_string_cstrcat(&(result->String), "\n");
        }
        

        AddLeftPadding(&(result->String), depth, padding);
        tb_string_cstrcat(&(result->String), ">");
    }
    else {
        tb_string_cstrcat(&(result->String), "<");

        if (name != KON_NULL) {
            KN nameToKonStr = KON_ToFormatString(kstate, name, true, depth, padding);
            tb_string_strcat(&(result->String), &KON_UNBOX_STRING(nameToKonStr));
            
        }

        
        if (innerTable != KON_NULL) {
            tb_string_cstrcat(&(result->String), " ");
            KN innerTableToKonStr = KON_ToFormatString(kstate, innerTable, false, depth + 1, padding);
            tb_string_cstrcat(&(result->String), KON_StringToCstr(innerTableToKonStr));
            
        }

        
        if (innerVector != KON_NULL) {
            tb_string_cstrcat(&(result->String), " ");
            KN innerVectorToKonStr = KON_ToFormatString(kstate, innerVector, false, depth + 1, padding);
            tb_string_cstrcat(&(result->String), KON_StringToCstr(innerVectorToKonStr));
            
        }

        if (innerList != KON_NULL) {
            tb_string_cstrcat(&(result->String), " ");
            KN innerListToKonStr = KON_ToFormatString(kstate, innerList, false, depth + 1, padding);
            tb_string_cstrcat(&(result->String), KON_StringToCstr(innerListToKonStr));
        }

        tb_string_cstrcat(&(result->String), ">");
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
