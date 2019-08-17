#ifndef KON_KSON_NODE_H
#define KON_KSON_NODE_H

#include <stdio.h>
#include "tbox/tbox.h"
#include "../prefix/config.h"
#include "../string/kx_stringbuffer.h"
#include "../container/kx_hashtable.h"
#include "../container/kx_vector.h"
#include "../container/kx_list.h"

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
//                 010:  immediate symbol
//                0110:  reserved as 8bit tag flag
//                1110:  unique label (NIL,NULL,TRUE,FALSE, etc.)
//            00010110:  char
//            00100110:  type label (optional)
//            00110110:  extended label (optional)


#define KON_FIXNUM_BITS 1
#define KON_POINTER_BITS 2
#define KON_IMDT_SYMBOL_BITS 3
#define KON_UNIQUE_LABEL_BITS 4
#define KON_EXTENDED_BITS 8

#define KON_FIXNUM_MASK ((1<<KON_FIXNUM_BITS)-1)
#define KON_POINTER_MASK ((1<<KON_POINTER_BITS)-1)
#define KON_IMDT_SYMBOL_MASK ((1<<KON_IMDT_SYMBOL_BITS)-1)
#define KON_UNIQUE_LABEL_MASK ((1<<KON_UNIQUE_LABEL_BITS)-1)
#define KON_EXTENDED_MASK ((1<<KON_EXTENDED_BITS)-1)

#define KON_POINTER_TAG 0
#define KON_FIXNUM_TAG 1
#define KON_IMDT_SYMBOL_TAG 2
#define KON_UNIQUE_LABEL_TAG 14

#define KON_CHAR_TAG 22
#define KON_TYPE_LABEL_TAG 38
#define KON_EXTENDED_TAG 54

#define KON_MAKE_UNIQUE_LABEL(n)  ((KN) ((n<<KON_UNIQUE_LABEL_BITS) \
                                          + KON_UNIQUE_LABEL_TAG))

// undefined, no value here a kind of exception
// like javascript undefined
#define KON_UNDEF  KON_MAKE_UNIQUE_LABEL(0) // 14 0x0e

// ref end of a data structure like list, tree, graph ...
// representes a no value struct/class instance pointer
// KON_IS_LIST(KON_NIL) == KON_TRUE
#define KON_NIL   KON_MAKE_UNIQUE_LABEL(1)  // 30 0x1e

// unknown, container empty placeholder
// like objective c [NSNull null]
#define KON_UKN  KON_MAKE_UNIQUE_LABEL(2)  // 46 0x2e
#define KON_TRUE   KON_MAKE_UNIQUE_LABEL(3) // 62 0x3e
#define KON_FALSE  KON_MAKE_UNIQUE_LABEL(4) // 78 0x4e
#define KON_EOF    KON_MAKE_UNIQUE_LABEL(5) // 94 0x5e

// tagging system end
////

////
// types start
typedef enum {
    // determined by tagging system
    KON_T_FIXNUM = 1,     //   1
    KON_T_POINTER,        // 000
    KON_T_IMMDT_SYMBOL,   // 0110
    KON_T_CHAR,               // 00011110
    KON_T_UNIQUE_IMMDT,   // 00111110
    KON_T_BOOLEAN,    // a sub type of unique immediate
    KON_T_UKN,
    KON_T_UNDEF,
    KON_T_NIL,

    // determined by tagging system and ((KonBase*)x)->Tag
    KON_T_NUMBER,
    KON_T_PAIRLIST, // KON_T_NIL AND KON_T_PAIR

    // determined by ((KonBase*)x)->Tag
    KON_T_STATE,
    KON_T_FLONUM,
    KON_T_BIGNUM,
    KON_T_PAIR,
    KON_T_SYMBOL,
    KON_T_SYNTAX_MARKER,
    KON_T_BYTES,
    KON_T_STRING,
    KON_T_VECTOR,
    KON_T_TABLE,
    KON_T_CELL,
    KON_T_QUOTE,
    KON_T_QUASIQUOTE,
    KON_T_EXPAND,
    KON_T_UNQUOTE,
    KON_T_ENV,
    KON_T_ATTR_SLOT,
    KON_T_MSG_DISPATCHER,
    KON_T_CONTINUATION,
    // KON_T_TRAMPOLINE,
    KON_T_PROCEDURE,
    KON_T_CPOINTER,
    KON_T_EXCEPTION,
} KonType;

