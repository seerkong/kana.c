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
    // Kon* data = (Kon*)(*((tb_pointer_t*)buff));

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

    // Kon* data = (Kon*)(*((tb_pointer_t*)buff));

    // TODO release
    // if (data) {
    //     tb_free(data);
    // }

    // TODO clear
    // *((tb_pointer_t*)buff) = tb_null;
}

Kon* KON_AllocTagged(Kon* kstate, size_t size, kon_uint_t tag)
{
  Kon* res = (Kon*) kon_alloc(kstate, size);
  if (res && ! kon_is_exception(res)) {
    kon_pointer_tag(res) = tag;
  }
  return res;
}


Kon* KON_ToFormatString(Kon* kstate, Kon* source, bool newLine, int depth, char* padding)
{
    if (kon_is_fixnum(source)) {
        return KON_FixnumStringify(kstate, source);
    }
    else if (kon_is_flonum(source)) {
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

Kon* KON_Stringify(Kon* kstate, Kon* source)
{
    return KON_ToFormatString(kstate, source, false, 0, " ");
}

bool IsContainerOrWrapper()
{

}

// number related
Kon* KON_FixnumStringify(Kon* kstate, Kon* source)
{
    if (!kon_is_fixnum(source)) {
        return KON_MakeEmptyString(kstate);
    }
    char buf[128] = { '\0' };
    kon_int_t num = kon_unbox_fixnum(source);
    itoa(num, buf, 10);

    Kon* value = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(value->Value.String));
    tb_string_cstrcat(&(value->Value.String), buf);

    return value;
}

Kon* KON_MakeFlonum(Kon* kstate, double f)
{
  Kon* x = kon_alloc_type(kstate, Flonum, KON_FLONUM);
  if (kon_is_exception(x)) {
      return x;
  }
  kon_flonum_value(x) = f;
  return x;
}

Kon* KON_FlonumStringify(Kon* kstate, Kon* source)
{
    if (!kon_is_flonum(source)) {
        return KON_MakeEmptyString(kstate);
    }
    char buf[128] = {'\0'};
    double num = kon_flonum_value(source);
    double_to_str(num, 2, buf);

    Kon* value = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(value->Value.String));
    tb_string_cstrcat(&(value->Value.String), buf);
    return value;
}

// 转换char为 #c,A;
Kon* KON_CharStringify(Kon* kstate, Kon* source)
{
    if (!kon_is_char(source)) {
        return KON_MakeEmptyString(kstate);
    }

    Kon* value = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(value->Value.String));
    tb_string_cstrcat(&(value->Value.String), "#c,");
    int charcode = kon_unbox_character(source);
    char buf[10] = {'\0'};
    snprintf(buf, 10, "%c", charcode);
    tb_string_cstrcat(&(value->Value.String), buf);
    tb_string_cstrcat(&(value->Value.String), ";");
    return value;
}

Kon* KON_StringStringify(Kon* kstate, Kon* source)
{
    Kon* value = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(value->Value.String));
    tb_string_cstrcat(&(value->Value.String), "\"");
    tb_string_strcat(&(value->Value.String), &(source->Value.String));
    tb_string_cstrcat(&(value->Value.String), "\"");
    return value;
}

Kon* KON_MakeString(Kon* kstate, const char* str)
{
    Kon* value = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(value->Value.String));
    tb_string_cstrcat(&(value->Value.String), str);
    return value;
}

Kon* KON_MakeEmptyString(Kon* kstate)
{
    Kon* value = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(value->Value.String));
    return value;
}

const char* KON_StringToCstr(Kon* str)
{
    if (!kon_is_string(str)) {
        return NULL;
    }
    return tb_string_cstr(&(str->Value.String));
}

void AddLeftPadding(tb_string_ref_t str, int depth, char* padding)
{
    for (int i = 0; i < depth; i++) {
        tb_string_cstrcat(str, padding);
    }
}

