#ifndef KN_KSON_NODE_H
#define KN_KSON_NODE_H

#include <stdio.h>
#include "../lib/tbox/tbox.h"
#include "./prefix/config.h"
#include "./string/kx_stringbuffer.h"
#include "./container/kx_hashtable.h"
#include "./container/kx_vector.h"
#include "./container/kx_list.h"

////
// type alias start

#if KN_64_BIT
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
//                  00:  KN pointer
//                 010:  c extension pointer
//                0110:  reserved as 8bit tag flag
//                1110:  unique label (NIL,NULL,TRUE,FALSE, etc.)
//            00010110:  char
//            00100110:  type label (optional)
//            00110110:  extended label (optional)


#define KN_FIXNUM_BITS 1
#define KN_POINTER_BITS 2
#define KN_EXT_POINTER_BITS 3
#define KN_UNIQUE_LABEL_BITS 4
#define KN_EXTENDED_BITS 8

#define KN_FIXNUM_MASK ((1<<KN_FIXNUM_BITS)-1)
#define KN_POINTER_MASK ((1<<KN_POINTER_BITS)-1)
#define KN_EXT_POINTER_MASK ((1<<KN_EXT_POINTER_BITS)-1)
#define KN_UNIQUE_LABEL_MASK ((1<<KN_UNIQUE_LABEL_BITS)-1)
#define KN_EXTENDED_MASK ((1<<KN_EXTENDED_BITS)-1)

#define KN_POINTER_TAG 0
#define KN_FIXNUM_TAG 1
#define KN_EXT_POINTER_TAG 2
#define KN_UNIQUE_LABEL_TAG 14

#define KN_CHAR_TAG 22
#define KN_TYPE_LABEL_TAG 38
#define KN_EXTENDED_TAG 54

#define KN_MAKE_UNIQUE_LABEL(n)  ((KN) ((n<<KN_UNIQUE_LABEL_BITS) \
                                          + KN_UNIQUE_LABEL_TAG))

// undefined, no value here a kind of exception
// like javascript undefined
#define KN_UNDEF  KN_MAKE_UNIQUE_LABEL(0) // 14 0x0e

// ref end of a data structure like list, tree, graph ...
// representes a no value struct/class instance pointer
// KN_IS_LIST(KN_NIL) == KN_TRUE
#define KN_NIL   KN_MAKE_UNIQUE_LABEL(1)  // 30 0x1e

// unknown, container empty placeholder
// like objective c [NSNull null]
#define KN_UKN  KN_MAKE_UNIQUE_LABEL(2)  // 46 0x2e
#define KN_TRUE   KN_MAKE_UNIQUE_LABEL(3) // 62 0x3e
#define KN_FALSE  KN_MAKE_UNIQUE_LABEL(4) // 78 0x4e
#define KN_EOF    KN_MAKE_UNIQUE_LABEL(5) // 94 0x5e

// tagging system end
////

////
// types start
typedef enum {
    // determined by tagging system
    KN_T_FIXNUM = 1,     //   1
    KN_T_POINTER,        // 000
    KN_T_IMMDT_SYMBOL,   // 0110
    KN_T_CHAR,               // 00011110
    KN_T_UNIQUE_IMMDT,   // 00111110
    KN_T_BOOLEAN,    // a sub type of unique immediate
    KN_T_UKN,
    KN_T_UNDEF,
    KN_T_NIL,

    // determined by tagging system and ((KonBase*)x)->tag
    KN_T_NUMBER,
    KN_T_PAIRLIST, // KN_T_NIL AND KN_T_PAIR

    // determined by ((KonBase*)x)->tag
    KN_T_STATE,
    KN_T_FLONUM,
    KN_T_BIGNUM,
    KN_T_PAIR,
    KN_T_SYMBOL,
    KN_T_SYNTAX_MARKER,
    KN_T_BYTES,
    KN_T_STRING,
    KN_T_VECTOR,
    KN_T_TABLE,
    KN_T_MAP,
    KN_T_CELL,
    KN_T_PARAM,
    KN_T_BLOCK,
    KN_T_QUOTE,
    KN_T_QUASIQUOTE,
    KN_T_EXPAND,
    KN_T_UNQUOTE,
    KN_T_ENV,
    KN_T_ACCESSOR,
    KN_T_MSG_DISPATCHER,
    KN_T_CONTINUATION,
    // KN_T_TRAMPOLINE,
    KN_T_PROCEDURE,
    KN_T_CPOINTER,
    KN_T_EXCEPTION,
} KonType;