typedef struct KonState KonState;
typedef struct KonSymbol KonSymbol;
typedef struct KonSyntaxMarker KonSyntaxMarker;
typedef struct KonString KonString;
typedef struct KonTable KonTable;
typedef struct KonVector KonVector;
typedef struct KonPair KonPair;
typedef struct KonCell KonCell;
typedef struct KonQuote KonQuote;
typedef struct KonQuasiquote KonQuasiquote;
typedef struct KonExpand KonExpand;
typedef struct KonUnquote KonUnquote;
typedef struct KonEnv KonEnv;
typedef struct KonAttrSlot KonAttrSlot;
typedef struct KonMsgDispatcher KonMsgDispatcher;
typedef struct KonProcedure KonProcedure;

// not used
#define KON_GC_MARK_WHITE '0'
// allocated between two continuation switch
#define KON_GC_MARK_GRAY '1'
// mark in process
#define KON_GC_MARK_RED '2'
// can be reached from root, liveness: true
#define KON_GC_MARK_BLACK '3'

typedef struct KonBase {
    KonType Tag;
    
    // unboxed fixnum
    unsigned int MsgDispatcherId;
    char GcMarkColor;
} KonBase;

typedef volatile union _Kon* KN;

typedef enum {
    KON_SYM_PREFIX_WORD, // !ass
    KON_SYM_SUFFIX_WORD, // ^ass
    KON_SYM_WORD,  // abc
    KON_SYM_VARIABLE,    // @abc
    KON_SYM_IDENTIFIER, // $abc
    KON_SYM_STRING, // $''

    KON_QUERY_PATH, // /tag /. /.. /~
    // KON_MSG_SIGNAL, // .add 5 2
} KonSymbolType;

struct KonSymbol {
    KonBase Base;
    const char* Data;
    KonSymbolType Type;
};

typedef enum {
    KON_QUOTE_LIST,         // $[1 2 3]
    KON_QUOTE_CELL          // ${ojb (:a 1 :b 2)}
} KonQuoteType;

struct KonQuote {
    KonBase Base;
    KN Inner;
    KonQuoteType Type;
};


typedef enum {
    KON_QUASI_LIST,         // @{1 2 3}
    KON_QUASI_CELL          // @<ojb (:a 1 :b 2)>
} KonQuasiquoteType;

struct KonQuasiquote {
    KonBase Base;
    KN Inner;
    KonQuasiquoteType Type;
};

typedef enum {
    KON_EXPAND_REPLACE,          // $.abc
    KON_EXPAND_SEQ,        // $~.[1 2 3]
    KON_EXPAND_KV        // $%.(:a 1 :b 2)
} KonExpandType;

struct KonExpand {
    KonBase Base;
    KN Inner;
    KonExpandType Type;
};

typedef enum {
    KON_UNQUOTE_REPLACE,          // @.abc
    KON_UNQUOTE_SEQ,        // @~.[(1 2 3)]
    KON_UNQUOTE_KV        // @%.[$(:a $var :b 2)]
} KonUnquoteType;

struct KonUnquote {
    KonBase Base;
    KN Inner;
    KonUnquoteType Type;
};


typedef enum {
    KON_SYNTAX_MARKER_APPLY,        // %
    KON_SYNTAX_MARKER_MSG_SIGNAL,   // .
    KON_SYNTAX_MARKER_PROC_PIPE,         // |
    KON_SYNTAX_MARKER_CLAUSE_END    // ;
} KonSyntaxMarkerType;