Kon* KON_SymbolStringify(Kon* kstate, Kon* source)
{
    KonSymbolType type = source->Value.Symbol.Type;
    tb_string_ref_t data = &source->Value.Symbol.Data;

    Kon* result = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(result->Value.String));

    switch (type) {
        case KON_SYM_IDENTIFER: {
            tb_string_strcat(&(result->Value.String), data);
            break;
        }
        case KON_SYM_STRING: {
            tb_string_cstrcat(&(result->Value.String), "\'");
            tb_string_strcat(&(result->Value.String), data);
            tb_string_cstrcat(&(result->Value.String), "\'");
            break;
        }
        case KON_SYM_VAR: {
            tb_string_cstrcat(&(result->Value.String), "$");
            tb_string_strcat(&(result->Value.String), data);
            break;
        }
        case KON_SYM_FORM_WORD: {
            tb_string_cstrcat(&(result->Value.String), "!");
            tb_string_strcat(&(result->Value.String), data);
            break;
        }
    }
    return result;
}

const char* KON_SymbolToCstr(Kon* sym)
{
    // TODO assert
    return tb_string_cstr(&sym->Value.Symbol.Data);
}

Kon* KON_SyntaxMarkerStringify(Kon* kstate, Kon* source)
{
    KonSyntaxMarkerType type = source->Value.SyntaxMarker.Type;

    Kon* result = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(result->Value.String));

    switch (type) {
        case KON_SYNTAX_MARKER_APPLY: {
            tb_string_cstrcat(&(result->Value.String), "%");
            break;
        }
        case KON_SYNTAX_MARKER_EXEC_MSG: {
            tb_string_cstrcat(&(result->Value.String), ".");
            break;
        }
        case KON_SYNTAX_MARKER_PIPE: {
            tb_string_cstrcat(&(result->Value.String), "|");
            break;
        }
        case KON_SYNTAX_MARKER_CLAUSE_END: {
            tb_string_cstrcat(&(result->Value.String), ";");
            break;
        }
    }
    return result;
}

Kon* KON_QuoteStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding)
{
    KonQuoteType type = source->Value.Quote.Type;
    Kon* inner = source->Value.Quote.Inner;

    Kon* result = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(result->Value.String));

    tb_string_cstrcat(&(result->Value.String), "@");

    Kon* innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    tb_string_strcat(&(result->Value.String), &(innerToKonStr->Value.String));

    return result;
}

Kon* KON_QuasiquoteStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding)
{
    KonQuasiquoteType type = source->Value.Quasiquote.Type;
    Kon* inner = source->Value.Quasiquote.Inner;

    Kon* result = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(result->Value.String));

    tb_string_cstrcat(&(result->Value.String), "$");

    Kon* innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    tb_string_strcat(&(result->Value.String), &(innerToKonStr->Value.String));

    return result;
}

Kon* KON_ExpandStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding)
{
    KonExpandType type = source->Value.Expand.Type;
    Kon* inner = source->Value.Expand.Inner;

    Kon* result = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(result->Value.String));

    tb_string_cstrcat(&(result->Value.String), "$");

    switch (type) {
        case KON_EXPAND_VECTOR: {
            tb_string_cstrcat(&(result->Value.String), "[]");
            break;
        }
        case KON_EXPAND_LIST: {
            tb_string_cstrcat(&(result->Value.String), "{}");
            break;
        }
        case KON_EXPAND_TABLE: {
            tb_string_cstrcat(&(result->Value.String), "()");
            break;
        }
    }

    tb_string_cstrcat(&(result->Value.String), ".");

    Kon* innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    tb_string_strcat(&(result->Value.String), &(innerToKonStr->Value.String));

    return result;
}

Kon* KON_UnquoteStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding)
{
    KonUnquoteType type = source->Value.Unquote.Type;
    Kon* inner = source->Value.Unquote.Inner;

    Kon* result = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(result->Value.String));

    tb_string_cstrcat(&(result->Value.String), "$");

    switch (type) {
        case KON_UNQUOTE_VECTOR: {
            tb_string_cstrcat(&(result->Value.String), "[]");
            break;
        }
        case KON_UNQUOTE_LIST: {
            tb_string_cstrcat(&(result->Value.String), "{}");
            break;
        }
        case KON_UNQUOTE_TABLE: {
            tb_string_cstrcat(&(result->Value.String), "()");
            break;
        }
    }

    tb_string_cstrcat(&(result->Value.String), "e.");

    Kon* innerToKonStr = KON_ToFormatString(kstate, inner, newLine, depth, padding);
    tb_string_strcat(&(result->Value.String), &(innerToKonStr->Value.String));

    return result;
}