typedef struct KonBase KonBase;
typedef struct KonFlonum KonFlonum;
typedef struct KonState KonState;
typedef struct KonSymbol KonSymbol;
typedef struct KonSyntaxMarker KonSyntaxMarker;
typedef struct KonString KonString;
typedef struct KonTable KonTable;
typedef struct KonVector KonVector;
typedef struct KonPair KonPair;
typedef struct KonMap KonMap;
typedef struct KonCell KonCell;
typedef struct KonParam KonParam;
typedef struct KonBlock KonBlock;
typedef struct KonQuote KonQuote;
typedef struct KonQuasiquote KonQuasiquote;
typedef struct KonExpand KonExpand;
typedef struct KonUnquote KonUnquote;
typedef struct KonEnv KonEnv;
typedef struct KonAccessor KonAccessor;
typedef struct KonMsgDispatcher KonMsgDispatcher;
typedef struct KonProcedure KonProcedure;
typedef struct KonContinuation KonContinuation;

union _KNValue {
    // kon_int_t asInt;
    // kon_int32_t asInt32;
    // long asLong;
    // double asDouble;
    // void* asPointer;
    KonBase* konBase;
    KonState* konState;
    KonSymbol* konSymbol;
    KonSyntaxMarker* konSyntaxMarker;
    KonString* konString;
    KonTable* konTable;
    KonVector* konVector;
    KonPair* konPair;
    KonMap* konMap;
    KonCell* konCell;
    KonQuote* konQuote;
    KonQuasiquote* konQuasiquote;
    KonExpand* konExpand;
    KonMsgDispatcher* konMsgDispatcher;
    KonEnv* konEnv;
    KonAccessor* konAccessor;
    KonProcedure* konProcedure;
    KonContinuation* konContinuation;
};

// not used
#define KN_GC_MARK_WHITE '0'
// allocated between two continuation switch
#define KN_GC_MARK_GRAY '1'
// mark in process
#define KN_GC_MARK_RED '2'
// can be reached from root, liveness: true
#define KN_GC_MARK_BLACK '3'

struct KonBase {
    KonType tag;
    
    // unboxed fixnum
    unsigned int msgDispatcherId;
    char gcMarkColor;
};

typedef volatile union _Kon* KN;
// typedef union _Kon KN;
// typedef volatile union _KNValue KN;

typedef enum {
    KN_SYM_PREFIX_WORD, // !ass
    KN_SYM_SUFFIX_WORD, // ^ass
    KN_SYM_WORD,  // abc
    KN_SYM_VARIABLE,    // @abc
    KN_SYM_IDENTIFIER, // $abc
    KN_SYM_STRING, // $''
} KonSymbolType;

struct KonSymbol {
    KonBase base;
    const char* data;
    KonSymbolType type;
};

typedef enum {
    KN_QUOTE_LIST,         // $[1 2 3]
    KN_QUOTE_CELL          // ${ojb (:a 1 :b 2)}
} KonQuoteType;

struct KonQuote {
    KonBase base;
    KN inner;
    KonQuoteType type;
};


typedef enum {
    KN_QUASI_LIST,         // @{1 2 3}
    KN_QUASI_CELL          // @<ojb (:a 1 :b 2)>
} KonQuasiquoteType;

struct KonQuasiquote {
    KonBase base;
    KN inner;
    KonQuasiquoteType type;
};

typedef enum {
    KN_EXPAND_REPLACE,          // @.abc
    KN_EXPAND_SEQ,        // @~.[1 2 3]
    KN_EXPAND_KV        // @%.(:a 1 :b 2)
} KonExpandType;

struct KonExpand {
    KonBase base;
    KN inner;
    KonExpandType type;
};

typedef enum {
    KN_UNQUOTE_REPLACE,          // $.abc
    KN_UNQUOTE_SEQ,        // $~.[(1 2 3)]
    KN_UNQUOTE_KV        // $%.[#(:a $var :b 2)]
} KonUnquoteType;