// eg: % | ;
struct KonSyntaxMarker {
    KonBase Base;
    KonSyntaxMarkerType Type;
};

struct KonPair {
    KonBase Base;
    KN Prev;
    KN Body;
    KN Next;
};

struct KonCell {
    KonBase Base;
    KN Core;
    KonVector* Vector;
    KonTable* Table;
    KonPair* List;
    KonCell* Next;
    KonCell* Prev;
};

struct KonEnv {
    KonBase Base;
    KonEnv* Parent;
    KxHashTable* Bindings;
    KxHashTable* MsgDispatchers;
};

struct KonAttrSlot {
    KonBase Base;
    bool IsDir;
    bool OpenToChildren;
    bool OpenToSibling;
    bool CanWrite;
    bool IsProc;    // func, blk, lambda
    bool IsMethod;  // func(self, p2, p3)
    KN Value;
    KxHashTable* Folder;
};

struct KonMsgDispatcher {
    KonBase Base;
    KonProcedure* OnApplyArgs;  // % p1 p2;
    KonProcedure* OnSelectPath;  // /abc /efg
    KonProcedure* OnMethodCall; // . push 1 2;
    KonProcedure* OnVisitVector;  // <>
    KonProcedure* OnVisitTable; // ()
    KonProcedure* OnVisitCell;  // {}
};

typedef enum {
    // KON_PRIMARY_FUNC,   // high order native func
    KON_NATIVE_FUNC,
    KON_NATIVE_OBJ_METHOD,
    // dynamic scope.
    // make by !func
    KON_COMPOSITE_FUNC,
    // lexial(static) scope
    // make by !lambda
    KON_COMPOSITE_LAMBDA,
    // a block of code, can be assigned to a var, and call
    // vars lookup in eval env. like a func without params
    // make by !blk
    KON_COMPOSITE_BLK,
    KON_COMPOSITE_OBJ_METHOD,
} KonProcedureType;

typedef KN (*KonNativeFuncRef)(KonState* kstate, KN argList);
typedef KN (*KonNativeObjMethodRef)(KonState* kstate, void* objRef, KN argList);

struct KonProcedure {
    KonBase Base;
    KonProcedureType Type;
    int ParamNum;   // exclude ... arg
    bool HasVariableParam;   // if have ... in arg list
    union {
        KonNativeFuncRef NativeFuncRef;

        KonNativeObjMethodRef NativeObjMethod;

        struct {
            KonPair* ArgList;
            KonPair* Body;
            KonEnv* LexicalEnv;
        } Composite;
    };
};



typedef struct _KonContinuation KonContinuation;

typedef enum {
    // should be the first continuation created
    KON_CONT_RETURN,
    // sentences like {{1 + 2} {2 + 3}}
    KON_CONT_EVAL_SENTENCE_LIST,
    // sentence like {"Abc" + "Efg" | to-upcase; | Length; | + 2}
    KON_CONT_EVAL_SENTENCE,
    // subject is the first item of a sentence, like the 'abc' in {abc + 2}
    KON_CONT_EVAL_SUBJ,
    KON_CONT_EVAL_CLAUSE_LIST,
    KON_CONT_EVAL_CLAUSE_ARGS,

    // native callback, use a MemoTable to store info
    KON_CONT_NATIVE_CALLBACK

} KonContinuationType;

typedef KN (*KonContFuncRef)(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked);

struct _KonContinuation {
    KonBase Base;
    KonContinuationType Type;
    KN Env;
    // a Return continuation's Cont is empty
    KonContinuation* Cont;
    union {
        // most continuations use this store rest jobs to do
        KN RestJobs;
        
        // for continuations need sentence subject
        struct {
            KN Subj;
            KN RestArgList;
            KN EvaledArgList;
        } EvalClauseArgs;

        struct {
            KN Subj;
            KN RestClauses;
        } EvalClauseList;