// the parent node add the first left padding
// don't add newline when stringify sub container types.
// add newline in parent node


Kon* KON_VectorStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding)
{
    Kon* result = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(result->Value.String));

    tb_vector_ref_t items = source->Value.Vector;
    tb_size_t itor = tb_iterator_head(items);
    tb_size_t tail = tb_iterator_tail(items);
    
    
    
    if (newLine) {
        tb_string_cstrcat(&(result->Value.String), "[");
        tb_string_cstrcat(&(result->Value.String), "\n");

        for (; itor != tail; itor = tb_iterator_next(items, itor)) {
            Kon* item = (Kon*)tb_iterator_item(items, itor);
            Kon* itemToKonStr = KON_ToFormatString(kstate, item, true, depth + 1, padding);

            AddLeftPadding(&(result->Value.String), depth, padding);
            tb_string_cstrcat(&(result->Value.String), padding);
            tb_string_strcat(&(result->Value.String), &(itemToKonStr->Value.String));
            tb_string_cstrcat(&(result->Value.String), "\n");
        }

        AddLeftPadding(&(result->Value.String), depth, padding);
        tb_string_cstrcat(&(result->Value.String), "]");
    }
    else {
        tb_string_cstrcat(&(result->Value.String), "[");
        
        while (itor != tb_iterator_tail(items)) {
            tb_size_t next = tb_iterator_next(items, itor);
            
            Kon* item = (Kon*)tb_iterator_item(items, itor);
            Kon* itemToKonStr = KON_ToFormatString(kstate, item, false, depth + 1, padding);
            tb_string_strcat(&(result->Value.String), &(itemToKonStr->Value.String));

            if (next != tb_iterator_tail(items)) {
                tb_string_cstrcat(&(result->Value.String), " ");
            }
            
            itor = next;
        }

        tb_string_cstrcat(&(result->Value.String), "]");
    }

    return result;
}

bool KON_IsList(Kon* source)
{
    Kon* iter = source;
    bool isList = true;
    do {
        if (iter == KON_NIL) {
            break;
        }
        if (!kon_is_pair(iter)) {
            isList = false;
            break;
        }
        iter = kon_cdr(iter);
    } while (iter != KON_NIL);
    return isList;
}

Kon* KON_ListStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding)
{
    assert(KON_IsList(source));
    Kon* result = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(result->Value.String));

    
    if (newLine) {
        tb_string_cstrcat(&(result->Value.String), "{");
        tb_string_cstrcat(&(result->Value.String), "\n");
        if (source != KON_NIL && kon_is_pair(source)) {
            Kon* iter = kon_cdr(source);
            Kon* item = kon_car(source);
            
            // print first item
            Kon* itemToKonStr = KON_ToFormatString(kstate, item, true, depth + 1, padding);
            AddLeftPadding(&(result->Value.String), depth, padding);
            tb_string_cstrcat(&(result->Value.String), padding);
            
            tb_string_strcat(&(result->Value.String), &(itemToKonStr->Value.String));
            tb_string_cstrcat(&(result->Value.String), "\n");
            
            // format rest item
            while (iter != KON_NIL) {
                item = kon_car(iter);
                iter = kon_cdr(iter);
                
                Kon* itemToKonStr = KON_ToFormatString(kstate, item, true, depth + 1, padding);
                
                AddLeftPadding(&(result->Value.String), depth, padding);
                tb_string_cstrcat(&(result->Value.String), padding);
                tb_string_strcat(&(result->Value.String), &(itemToKonStr->Value.String));
                tb_string_cstrcat(&(result->Value.String), "\n");
            }
        }

        AddLeftPadding(&(result->Value.String), depth, padding);
        tb_string_cstrcat(&(result->Value.String), "}");
    }
    else {
        tb_string_cstrcat(&(result->Value.String), "{");
        
        if (source != KON_NIL && kon_is_pair(source)) {
            Kon* iter = kon_cdr(source);
            Kon* item = kon_car(source);
            
            Kon* itemToKonStr = KON_ToFormatString(kstate, item, false, depth + 1, padding);
            tb_string_strcat(&(result->Value.String), &(itemToKonStr->Value.String));
            tb_string_cstrcat(&(result->Value.String), " ");
            
            while (iter != KON_NIL) {
                item = kon_car(iter);
                iter = kon_cdr(iter);
                
                Kon* itemToKonStr = KON_ToFormatString(kstate, item, false, depth + 1, padding);
                tb_string_strcat(&(result->Value.String), &(itemToKonStr->Value.String));
                if (iter != KON_NIL) {
                    tb_string_cstrcat(&(result->Value.String), " ");
                }
            }
        }

        tb_string_cstrcat(&(result->Value.String), "}");
    }

    return result;
}