struct KonUnquote {
    KonBase base;
    KN inner;
    KonUnquoteType type;
};


typedef enum {
    KN_SYNTAX_MARKER_APPLY,        // %
    KN_SYNTAX_MARKER_EQUAL,        // =
    KN_SYNTAX_MARKER_ASSIGN,        // :=
    KN_SYNTAX_MARKER_MSG_SIGNAL,   // .
    KN_SYNTAX_MARKER_GET_SLOT,        // / /. /.. /ABC /~
    KN_SYNTAX_MARKER_PROC_PIPE,         // |
    KN_SYNTAX_MARKER_CLAUSE_END    // ;
} KonSyntaxMarkerType;

// eg: % | ;
struct KonSyntaxMarker {
    KonBase base;
    KonSyntaxMarkerType type;
};

struct KonPair {
    KonBase base;
    KN prev;
    KN body;
    KN next;
};

struct KonBlock {
    KonBase base;
    KN prev;
    KN body;
    KN next;
};

struct KonMap {
    KonBase base;
    KxHashTable* map;
};

struct KonCell {
    KonBase base;
    KN core;
    KonMap* map;
    KonTable* table;
    KonPair* list;
    KonCell* next;
    KonCell* prev;
};

struct KonEnv {
    KonBase base;
    KonEnv* parent;
    KxHashTable* bindings;
};

// an accessor is a value wrapper
// like file folders and files
// if IsDir is true, data is stored in Dir, 
// if IsDir is false, means it is a leaf node, is a property
struct KonAccessor {
    KonBase base;
    bool isDir; // d
    bool openToRef;    // r. only used in dir mod
    bool openToChildren;
    bool openToSibling;
    bool canWrite;  // w
    // bool canExec;    // x func, blk, lambda
    // bool isMethod;  // m func(self, p2, p3)
    KxHashTable* dir;   // key: cstr, value: KonAccessor*

    KN value;
    KonProcedure* setter;
    // KonProcedure* getter;
};

struct KonMsgDispatcher {
    KonBase base;
    KonProcedure* onSymbol;   // {obj .key1 = 5}
    KonProcedure* onSyntaxMarker;  // % p1 p2; /abc /efg
    KonProcedure* onMethodCall; // {5 + 1 2}
    KonProcedure* onVisitList;  // {#<1 2 3> [1]}
    KonProcedure* onVisitVector;  // #<>
    KonProcedure* onVisitTable; // #()
    KonProcedure* onVisitCell;  // {}
    KonProcedure* onOtherType;  // {5 5} number, string etc
};

typedef enum {
    KN_NATIVE_FUNC,
    KN_NATIVE_OBJ_METHOD,
    // dynamic scope.
    KN_COMPOSITE_FUNC,
    // lexial(static) scope
    KN_COMPOSITE_LAMBDA,

    KN_COMPOSITE_OBJ_METHOD,

    // dynamic scope.
    KN_COMPOSITE_MACRO_FUNC,
    // lexial(static) scope
    KN_COMPOSITE_MACRO_LAMBDA,
} KonProcedureType;

typedef KN (*KonNativeFuncRef)(KonState* kstate, ...);

struct KonProcedure {
    KonBase base;
    KonProcedureType type;
    int paramNum;   // arg num before ...
    int hasVAList;   // if have ... in arg list
    int hasVAMap;   // variable argument map
    union {
        KonNativeFuncRef nativeFuncRef;

        struct {
            KN captureList;
            KN argList;
            KN body;
            KonEnv* lexicalEnv;
        } composite;
    };
};

typedef enum {
    // should be the first continuation created
    KN_CONT_RETURN,
    // sentences like {{1 + 2} {2 + 3}}
    KN_CONT_EVAL_SENTENCE_LIST,
    // sentence like {"Abc" + "Efg" | to-upcase; | Length; | + 2}
    KN_CONT_EVAL_SENTENCE,
    // subject is the first item of a sentence, like the 'abc' in {abc + 2}
    KN_CONT_EVAL_SUBJ,
    KN_CONT_EVAL_CLAUSE_LIST,
    KN_CONT_EVAL_CLAUSE_ARGS,

    KN_CONT_EVAL_QUASI_LIST_ITEMS,

    // native callback, use a MemoTable to store info
    KN_CONT_NATIVE_CALLBACK

} KonContinuationType;

