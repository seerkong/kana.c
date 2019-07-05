#ifndef KON_KSON_NODE_H
#define KON_KSON_NODE_H

#include <stdio.h>
#include <tbox/tbox.h>
#include "../prefix/config.h"


////
// type alias start

#if KON_64_BIT
typedef unsigned int kon_tag_t;
typedef unsigned long long kon_uint_t;
typedef long long kon_int_t;
#else
typedef unsigned short kon_tag_t;
typedef unsigned int kon_uint_t;
typedef int kon_int_t;
#endif

typedef unsigned char kon_uint8_t;
typedef unsigned int kon_uint32_t;
typedef int kon_int32_t;


// type alias end
////





////
// tagging system start

// tagging system
//   bits end in     1:  fixnum
//                  00:  pointer
//                 010:  string cursor (optional)
//                0110:  immediate symbol (optional)
//            00001110:  immediate flonum (optional)
//            00011110:  char
//            00101110:  reader label (optional)
//            00111110:  unique immediate (NULL, TRUE, FALSE)

#define KON_FIXNUM_BITS 1
#define KON_POINTER_BITS 2
#define KON_STRING_CURSOR_BITS 3
#define KON_IMMEDIATE_BITS 4
#define KON_EXTENDED_BITS 8

#define KON_FIXNUM_MASK ((1<<KON_FIXNUM_BITS)-1)
#define KON_POINTER_MASK ((1<<KON_POINTER_BITS)-1)
#define KON_STRING_CURSOR_MASK ((1<<KON_STRING_CURSOR_BITS)-1)
#define KON_IMMEDIATE_MASK ((1<<KON_IMMEDIATE_BITS)-1)
#define KON_EXTENDED_MASK ((1<<KON_EXTENDED_BITS)-1)

#define KON_POINTER_TAG 0
#define KON_FIXNUM_TAG 1
#define KON_STRING_CURSOR_TAG 2
#define KON_ISYMBOL_TAG 6
#define KON_IFLONUM_TAG 14
#define KON_CHAR_TAG 30
#define KON_READER_LABEL_TAG 46
#define KON_EXTENDED_TAG 62

// 00111110前面的高位字节作为n
// 2: 1000111110
// 比如3： 1100111110
// 8: 100000111110
#define KON_MAKE_IMMEDIATE(n)  ((Kon*) ((n<<KON_EXTENDED_BITS) \
                                          + KON_EXTENDED_TAG))

#define KON_FALSE  KON_MAKE_IMMEDIATE(0) /* 14 0x0e */
#define KON_TRUE   KON_MAKE_IMMEDIATE(1) /* 30 0x1e */
#define KON_NULL   KON_MAKE_IMMEDIATE(2) /* 46 0x2e, container placeholder */
#define KON_EOF    KON_MAKE_IMMEDIATE(3) /* 62 0x3e */
#define KON_NIL   KON_MAKE_IMMEDIATE(4) /*  0x43e empyt list */
#define KON_UKN  KON_MAKE_IMMEDIATE(5) /* unknown, undefined */

#define kon_align(n, bits) (((n)+(1<<(bits))-1)&(((kon_uint_t)-1)-((1<<(bits))-1)))

#if KON_64_BIT
#define kon_word_align(n) kon_align((n), 3)
#else
#define kon_word_align(n) kon_align((n), 2)
#endif

#define kon_sizeof(x) (offsetof(struct KonStruct, Value) + sizeof(((Kon*)0)->Value.x))
#define kon_offsetof(type, f) (offsetof(struct KonStruct, value.type.f))

#define kon_alloc_type(kstate, type, tag) KON_AllocTagged(kstate, kon_sizeof(type), tag)
#define kon_alloc_bytecode(kstate, i) KON_AllocTagged(kstate, kon_sizeof(bytecode) + i, KON_BYTECODE)


// tagging system end
////

////
// types start
typedef enum {
    KON_NUMBER,
    KON_FIXNUM,
    KON_FLONUM,
    KON_BIGNUM,
    KON_CHAR,
    KON_BOOLEAN,
    KON_PAIR,
    KON_SYMBOL,
    KON_SYNTAX_MARKER,
    KON_BYTES,
    KON_STRING,
    KON_VECTOR,
    KON_TABLE,
    KON_CELL,
    KON_QUOTE,
    KON_QUASIQUOTE,
    KON_EXPAND,
    KON_UNQUOTE,
    KON_ENV,
    KON_CPOINTER,
    KON_EXCEPTION
} KonType;