Kon* Kon_ListRevert(Kon* kstate, Kon* source)
{
    Kon* result = KON_NIL;
    if (source != KON_NIL && kon_is_pair(source)) {
        Kon* iter = source;
        do {
            Kon* item = kon_car(iter);
            
            result = kon_cons(kstate, item, result);
            
            iter = kon_cdr(iter);
        } while (iter != KON_NIL);
    }
    return result;
}

Kon* KON_Cons(Kon* kstate, Kon* self, kon_int_t n, Kon* head, Kon* tail)
{
  Kon* pair = kon_alloc_type(kstate, Pair, KON_PAIR);
  if (kon_is_exception(pair)) {
      return pair;
  }
  kon_car(pair) = head;
  kon_cdr(pair) = tail;
  return pair;
}

Kon* KON_List2(Kon* kstate, Kon* a, Kon* b)
{
  Kon* res = kon_cons(kstate, b, KON_NULL);
  res = kon_cons(kstate, a, res);
  return res;
}

Kon* KON_List3(Kon* kstate, Kon* a, Kon* b, Kon* c)
{
  Kon* res = KON_List2(kstate, b, c);
  res = kon_cons(kstate, a, res);
  return res;
}



Kon* KON_TableStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding)
{
    Kon* result = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(result->Value.String));

    tb_hash_map_ref_t hashmap = source->Value.Table;
    tb_size_t itor = tb_iterator_head(hashmap);
    
    if (newLine) {
        tb_string_cstrcat(&(result->Value.String), "(");
        tb_string_cstrcat(&(result->Value.String), "\n");


        while (itor != tb_iterator_tail(hashmap)) {
            tb_hash_map_item_ref_t hashItem = (tb_hash_map_item_ref_t)tb_iterator_item(hashmap, itor);
            
            tb_size_t next = tb_iterator_next(hashmap, itor);
            char* itemKey = (char*)hashItem->name;
            Kon* itemValue = (Kon*)hashItem->data;
            Kon* itemToKonStr = KON_ToFormatString(kstate, itemValue, true, depth + 1, padding);

            AddLeftPadding(&(result->Value.String), depth, padding);
            tb_string_cstrcat(&(result->Value.String), ":'");
            tb_string_cstrcat(&(result->Value.String), itemKey);
            tb_string_cstrcat(&(result->Value.String), "'");
            tb_string_cstrcat(&(result->Value.String), "\n");

            AddLeftPadding(&(result->Value.String), depth, padding);
            tb_string_cstrcat(&(result->Value.String), padding);

            tb_string_cstrcat(&(result->Value.String), KON_StringToCstr(itemToKonStr));
            tb_string_cstrcat(&(result->Value.String), "\n");

            itor = next;
        }

        AddLeftPadding(&(result->Value.String), depth, padding);
        tb_string_cstrcat(&(result->Value.String), ")");
    }
    else {
        tb_string_cstrcat(&(result->Value.String), "(");

        while (itor != tb_iterator_tail(hashmap)) {
            tb_hash_map_item_ref_t hashItem = (tb_hash_map_item_ref_t)tb_iterator_item(hashmap, itor);
            
            tb_size_t next = tb_iterator_next(hashmap, itor);
            char* itemKey = (char*)hashItem->name;
            Kon* itemValue = (Kon*)hashItem->data;
            Kon* itemToKonStr = KON_ToFormatString(kstate, itemValue, false, depth + 1, padding);

            tb_string_cstrcat(&(result->Value.String), ":'");
            tb_string_cstrcat(&(result->Value.String), itemKey);
            tb_string_cstrcat(&(result->Value.String), "'");
            tb_string_cstrcat(&(result->Value.String), " ");

            tb_string_cstrcat(&(result->Value.String), KON_StringToCstr(itemToKonStr));
            
            if (next != tb_iterator_tail(hashmap)) {
                tb_string_cstrcat(&(result->Value.String), " ");
            }

            itor = next;
        }
        tb_string_cstrcat(&(result->Value.String), ")");
    }

    return result;
}