        struct {
            KonContFuncRef Callback;
            // used for native marcros, save more custom info
            KxHashTable* MemoTable;
        } Native;
    };
};


struct KonState {
    KonBase Base;
    KonEnv* RootEnv;

    ////
    // gc root source start

    KonContinuation* CurrCont;
    // temp pointer type KN list allocated between two
    // continuation switch
    KxList* WriteBarrierGen;
    KN CurrCode;

    // gc root source end
    ////


    // a list of KxVector. store heap pointers
    KxList* HeapPtrSegs;
    KxVector* SegmentMaxSizeVec;    // buffsize of each ptr segment
    unsigned long MaxObjCntLimit;    // how many objs can be stored in ptr seg

    // mark task queue. mark the value grey before add to this queue
    KxList* MarkTaskQueue;
    
    tb_allocator_ref_t LargeAllocator;
    tb_allocator_ref_t Allocator;   // default allocator

    // how dispatch a message
    KxVector* MsgDispatchers;
    unsigned int NextMsgDispatcherId;
};

typedef struct KonFlonum {
    KonBase Base;
    double Flonum;
} KonFlonum;

// TODO replace to kon string impl
struct KonString {
    KonBase Base;
    KxStringBuffer* String;
};

struct KonVector {
    KonBase Base;
    KxVector* Vector;
};

struct KonTable {
    KonBase Base;
    KxHashTable* Table;
};

union _Kon {
    KonBase KonBase;
    KonState KonState;
    KonSymbol KonSymbol;
    KonSyntaxMarker KonSyntaxMarker;
    KonString KonString;
    KonTable KonTable;
    KonVector KonVector;
    KonPair KonPair;
    KonCell KonCell;
    KonQuote KonQuote;
    KonQuasiquote KonQuasiquote;
    KonExpand KonExpand;
    KonUnquote KonUnquote;
    KonEnv KonEnv;
    KonAttrSlot KonAttrSlot;
    KonProcedure KonProcedure;
    KonContinuation KonContinuation;
};

// types end
////

KON_API KN KON_AllocTagged(KonState* kstate, size_t size, kon_uint_t tag);
KON_API unsigned int KON_NodeDispacherId(KonState* kstate, KN obj);

#define KON_ALLOC_TYPE_TAG(kstate,t,tag)  ((t *)KON_AllocTagged(kstate, sizeof(t), tag))
#define KON_FREE(kstate, ptr) KON_GC_FREE(ptr)


// inline KN KON_AllocTagged(KonState* kstate, size_t size, kon_uint_t tag)
// {
//   KN res = (KN) KON_GC_MALLOC(size);
//   if (res && ! KON_IS_EXCEPTION(res)) {
//     ((KonBase*)res)->Tag = tag;
//   }
//   return res;
// }


////
// predicates




// #define KonDef(t)          struct Kon##t *
#define CAST_Kon(t, v)          ((struct Kon##t *)v)
#define KON_PTR_TYPE(x)     (((KonBase*)(x))->Tag)



#define KON_IS_TRUE(x)    ((x) != KON_FALSE)
#define KON_IS_FALSE(x)      ((x) == KON_FALSE)
#define KON_IS_UKN(x)    ((x) == KON_UKN)
#define KON_IS_UNDEF(x)    ((x) == KON_UNDEF)
#define KON_IS_NIL(x)    ((x) == KON_NIL || (KON_CHECK_TAG(x, KON_T_QUOTE) && KON_QUOTE_TYPE(x) == KON_QUOTE_LIST && ((KonQuote*)x)->Inner == KON_NIL))
#define KON_IS_POINTER(x) (((kon_uint_t)(size_t)(x) & KON_POINTER_MASK) == KON_POINTER_TAG)
#define KON_IS_FIXNUM(x)  (((kon_uint_t)(x) & KON_FIXNUM_MASK) == KON_FIXNUM_TAG)