typedef enum {
    KON_SYM_IDENTIFER,  // abc
    KON_SYM_STRING, // ''
    KON_SYM_VAR,    // $abc
    KON_SYM_FORM_WORD // !ass
} KonSymbolType;

typedef enum {
    KON_QUOTE_IDENTIFER,    // @abc
    KON_QUOTE_SYMBOL,       // @'zhang san'
    KON_QUOTE_VECTOR,        // @[1 2 3]
    KON_QUOTE_LIST,         // @{1 2 3}
    KON_QUOTE_TABLE,        // @(:a 1 :b 2)
    KON_QUOTE_CELL          // @<ojb (:a 1 :b 2)>
} KonQuoteType;

typedef enum {
    KON_QUASI_VECTOR,        // $[1 2 3]
    KON_QUASI_LIST,         // ${1 2 3}
    KON_QUASI_TABLE,        // $(:a 1 :b 2)
    KON_QUASI_CELL          // $<ojb (:a 1 :b 2)>
} KonQuasiquoteType;

typedef enum {
    KON_EXPAND_REPLACE,          // $.abc
    KON_EXPAND_VECTOR,        // $[].[1 2 3]
    KON_EXPAND_LIST,         // ${}.{1 2 3}
    KON_EXPAND_TABLE        // $().(:a 1 :b 2)
} KonExpandType;

typedef enum {
    KON_UNQUOTE_REPLACE,          // $e.abc
    KON_UNQUOTE_VECTOR,        // $[]e.[1 2 3]
    KON_UNQUOTE_LIST,         // ${}e.{1 2 3}
    KON_UNQUOTE_TABLE        // $()e.(:a 1 :b 2)
} KonUnquoteType;

typedef struct KonStruct Kon;

typedef struct _KonSymbol {
    tb_string_t Data;
    KonSymbolType Type;
} KonSymbol;

typedef enum {
    KON_SYNTAX_MARKER_APPLY,        // %
    KON_SYNTAX_MARKER_EXEC_MSG,     // .
    KON_SYNTAX_MARKER_PIPE,         // |
    KON_SYNTAX_MARKER_CLAUSE_END    // ;
} KonSyntaxMarkerType;

// eg: % | ;
typedef struct _KonSyntaxMarker {
    KonSyntaxMarkerType Type;
} KonSyntaxMarker;

typedef struct _KonPair {
    Kon* Car;
    Kon* Cdr;
} KonPair;

typedef struct _KonCell {
    Kon* Subj;
    Kon* Vector;
    Kon* Table;
    Kon* List;
} KonCell;

typedef struct _KonQuote {
    Kon* Inner;
    KonQuoteType Type;
} KonQuote;

typedef struct _KonQuasiquote {
    Kon* Inner;
    KonQuasiquoteType Type;
} KonQuasiquote;

typedef struct _KonExpand {
    Kon* Inner;
    KonExpandType Type;
} KonExpand;

typedef struct _Unquote {
    Kon* Inner;
    KonUnquoteType Type;
} KonUnquote;

typedef struct {
    Kon* Parent;
    Kon* Lambda;
    Kon* Bindings;
} KonEnv;

struct KonStruct {
    KonType Tag;
    char Markedp;
    union {
        // basic types
        double Flonum;
        tb_string_t String;
        KonSymbol Symbol;
        KonSyntaxMarker SyntaxMarker;

        // list node
        KonPair Pair;

        tb_vector_ref_t Vector;

        tb_hash_map_ref_t Table;

        KonCell Cell;

        // quote
        KonQuote Quote;

        // quasiquote
        KonQuasiquote Quasiquote;

        // expand
        KonExpand Expand;
        
        // unquote
        KonUnquote Unquote;

        KonEnv Env;

    } Value;
};

// types end
////

////
// predicates

#define kon_is_true(x)    ((x) != KON_FALSE)
#define kon_is_not(x)      ((x) == KON_FALSE)