Kon* KON_CellStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding)
{
    Kon* result = KON_AllocTagged(kstate, sizeof(tb_string_t), KON_STRING);
    tb_string_init(&(result->Value.String));

    Kon* subj = source->Value.Cell.Subj;
    Kon* innerVector = source->Value.Cell.Vector;
    Kon* innerTable = source->Value.Cell.Table;
    Kon* innerList = source->Value.Cell.List;
    
    if (newLine) {
        tb_string_cstrcat(&(result->Value.String), "<");
        
        if (subj != KON_NULL) {
            Kon* subjToKonStr = KON_ToFormatString(kstate, subj, true, depth, padding);
            tb_string_strcat(&(result->Value.String), &subjToKonStr->Value.String);
        }
        tb_string_cstrcat(&(result->Value.String), "\n");

        AddLeftPadding(&(result->Value.String), depth, padding);
        tb_string_cstrcat(&(result->Value.String), padding);
        if (innerTable != KON_NULL) {
            Kon* innerTableToKonStr = KON_ToFormatString(kstate, innerTable, true, depth + 1, padding);
            tb_string_cstrcat(&(result->Value.String), KON_StringToCstr(innerTableToKonStr));
            tb_string_cstrcat(&(result->Value.String), "\n");
        }
        

        AddLeftPadding(&(result->Value.String), depth, padding);
        tb_string_cstrcat(&(result->Value.String), padding);
        if (innerVector != KON_NULL) {
            Kon* innerVectorToKonStr = KON_ToFormatString(kstate, innerVector, true, depth + 1, padding);
            tb_string_cstrcat(&(result->Value.String), KON_StringToCstr(innerVectorToKonStr));
            tb_string_cstrcat(&(result->Value.String), "\n");
        }
        


        AddLeftPadding(&(result->Value.String), depth, padding);
        tb_string_cstrcat(&(result->Value.String), padding);
        if (innerList != KON_NULL) {
            Kon* innerListToKonStr = KON_ToFormatString(kstate, innerList, true, depth + 1, padding);
            tb_string_cstrcat(&(result->Value.String), KON_StringToCstr(innerListToKonStr));
            tb_string_cstrcat(&(result->Value.String), "\n");
        }
        

        AddLeftPadding(&(result->Value.String), depth, padding);
        tb_string_cstrcat(&(result->Value.String), ">");
    }
    else {
        tb_string_cstrcat(&(result->Value.String), "<");

        if (subj != KON_NULL) {
            Kon* subjToKonStr = KON_ToFormatString(kstate, subj, true, depth, padding);
            tb_string_strcat(&(result->Value.String), &subjToKonStr->Value.String);
            
        }

        
        if (innerTable != KON_NULL) {
            tb_string_cstrcat(&(result->Value.String), " ");
            Kon* innerTableToKonStr = KON_ToFormatString(kstate, innerTable, false, depth + 1, padding);
            tb_string_cstrcat(&(result->Value.String), KON_StringToCstr(innerTableToKonStr));
            
        }

        
        if (innerVector != KON_NULL) {
            tb_string_cstrcat(&(result->Value.String), " ");
            Kon* innerVectorToKonStr = KON_ToFormatString(kstate, innerVector, false, depth + 1, padding);
            tb_string_cstrcat(&(result->Value.String), KON_StringToCstr(innerVectorToKonStr));
            
        }

        if (innerList != KON_NULL) {
            tb_string_cstrcat(&(result->Value.String), " ");
            Kon* innerListToKonStr = KON_ToFormatString(kstate, innerList, false, depth + 1, padding);
            tb_string_cstrcat(&(result->Value.String), KON_StringToCstr(innerListToKonStr));
        }

        tb_string_cstrcat(&(result->Value.String), ">");
    }

    return result;
}

Kon* MakeNativeProcedure(Kon* kstate, KonProcedureType type, KonNativeFuncRef funcRef)
{
    Kon* result = KON_AllocTagged(kstate, sizeof(KonProcedure), KON_PROCEDURE);
    result->Value.Procedure.Type = type;
    result->Value.Procedure.NativeFuncRef = funcRef;
    return result;
}