#define KON_IS_IMDT_SYMBOL(x) (((kon_uint_t)(x) & KON_IMDT_SYMBOL_BITS) == KON_IMDT_SYMBOL_TAG)
#define KON_IS_CHAR(x)    (((kon_uint_t)(x) & KON_EXTENDED_MASK) == KON_CHAR_TAG)
#define KON_IS_BOOLEAN(x) (((x) == KON_TRUE) || ((x) == KON_FALSE))

#define KON_GET_PTR_TAG(x)      (((KonBase*)x)->Tag)

#define KON_CHECK_TAG(x,t)  (KON_IS_POINTER(x) && (KON_PTR_TYPE(x) == (t)))

// #define kon_slot_ref(x,i)   (((KN)&((x)->Value))[i])
// #define kon_slot_set(x,i,v) (((KN)&((x)->Value))[i] = (v))

#define KON_IS_FLONUM(x)      (KON_CHECK_TAG(x, KON_T_FLONUM))
#define KON_FLONUM_VALUE(f) (((KonFlonum*)f)->Flonum)
#define KON_FLONUM_VALUE_SET(f, x) (((KonFlonum*)f)->Flonum = x)

#define KON_IS_BYTES(x)         (KON_CHECK_TAG(x, KON_T_BYTES))
#define KON_IS_STRING(x)        (KON_CHECK_TAG(x, KON_T_STRING))
#define KON_IS_SYMBOL(x)        (KON_CHECK_TAG(x, KON_T_SYMBOL))
#define KON_IS_VARIABLE(x)      (KON_CHECK_TAG(x, KON_T_SYMBOL) && ((KonSymbol*)x)->Type == KON_SYM_VARIABLE)
#define KON_IS_IDENTIFIER(x)      (KON_CHECK_TAG(x, KON_T_SYMBOL) && ((KonSymbol*)x)->Type == KON_SYM_IDENTIFIER)
#define KON_IS_WORD(x)      (KON_CHECK_TAG(x, KON_T_SYMBOL) && ((KonSymbol*)x)->Type == KON_SYM_WORD)
#define KON_IS_PREFIX_MARCRO(x) (KON_CHECK_TAG(x, KON_T_SYMBOL) && ((KonSymbol*)x)->Type == KON_SYM_PREFIX_WORD)
#define KON_IS_SYNTAX_MARKER(x) (KON_CHECK_TAG(x, KON_T_SYNTAX_MARKER))
#define KON_IS_QUERY_PATH(x) (KON_CHECK_TAG(x, KON_T_SYMBOL) && ((KonSymbol*)x)->Type == KON_QUERY_PATH)

#define KON_IS_ATTR_SLOT(x) (KON_CHECK_TAG(x, KON_T_ATTR_SLOT))

#define KON_IS_PAIR(x)       (KON_CHECK_TAG(x, KON_T_PAIR))
#define KON_IS_VECTOR(x)     (KON_CHECK_TAG(x, KON_T_VECTOR))
#define KON_IS_TABLE(x)     (KON_CHECK_TAG(x, KON_T_TABLE))
#define KON_IS_CELL(x)     (KON_CHECK_TAG(x, KON_T_CELL))

#define KON_IS_QUOTE(x)    (KON_CHECK_TAG(x, KON_T_QUOTE))
#define KON_IS_QUOTE_LIST(x)    (KON_CHECK_TAG(x, KON_T_QUOTE) && KON_QUOTE_TYPE(x) == KON_QUOTE_LIST)
#define KON_IS_QUOTE_NIL(x)    (KON_CHECK_TAG(x, KON_T_QUOTE) && KON_QUOTE_TYPE(x) == KON_QUOTE_LIST && ((KonQuote*)x)->Inner == KON_NIL)
#define KON_IS_QUASIQUOTE(x)    (KON_CHECK_TAG(x, KON_T_QUASIQUOTE))
#define KON_IS_EXPAND(x)    (KON_CHECK_TAG(x, KON_T_EXPAND))
#define KON_IS_UNQUOTE(x)    (KON_CHECK_TAG(x, KON_T_UNQUOTE))