#define kon_is_null(x)    ((x) == KON_NULL)
#define kon_is_nil(x)    ((x) == KON_NIL)
#define kon_is_pointer(x) (((kon_uint_t)(size_t)(x) & KON_POINTER_MASK) == KON_POINTER_TAG)
#define kon_is_fixnum(x)  (((kon_uint_t)(x) & KON_FIXNUM_MASK) == KON_FIXNUM_TAG)

#define kon_is_immediate_symbol(x) (((kon_uint_t)(x) & KON_IMMEDIATE_MASK) == KON_ISYMBOL_TAG)
#define kon_is_char(x)    (((kon_uint_t)(x) & KON_EXTENDED_MASK) == KON_CHAR_TAG)
#define kon_is_reader_label(x) (((kon_uint_t)(x) & KON_EXTENDED_MASK) == KON_READER_LABEL_TAG)
#define kon_is_boolean(x) (((x) == KON_TRUE) || ((x) == KON_FALSE))

#define kon_pointer_tag(x)      ((x)->Tag)

#define kon_check_tag(x,t)  (kon_is_pointer(x) && (kon_pointer_tag(x) == (t)))

#define kon_slot_ref(x,i)   (((Kon*)&((x)->Value))[i])
#define kon_slot_set(x,i,v) (((Kon*)&((x)->Value))[i] = (v))

#define kon_is_flonum(x)      (kon_check_tag(x, KON_FLONUM))
#define kon_flonum_value(f) ((f)->Value.Flonum)
#define kon_flonum_value_set(f, x) ((f)->Value.Flonum = x)

#define kon_is_bytes(x)      (kon_check_tag(x, KON_BYTES))
#define kon_is_string(x)     (kon_check_tag(x, KON_STRING))
#define kon_is_symbol(x)    (kon_check_tag(x, KON_SYMBOL))
#define kon_is_syntax_marker(x)    (kon_check_tag(x, KON_SYNTAX_MARKER))

#define kon_is_pair(x)       (kon_check_tag(x, KON_PAIR))
#define kon_is_vector(x)     (kon_check_tag(x, KON_VECTOR))
#define kon_is_table(x)     (kon_check_tag(x, KON_TABLE))
#define kon_is_cell(x)     (kon_check_tag(x, KON_CELL))

#define kon_is_quote(x)    (kon_check_tag(x, KON_QUOTE))
#define kon_is_quasiquote(x)    (kon_check_tag(x, KON_QUASIQUOTE))
#define kon_is_expand(x)    (kon_check_tag(x, KON_EXPAND))
#define kon_is_unquote(x)    (kon_check_tag(x, KON_UNQUOTE))


#define kon_is_cpointer(x)   (kon_check_tag(x, KON_CPOINTER))
#define kon_is_exception(x)  (kon_check_tag(x, KON_EXCEPTION))
#define kon_is_env(x)        (kon_check_tag(x, KON_ENV))

// predicates end
////

////
// constructors start

#define KON_NEG_ONE kon_make_fixnum(-1)
#define KON_ZERO    kon_make_fixnum(0)
#define KON_ONE     kon_make_fixnum(1)
#define KON_TWO     kon_make_fixnum(2)
#define KON_THREE   kon_make_fixnum(3)
#define KON_FOUR    kon_make_fixnum(4)
#define KON_FIVE    kon_make_fixnum(5)
#define KON_SIX     kon_make_fixnum(6)
#define KON_SEVEN   kon_make_fixnum(7)
#define KON_EIGHT   kon_make_fixnum(8)
#define KON_NINE    kon_make_fixnum(9)
#define KON_TEN     kon_make_fixnum(10)


#define kon_make_fixnum(n)    ((Kon*) ((((kon_int_t)(n))*(kon_int_t)((kon_int_t)1<<KON_FIXNUM_BITS)) | KON_FIXNUM_TAG))
#define kon_unbox_fixnum(n)   (((kon_int_t)((kon_uint_t)(n) & ~KON_FIXNUM_TAG))/(kon_int_t)((kon_int_t)1<<KON_FIXNUM_BITS))


#define kon_field(x, type, id, field) ((x)->Value.type.field)