typedef KN (*KonContFuncRef)(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked);

struct KonContinuation {
    KonBase base;
    KonContinuationType type;
    KonEnv* env;
    // a Return continuation's Cont is empty
    KonContinuation* cont;
    union {
        // most continuations use this store rest jobs to do
        KN restJobs;

        struct {
            KN restList;
            KN evaledList;
        } evalListItems;
        
        // for continuations need sentence subject
        struct {
            KN subj;
            KN restArgList;
            KN evaledArgList;
        } evalClauseArgs;

        struct {
            KN subj;
            KN restClauses;
        } evalClauseList;

        struct {
            KonContFuncRef callback;
            // used for native marcros, save more custom info
            KxHashTable* memoTable;
        } native;
    };
};


struct KonState {
    KonBase base;
    KonEnv* rootEnv;

    ////
    // gc root source start

    KonContinuation* currCont;
    // temp pointer type KN list allocated between two
    // continuation switch
    KxList* writeBarrierGen;
    KN currCode;

    // gc root source end
    ////


    // a list of KxVector. store heap pointers
    KxList* heapPtrSegs;
    KxVector* segmentMaxSizeVec;    // buffsize of each ptr segment
    unsigned long maxObjCntLimit;    // how many objs can be stored in ptr seg
    unsigned long gcThreshold;  // trigger gc when obj count bigger than this number
    bool needGc;

    // mark task queue. mark the value grey before add to this queue
    KxList* markTaskQueue;
    
    tb_allocator_ref_t largeAllocator;
    tb_allocator_ref_t allocator;   // default allocator

    // how dispatch a message
    KxVector* msgDispatchers;
    unsigned int nextMsgDispatcherId;
};

struct KonFlonum {
    KonBase base;
    double flonum;
};

// TODO replace to kon string impl
struct KonString {
    KonBase base;
    KxStringBuffer* string;
};

struct KonVector {
    KonBase base;
    KxVector* vector;
};

struct KonTable {
    KonBase base;
    KxHashTable* table;
};

struct KonParam {
    KonBase base;
    KxHashTable* table;
};

union _Kon {
    KonBase konBase;
    KonState konState;
    KonSymbol konSymbol;
    KonSyntaxMarker konSyntaxMarker;
    KonString konString;
    KonTable konTable;
    KonVector konVector;
    KonPair konPair;
    KonMap konMap;
    KonCell konCell;
    KonQuote konQuote;
    KonQuasiquote konQuasiquote;
    KonExpand konExpand;
    KonMsgDispatcher konMsgDispatcher;
    KonEnv konEnv;
    KonAccessor konAccessor;
    KonProcedure konProcedure;
    KonContinuation konContinuation;
};



// types end
////

KN_API KN KN_AllocTagged(KonState* kstate, size_t size, kon_uint_t tag);
KN_API unsigned int KN_NodeDispacherId(KonState* kstate, KN obj);

#define KN_ALLOC_TYPE_TAG(kstate,t,tag)  ((t *)KN_AllocTagged(kstate, sizeof(t), tag))
#define KN_FREE(kstate, ptr) KN_GC_FREE(ptr)


// inline KN KN_AllocTagged(KonState* kstate, size_t size, kon_uint_t tag)
// {
//   KN res = (KN) KN_GC_MALLOC(size);
//   if (res && ! KN_IS_EXCEPTION(res)) {
//     ((KonBase*)res)->tag = tag;
//   }
//   return res;
// }


////
// predicates




// #define KonDef(t)          struct Kon##t *
#define CAST_Kon(t, v)          ((struct Kon##t *)v)
#define KN_PTR_TYPE(x)     (((KonBase*)(x))->tag)