#define KON_IS_ENV(x)        (KON_CHECK_TAG(x, KON_T_ENV))
#define KON_IS_PROCEDURE(x)        (KON_CHECK_TAG(x, KON_T_PROCEDURE))
#define KON_IS_CONTINUATION(x)        (KON_CHECK_TAG(x, KON_T_CONTINUATION))
#define KON_IS_CPOINTER(x)   (KON_CHECK_TAG(x, KON_T_CPOINTER))
#define KON_IS_EXCEPTION(x)  (KON_CHECK_TAG(x, KON_T_EXCEPTION))

// predicates end
////

////
// data structure util start

#define KON_NEG_ONE KON_MAKE_FIXNUM(-1)
#define KON_ZERO    KON_MAKE_FIXNUM(0)
#define KON_ONE     KON_MAKE_FIXNUM(1)
#define KON_TWO     KON_MAKE_FIXNUM(2)
#define KON_THREE   KON_MAKE_FIXNUM(3)
#define KON_FOUR    KON_MAKE_FIXNUM(4)
#define KON_FIVE    KON_MAKE_FIXNUM(5)
#define KON_SIX     KON_MAKE_FIXNUM(6)
#define KON_SEVEN   KON_MAKE_FIXNUM(7)
#define KON_EIGHT   KON_MAKE_FIXNUM(8)
#define KON_NINE    KON_MAKE_FIXNUM(9)
#define KON_TEN     KON_MAKE_FIXNUM(10)


#define KON_MAKE_FIXNUM(n)    ((KN) ((((kon_int_t)(n))*(kon_int_t)((kon_int_t)1<<KON_FIXNUM_BITS)) | KON_FIXNUM_TAG))
#define KON_UNBOX_FIXNUM(n)   (((kon_int_t)((kon_uint_t)(n) & ~KON_FIXNUM_TAG))/(kon_int_t)((kon_int_t)1<<KON_FIXNUM_BITS))

static inline KN KON_MAKE_FLONUM(KonState* kstate, double num) {
  KonFlonum* result = (KonFlonum*)KON_AllocTagged(kstate, sizeof(KonFlonum), KON_T_FLONUM);
  result->Flonum = num;
  return (result);
}
#define KON_UNBOX_FLONUM(n) ((KonFlonum*)n)->Flonum

#define KON_FIELD(x, type, field) (((type *)x)->field)

#define kon_make_character(n)  ((KN) ((((kon_int_t)(n))<<KON_EXTENDED_BITS) + KON_CHAR_TAG))
#define KON_UNBOX_CHAR(n) ((int) (((kon_int_t)(n))>>KON_EXTENDED_BITS))

#define KON_UNBOX_STRING(str) (((KonString*)str)->String)

#define KON_UNBOX_VECTOR(str) (((KonVector*)str)->Vector)

#define KON_UNBOX_TABLE(str) (((KonVector*)str)->Vector)

#define KON_UNBOX_SYMBOL(x) (((KonSymbol*)x)->Data)

#define KON_UNBOX_QUOTE(x) (((KonQuote*)x)->Inner)
#define KON_QUOTE_TYPE(x) (((KonQuote*)x)->Type)

// list
#define KON_CONS(kstate, a, b) KON_Cons(kstate, NULL, 2, a, b)
#define KON_CAR(x)         (KON_FIELD(x, KonPair, Body))
#define KON_CDR(x)         (KON_FIELD(x, KonPair, Next))