#define kon_make_character(n)  ((Kon*) ((((kon_int_t)(n))<<KON_EXTENDED_BITS) + KON_CHAR_TAG))
#define kon_unbox_character(n) ((int) (((kon_int_t)(n))>>KON_EXTENDED_BITS))

// list
#define kon_cons(kstate, a, b) KON_Cons(kstate, NULL, 2, a, b)
#define kon_car(x)         (kon_field(x, Pair, KON_PAIR, Car))
#define kon_cdr(x)         (kon_field(x, Pair, KON_PAIR, Cdr))

#define kon_caar(x)      (kon_car(kon_car(x)))
#define kon_cadr(x)      (kon_car(kon_cdr(x)))
#define kon_cdar(x)      (kon_cdr(kon_car(x)))
#define kon_cddr(x)      (kon_cdr(kon_cdr(x)))
#define kon_caaar(x)     (kon_car(kon_caar(x)))
#define kon_caadr(x)     (kon_car(kon_cadr(x)))
#define kon_cadar(x)     (kon_car(kon_cdar(x)))
#define kon_caddr(x)     (kon_car(kon_cddr(x)))
#define kon_cdaar(x)     (kon_cdr(kon_caar(x)))
#define kon_cdadr(x)     (kon_cdr(kon_cadr(x)))
#define kon_cddar(x)     (kon_cdr(kon_cdar(x)))
#define kon_cdddr(x)     (kon_cdr(kon_cddr(x)))
#define kon_cadddr(x)    (kon_cadr(kon_cddr(x))) /* just these two */
#define kon_cddddr(x)    (kon_cddr(kon_cddr(x)))

#define kon_list1(kstate,a)        kon_cons((kstate), (a), KON_NIL)


// constructors end
////

#define kon_alloc(kstate, size) kon_malloc(size)
// TODO change to self managed gc,malloc,free
#define kon_malloc malloc
#define kon_free free


// data structure apis start

KON_API Kon* KON_Stringify(Kon* kstate, Kon* source);
Kon* KON_ToFormatString(Kon* kstate, Kon* source, bool newLine, int depth, char* padding);

// number
KON_API Kon* KON_FixnumStringify(Kon* kstate, Kon* source);

KON_API Kon* KON_MakeFlonum(Kon* kstate, double f);
KON_API Kon* KON_FlonumStringify(Kon* kstate, Kon* source);

// char
KON_API Kon* KON_CharStringify(Kon* kstate, Kon* source);

// string
KON_API Kon* KON_MakeString(Kon* kstate, const char* str);
KON_API Kon* KON_MakeEmptyString(Kon* kstate);
KON_API const char* KON_StringToCstr(Kon* str);
KON_API Kon* KON_StringStringify(Kon* kstate, Kon* source);

KON_API Kon* KON_VectorStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding);

// symbol
KON_API Kon* KON_SymbolStringify(Kon* kstate, Kon* source);

// list
KON_API Kon* KON_MakeList(Kon* kstate, ...);
KON_API Kon* KON_Cons(Kon* kstate, Kon* self, kon_int_t n, Kon* head, Kon* tail);
KON_API Kon* KON_List2(Kon* kstate, Kon* a, Kon* b);
KON_API Kon* KON_List3(Kon* kstate, Kon* a, Kon* b, Kon* c);
KON_API bool KON_IsList(Kon* source);
Kon* KON_ListStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding);

// table
Kon* KON_TableStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding);

// cell
Kon* KON_CellStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding);

Kon* KON_SyntaxMarkerStringify(Kon* kstate, Kon* source);

// @
Kon* KON_QuoteStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding);
// $
Kon* KON_QuasiquoteStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding);
// eg $[].
Kon* KON_ExpandStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding);
// eg $[]e.
Kon* KON_UnquoteStringify(Kon* kstate, Kon* source, bool newLine, int depth, char* padding);

// data structure apis end

KON_API tb_void_t kon_hash_item_ptr_free(tb_element_ref_t element, tb_pointer_t buff);
KON_API tb_void_t kon_vector_item_ptr_free(tb_element_ref_t element, tb_pointer_t buff);

// common utils start
KON_API const char* KON_HumanFormatTime();
KON_API Kon* KON_AllocTagged(Kon* kstate, size_t size, kon_uint_t tag);


// common utils end


#endif