#define KN_IS_TRUE(x)    ((x) != KN_FALSE)
#define KN_IS_FALSE(x)      ((x) == KN_FALSE)
#define KN_IS_UKN(x)    ((x) == KN_UKN)
#define KN_IS_UNDEF(x)    ((x) == KN_UNDEF)
#define KN_IS_NIL(x)    ((x) == KN_NIL || (KN_CHECK_TAG(x, KN_T_QUOTE) && KN_QUOTE_TYPE(x) == KN_QUOTE_LIST && ((KonQuote*)x)->inner == KN_NIL))
#define KN_IS_POINTER(x) (((kon_uint_t)(size_t)(x) & KN_POINTER_MASK) == KN_POINTER_TAG)
#define KN_IS_FIXNUM(x)  (((kon_uint_t)(x) & KN_FIXNUM_MASK) == KN_FIXNUM_TAG)

#define KN_IS_EXT_POINTER(x) (((kon_uint_t)(x) & KN_EXT_POINTER_BITS) == KN_EXT_POINTER_TAG)
#define KN_IS_CHAR(x)    (((kon_uint_t)(x) & KN_EXTENDED_MASK) == KN_CHAR_TAG)
#define KN_IS_BOOLEAN(x) (((x) == KN_TRUE) || ((x) == KN_FALSE))

#define KN_GET_PTR_TAG(x)      (((KonBase*)x)->tag)

#define KN_CHECK_TAG(x,t)  (KN_IS_POINTER(x) && (KN_PTR_TYPE(x) == (t)))

// #define kon_slot_ref(x,i)   (((KN)&((x)->value))[i])
// #define kon_slot_set(x,i,v) (((KN)&((x)->value))[i] = (v))

#define KN_IS_FLONUM(x)      (KN_CHECK_TAG(x, KN_T_FLONUM))
#define KN_FLONUM_VALUE(f) (((KonFlonum*)f)->flonum)
#define KN_FLONUM_VALUE_SET(f, x) (((KonFlonum*)f)->flonum = x)

#define KN_IS_BYTES(x)         (KN_CHECK_TAG(x, KN_T_BYTES))
#define KN_IS_STRING(x)        (KN_CHECK_TAG(x, KN_T_STRING))
#define KN_IS_SYMBOL(x)        (KN_CHECK_TAG(x, KN_T_SYMBOL))
#define KN_IS_VARIABLE(x)      (KN_CHECK_TAG(x, KN_T_SYMBOL) && ((KonSymbol*)x)->type == KN_SYM_VARIABLE)
#define KN_IS_IDENTIFIER(x)      (KN_CHECK_TAG(x, KN_T_SYMBOL) && ((KonSymbol*)x)->type == KN_SYM_IDENTIFIER)
#define KN_IS_WORD(x)      (KN_CHECK_TAG(x, KN_T_SYMBOL) && ((KonSymbol*)x)->type == KN_SYM_WORD)
// is a variable like @abc or a word like abc
#define KN_IS_REFERENCE(x)      (KN_CHECK_TAG(x, KN_T_SYMBOL) && (((KonSymbol*)x)->type == KN_SYM_WORD || ((KonSymbol*)x)->type == KN_SYM_VARIABLE || ((KonSymbol*)x)->type == KN_SYM_PREFIX_WORD))
#define KN_IS_PREFIX_MARCRO(x) (KN_CHECK_TAG(x, KN_T_SYMBOL) && ((KonSymbol*)x)->type == KN_SYM_PREFIX_WORD)
#define KN_IS_SYNTAX_MARKER(x) (KN_CHECK_TAG(x, KN_T_SYNTAX_MARKER))

#define KN_IS_ACCESSOR(x) (KN_CHECK_TAG(x, KN_T_ACCESSOR))

#define KN_IS_PAIR(x)       (KN_CHECK_TAG(x, KN_T_PAIR))
#define KN_IS_BLOCK(x)       (KN_CHECK_TAG(x, KN_T_BLOCK))
#define KN_IS_VECTOR(x)     (KN_CHECK_TAG(x, KN_T_VECTOR))
#define KN_IS_TABLE(x)     (KN_CHECK_TAG(x, KN_T_TABLE))
#define KN_IS_PARAM(x)     (KN_CHECK_TAG(x, KN_T_PARAM))
#define KN_IS_MAP(x)     (KN_CHECK_TAG(x, KN_T_MAP))
#define KN_IS_CELL(x)     (KN_CHECK_TAG(x, KN_T_CELL))