#define KON_CAAR(x)      (KON_CAR(KON_CAR(x)))
#define KON_CADR(x)      (KON_CAR(KON_CDR(x)))
#define KON_CDAR(x)      (KON_CDR(KON_CAR(x)))
#define KON_CDDR(x)      (KON_CDR(KON_CDR(x)))
#define KON_CAAAR(x)     (KON_CAR(KON_CAAR(x)))
#define KON_CAADR(x)     (KON_CAR(KON_CADR(x)))
#define KON_CADAR(x)     (KON_CAR(KON_CDAR(x)))
#define KON_CADDR(x)     (KON_CAR(KON_CDDR(x)))
#define KON_CDAAR(x)     (KON_CDR(KON_CAAR(x)))
#define KON_CDADR(x)     (KON_CDR(KON_CADR(x)))
#define KON_CDDAR(x)     (KON_CDR(KON_CDAR(x)))
#define KON_CDDDR(x)     (KON_CDR(KON_CDDR(x)))
#define KON_CADDDR(x)    (KON_CADR(KON_CDDR(x))) /* just these two */
#define KON_CDDDDR(x)    (KON_CDDR(KON_CDDR(x)))

#define KON_LIST1(kstate,a)        KON_CONS((kstate), (a), KON_NIL)


// data structure util end
////



// data structure apis start

KON_API KN KON_Stringify(KonState* kstate, KN source);
KN KON_ToFormatString(KonState* kstate, KN source, bool newLine, int depth, char* padding);

// number
KON_API KN KON_FixnumStringify(KonState* kstate, KN source);

KON_API KN KON_MakeFlonum(KonState* kstate, double f);
KON_API KN KON_FlonumStringify(KonState* kstate, KN source);

// char
KON_API KN KON_CharStringify(KonState* kstate, KN source);

// string
KON_API KN KON_MakeString(KonState* kstate, const char* str);
KON_API KN KON_MakeEmptyString(KonState* kstate);
KON_API const char* KON_StringToCstr(KN str);
KON_API KN KON_StringStringify(KonState* kstate, KN source);

KON_API KN KON_VectorStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);

// symbol
KON_API KN KON_SymbolStringify(KonState* kstate, KN source);
KON_API const char* KON_SymbolToCstr(KN sym);

// list
// KON_API KN KON_MakePairList(KonState* kstate, ...);
KON_API KN KON_Cons(KonState* kstate, KN self, kon_int_t n, KN head, KN tail);
KON_API KN KON_PairList2(KonState* kstate, KN a, KN b);
KON_API KN KON_PairList3(KonState* kstate, KN a, KN b, KN c);
KON_API bool KON_IsPairList(KN source);
KN KON_PairListStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
KN KON_PairListRevert(KonState* kstate, KN source);
KN KON_PairListLength(KonState* kstate, KN source);

// table
KN KON_TableStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);

// cell
KN KON_CellStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);

KN KON_SyntaxMarkerStringify(KonState* kstate, KN source);

// @
KN KON_QuoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
// $
KN KON_QuasiquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
// eg $[].
KN KON_ExpandStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
// eg $[]e.
KN KON_UnquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);


KN MakeNativeProcedure(KonState* kstate, KonProcedureType type, KonNativeFuncRef funcRef);
KonProcedure* MakeDispatchProc(KonState* kstate, KN procAst, KN env);
KonMsgDispatcher* MakeMsgDispatcher(KonState* kstate);
int KON_SetMsgDispatcher(KonState* kstate, unsigned int dispatcherId, KonMsgDispatcher* dispatcher);
unsigned int KON_SetNextMsgDispatcher(KonState* kstate, KonMsgDispatcher* dispatcher);
KonMsgDispatcher* KON_GetMsgDispatcher(KonState* kstate, unsigned int dispatcherId);

KN MakeAttrSlotLeaf(KonState* kstate, KN value, char* mod);
KN MakeAttrSlotFolder(KonState* kstate, char* mod);

// data structure apis end

// common utils start
KON_API KxStringBuffer* KON_ReadFileContent(const char* filePathOrigin);
KON_API const char* KON_HumanFormatTime();

KN KON_VectorToKonPairList(KonState* kstate, KxVector* vector);

// common utils end


#endif