#define KN_IS_QUOTE(x)    (KN_CHECK_TAG(x, KN_T_QUOTE))
#define KN_IS_QUOTE_LIST(x)    (KN_CHECK_TAG(x, KN_T_QUOTE) && KN_QUOTE_TYPE(x) == KN_QUOTE_LIST)
#define KN_IS_QUOTE_NIL(x)    (KN_CHECK_TAG(x, KN_T_QUOTE) && KN_QUOTE_TYPE(x) == KN_QUOTE_LIST && ((KonQuote*)x)->inner == KN_NIL)
#define KN_IS_QUOTE_PAIR(x)    (KN_CHECK_TAG(x, KN_T_QUOTE) && KN_QUOTE_TYPE(x) == KN_QUOTE_LIST && ((KonQuote*)x)->inner != KN_NIL)
#define KN_IS_QUOTE_CELL(x)    (KN_CHECK_TAG(x, KN_T_QUOTE) && KN_QUOTE_TYPE(x) == KN_QUOTE_CELL)
#define KN_IS_QUOTE_STR(x)    (KN_CHECK_TAG(x, KN_T_QUOTE) && KN_QUOTE_TYPE(x) == KN_SYM_STRING)
#define KN_IS_QUASIQUOTE(x)    (KN_CHECK_TAG(x, KN_T_QUASIQUOTE))
#define KN_IS_QUASI_PAIR(x)    (KN_CHECK_TAG(x, KN_T_QUASIQUOTE) && KN_QUASI_TYPE(x) == KN_QUASI_LIST && ((KonQuasiquote*)x)->inner != KN_NIL)
#define KN_IS_EXPAND(x)    (KN_CHECK_TAG(x, KN_T_EXPAND))
#define KN_IS_UNQUOTE(x)    (KN_CHECK_TAG(x, KN_T_UNQUOTE))

#define KN_IS_ENV(x)        (KN_CHECK_TAG(x, KN_T_ENV))
#define KN_IS_PROCEDURE(x)        (KN_CHECK_TAG(x, KN_T_PROCEDURE))
#define KN_IS_CONTINUATION(x)        (KN_CHECK_TAG(x, KN_T_CONTINUATION))
// #define KN_IS_CPOINTER(x)   (KN_CHECK_TAG(x, KN_T_CPOINTER))
#define KN_IS_EXCEPTION(x)  (KN_CHECK_TAG(x, KN_T_EXCEPTION))

// predicates end
////

////
// data structure util start

#define KN_NEG_ONE KN_MAKE_FIXNUM(-1)
#define KN_ZERO    KN_MAKE_FIXNUM(0)
#define KN_ONE     KN_MAKE_FIXNUM(1)
#define KN_TWO     KN_MAKE_FIXNUM(2)
#define KN_THREE   KN_MAKE_FIXNUM(3)
#define KN_FOUR    KN_MAKE_FIXNUM(4)
#define KN_FIVE    KN_MAKE_FIXNUM(5)
#define KN_SIX     KN_MAKE_FIXNUM(6)
#define KN_SEVEN   KN_MAKE_FIXNUM(7)
#define KN_EIGHT   KN_MAKE_FIXNUM(8)
#define KN_NINE    KN_MAKE_FIXNUM(9)
#define KN_TEN     KN_MAKE_FIXNUM(10)


#define KN_MAKE_FIXNUM(n)    ((KN) ((((kon_int_t)(n))*(kon_int_t)((kon_int_t)1<<KN_FIXNUM_BITS)) | KN_FIXNUM_TAG))
#define KN_UNBOX_FIXNUM(n)   (((kon_int_t)((kon_uint_t)(n) & ~KN_FIXNUM_TAG))/(kon_int_t)((kon_int_t)1<<KN_FIXNUM_BITS))

#define KN_MAKE_EXT_POINTER(n)  ((KN) ((kon_uint_t)(n) + KN_EXT_POINTER_TAG) )
#define KN_UNBOX_EXT_POINTER(n)  ((KN) ((kon_uint_t)(n) & ~((kon_uint_t)KN_EXT_POINTER_MASK) ) )



static inline KN KN_MAKE_FLONUM(KonState* kstate, double num) {
  KonFlonum* result = (KonFlonum*)KN_AllocTagged(kstate, sizeof(KonFlonum), KN_T_FLONUM);
  result->flonum = num;
  return (KN)(result);
}
#define KN_UNBOX_FLONUM(n) ((KonFlonum*)n)->flonum

#define KN_FIELD(x, type, field) (((type *)(void*)x)->field)

#define kon_make_character(n)  ((KN) ((((kon_int_t)(n))<<KN_EXTENDED_BITS) + KN_CHAR_TAG))
#define KN_UNBOX_CHAR(n) ((int) (((kon_int_t)(n))>>KN_EXTENDED_BITS))

#define KN_UNBOX_STRING(x) (((KonString*)x)->string)

#define KN_UNBOX_VECTOR(x) (((KonVector*)x)->vector)

#define KN_UNBOX_TABLE(x) (((KonTable*)x)->table)
#define KN_UNBOX_MAP(x) (((KonMap*)x)->map)

#define KN_UNBOX_SYMBOL(x) (((KonSymbol*)x)->data)

#define KN_UNBOX_QUOTE(x) (((KonQuote*)x)->inner)
#define KN_QUOTE_TYPE(x) (((KonQuote*)x)->type)

#define KN_UNBOX_QUASI(x) (((KonQuasiquote*)x)->inner)
#define KN_QUASI_TYPE(x) (((KonQuasiquote*)x)->type)

#define KN_UNBOX_UNQUOTE(x) (((KonUnquote*)x)->inner)


// list
#define KN_CONS(kstate, a, b) KN_Cons(kstate, a, b)
#define KN_CAR(x)         (KN_FIELD(x, KonPair, body))
#define KN_CDR(x)         (KN_FIELD(x, KonPair, next))

#define KN_CAAR(x)      (KN_CAR(KN_CAR(x)))
#define KN_CADR(x)      (KN_CAR(KN_CDR(x)))
#define KN_CDAR(x)      (KN_CDR(KN_CAR(x)))
#define KN_CDDR(x)      (KN_CDR(KN_CDR(x)))
#define KN_CAAAR(x)     (KN_CAR(KN_CAAR(x)))
#define KN_CAADR(x)     (KN_CAR(KN_CADR(x)))
#define KN_CADAR(x)     (KN_CAR(KN_CDAR(x)))
#define KN_CADDR(x)     (KN_CAR(KN_CDDR(x)))
#define KN_CDAAR(x)     (KN_CDR(KN_CAAR(x)))
#define KN_CDADR(x)     (KN_CDR(KN_CADR(x)))
#define KN_CDDAR(x)     (KN_CDR(KN_CDAR(x)))
#define KN_CDDDR(x)     (KN_CDR(KN_CDDR(x)))
#define KN_CADDDR(x)    (KN_CADR(KN_CDDR(x))) /* just these two */
#define KN_CDDDDR(x)    (KN_CDDR(KN_CDDR(x)))

#define KN_LIST1(kstate,a)        KN_CONS((kstate), (a), KN_NIL)

// cell core, similar to car
#define KN_DCR(x)         ((KN)KN_FIELD(x, KonCell, core))
// cell next, similar to cdr
#define KN_DNR(x)         ((KN)KN_FIELD(x, KonCell, next))
// cell prev
#define KN_DPR(x)         ((KN)KN_FIELD(x, KonCell, prev))
// cell list
#define KN_DLR(x)         ((KN)KN_FIELD(x, KonCell, list))
// cell table
#define KN_DTR(x)         ((KN)KN_FIELD(x, KonCell, table))
// alias
#define KN_DCNR(x)      (KN_DCR(KN_DNR(x)))
#define KN_DTNR(x)      (KN_DTR(KN_DNR(x)))
#define KN_DLNR(x)      (KN_DLR(KN_DNR(x)))
#define KN_DCCR(x)      (KN_DCR(KN_DCR(x)))
#define KN_DTCR(x)      (KN_DTR(KN_DCR(x)))
#define KN_DLCR(x)      (KN_DLR(KN_DCR(x)))

#define KN_DCNNR(x)      (KN_DCR(KN_DNR(KN_DNR(x))))
#define KN_DTNNR(x)      (KN_DTR(KN_DNR(KN_DNR(x))))
#define KN_DLNNR(x)      (KN_DLR(KN_DNR(KN_DNR(x))))
#define KN_DCCNR(x)      (KN_DCR(KN_DCR(KN_DNR(x))))
#define KN_DTCNR(x)      (KN_DTR(KN_DCR(KN_DNR(x))))
#define KN_DLCNR(x)      (KN_DLR(KN_DCR(KN_DNR(x))))

// data structure util end
////



// data structure apis start

KN_API KN KN_Stringify(KonState* kstate, KN source);
KN KN_ToFormatString(KonState* kstate, KN source, bool newLine, int depth, char* padding);

// number
KN_API KN KN_FixnumStringify(KonState* kstate, KN source);

KN_API KN KN_MakeFlonum(KonState* kstate, double f);
KN_API KN KN_FlonumStringify(KonState* kstate, KN source);

// char
KN_API KN KN_CharStringify(KonState* kstate, KN source);

// string
KN_API KN KN_MakeString(KonState* kstate, const char* str);
KN_API KN KN_MakeEmptyString(KonState* kstate);
KN_API const char* KN_StringToCstr(KN str);
KN_API KN KN_StringStringify(KonState* kstate, KN source);

KN_API KN KN_VectorStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);

// symbol
KN_API KN KN_SymbolStringify(KonState* kstate, KN source);
KN_API const char* KN_SymbolToCstr(KN sym);

// list
// KN_API KN KN_MakePairList(KonState* kstate, ...);
KN_API KN KN_Cons(KonState* kstate, KN head, KN tail);
KN_API KN KN_PairList2(KonState* kstate, KN a, KN b);
KN_API KN KN_PairList3(KonState* kstate, KN a, KN b, KN c);

KN_API bool KN_IsPairList(KN source);
KN_API bool KN_IsBlock(KN source);

KN KN_PairListStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
KN KN_PairListRevert(KonState* kstate, KN source);
KN KN_PairListLength(KonState* kstate, KN source);

// table
KN KN_TableStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);

// block
KN KN_BlockStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
// param table
KN KN_ParamStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);

KN KN_ParamTableToList(KonState* kstate, KN source);

// map
KN KN_MapStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);

// cell
KN KN_CellStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
// eg: {sh ls -al} => [sh ls -al]
KN KN_CellCoresToList(KonState* kstate, KN source);

// attribute accessor
KN KN_AccessorStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);

KN KN_SyntaxMarkerStringify(KonState* kstate, KN source);

// @
KN KN_QuoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
// $
KN KN_QuasiquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
// eg $[].
KN KN_ExpandStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
// eg $[]e.
KN KN_UnquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);


KN MakeNativeProcedure(KonState* kstate, KonProcedureType type, KonNativeFuncRef funcRef, int paramNum, int hasVAList, int hasVAMap);
KonProcedure* MakeDispatchProc(KonState* kstate, KN procAst, KonEnv* env);
KonMsgDispatcher* MakeMsgDispatcher(KonState* kstate);
int KN_SetMsgDispatcher(KonState* kstate, unsigned int dispatcherId, KonMsgDispatcher* dispatcher);
unsigned int KN_SetNextMsgDispatcher(KonState* kstate, KonMsgDispatcher* dispatcher);
KonMsgDispatcher* KN_GetMsgDispatcher(KonState* kstate, unsigned int dispatcherId);

KonAccessor* KN_InitAccessorWithMod(KonState* kstate, char* mod);
KN KN_MakePropertyAccessor(KonState* kstate, KN value, char* mod, KonProcedure* setter);
KN KN_MakeDirAccessor(KonState* kstate, char* mod, KonProcedure* setter);
bool KN_DirAccessorPutKeyProperty(KonState* kstate, KN dir, char* key, KN property);
bool KN_DirAccessorPutKeyValue(KonState* kstate, KN dir, char* key, KN value, char* mod, KonProcedure* setter);

// data structure apis end

// common utils start
KN_API KxStringBuffer* KN_ReadFileContent(const char* filePathOrigin);
KN_API const char* KN_HumanFormatTime();

KN KN_VectorToKonPairList(KonState* kstate, KxVector* vector);

// common utils end


#endif
