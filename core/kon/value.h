#ifndef KN_KON_VALUE_H
#define KN_KON_VALUE_H 1

#include "prefix.h"


////
// types start
typedef struct _Kana Kana;

typedef struct _KonBase KonBase;
typedef struct _KonFlonum KonFlonum;
typedef struct _KonSymbol KonSymbol;
typedef struct _KonSyntaxMarker KonSyntaxMarker;
typedef struct _KonString KonString;
typedef struct _KonTable KonTable;
typedef struct _KonVector KonVector;
typedef struct _KonPair KonPair;
typedef struct _KonMap KonMap;
typedef struct _KonCell KonCell;
typedef struct _KonParam KonParam;
typedef struct _KonBlock KonBlock;
typedef struct _KonTxtMarcro KonTxtMarcro;
typedef struct _KonObjBuilder KonObjBuilder;
typedef struct _KonPrefix KonPrefix;
typedef struct _KonSuffix KonSuffix;
typedef struct _KonQuote KonQuote;
typedef struct _KonQuasiquote KonQuasiquote;
typedef struct _KonExpand KonExpand;
typedef struct _KonUnquote KonUnquote;
typedef struct _KonEnv KonEnv;
typedef struct _KonAccessor KonAccessor;
typedef struct _KonMsgDispatcher KonMsgDispatcher;
typedef struct _KonProcedure KonProcedure;
typedef struct _KonContinuation KonContinuation;

typedef union _KonValue KonValue;
// typedef volatile union _KN KN;
typedef union _KN KN;


union _KN {
    union _KonValue* asKon;    /* collectable objects */
    int32_t asI32;
    uint64_t asU64;
    unsigned char asBytes[sizeof(double)];
    double asF64;
    void* asPtr;
};

#define NUNBOX_OBJ_PTR_TYPE KN
#include "../box/box.h"
#include "../gc/gc.h"

typedef enum {
    // determined by tagging system
    KN_T_FIXNUM = 1,     //
    KN_T_POINTER,        //
    KN_T_IMMDT_SYMBOL,   //
    KN_T_CHAR,           //
    KN_T_UNIQUE_IMMDT,   //
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
    KN_T_TXT_MARCRO,
    KN_T_OBJ_BUILDER,
    KN_T_PREFIX,
    KN_T_SUFFIX,
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

// not used
#define KN_GC_MARK_WHITE '0'
// allocated between two continuation switch
#define KN_GC_MARK_GRAY '1'
// mark in process
#define KN_GC_MARK_RED '2'
// can be reached from root, liveness: true
#define KN_GC_MARK_BLACK '3'

struct _KonBase {
    KonType tag;
    // unboxed fixnum
    unsigned int msgDispatcherId;
    char gcMarkColor;
    KN prefix;
    KN suffix;
};


typedef enum {
    KN_SYM_MARCRO, // ass!
    KN_SYM_CELL_SEG_END, // ^ass
    KN_SYM_WORD,  // abc
    KN_SYM_VARIABLE,    // @abc
    KN_SYM_IDENTIFIER, // $abc
    KN_SYM_STRING, // $''
} KonSymbolType;

struct _KonSymbol {
    KonBase base;
    const char* data;
    KonSymbolType type;
};

struct _KonObjBuilder {
    KonBase base;
    KN inner;
    KN name;
};

struct _KonTxtMarcro {
    KonBase base;
    KN inner;
    KN name;
};

struct _KonPrefix {
    KonBase base;
    KN inner;
};

struct _KonSuffix {
    KonBase base;
    KN inner;
};

typedef enum {
    KN_QUOTE_LIST,         // $.[1 2 3]
    KN_QUOTE_CELL          // $.{ojb (:a 1 :b 2)}
} KonQuoteType;

struct _KonQuote {
    KonBase base;
    KN inner;
    KN name;
    KonQuoteType type;
};


typedef enum {
    KN_QUASI_LIST,         // @.[1 2 3] @xx.[1 2 3]
    KN_QUASI_CELL          // @.{ojb (:a 1 :b 2)} @fa.{ff}
} KonQuasiquoteType;

struct _KonQuasiquote {
    KonBase base;
    KN inner;
    KN name;
    KonQuasiquoteType type;
};

typedef enum {
    KN_EXPAND_REPLACE,          // @.abc
    KN_EXPAND_SEQ,        // @~.[1 2 3]
    KN_EXPAND_KV        // @%.(:a 1 :b 2)
} KonExpandType;

struct _KonExpand {
    KonBase base;
    KN inner;
    KonExpandType type;
};


typedef enum {
    KN_UNQUOTE_REPLACE,          // $.abc
    KN_UNQUOTE_SEQ,        // $~.[(1 2 3)]
    KN_UNQUOTE_KV        // $%.[#(:a $var :b 2)]
} KonUnquoteType;

struct _KonUnquote {
    KonBase base;
    KN inner;
    KonUnquoteType type;
};


typedef enum {
    KN_SYNTAX_MARKER_APPLY,        // %
    KN_SYNTAX_MARKER_EQUAL,        // =
    KN_SYNTAX_MARKER_ASSIGN,        // :=
    KN_SYNTAX_MARKER_MSG_SIGNAL,   // .
    KN_SYNTAX_MARKER_GET_LVALUE,        // \:abc
    KN_SYNTAX_MARKER_GET_RVALUE,        // \abc
    KN_SYNTAX_MARKER_PROC_PIPE,         // |
    KN_SYNTAX_MARKER_CLAUSE_END    // ;
} KonSyntaxMarkerType;

// eg: % | ;
struct _KonSyntaxMarker {
    KonBase base;
    KonSyntaxMarkerType type;
};


struct _KonPair {
    KonBase base;
    KN prev;
    KN body;
    KN next;
};

struct _KonBlock {
    KonBase base;
    KN prev;
    KN body;
    KN next;
};


struct _KonMap {
    KonBase base;
    KnHashTable* map;
};

struct _KonCell {
    KonBase base;
    KN core;
    KonSuffix* suffix;
    KonVector* vector;
    KonMap* map;
    KonTable* table;
    KonPair* list;
    KonCell* next;
    KonCell* prev;
};

struct _KonEnv {
    KonBase base;
    KonEnv* parent;
    KnHashTable* bindings;
};


// an accessor is a value wrapper
// like file folders and files
// if IsDir is true, data is stored in Dir, 
// if IsDir is false, means it is a leaf node, is a property
struct _KonAccessor {
    KonBase base;
    bool isDir; // d
    bool openToRef;    // r. only used in dir mod
    bool openToChildren;
    bool openToSibling;
    bool canWrite;  // w
    // bool canExec;    // x func, blk, lambda
    // bool isMethod;  // m func(self, p2, p3)
    KnHashTable* dir;   // key: cstr, value: KonAccessor*

    KN value;
    KonProcedure* setter;
    // KonProcedure* getter;
};


struct _KonMsgDispatcher {
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

typedef KN (*KonNativeFuncRef)(Kana* kana, ...);



struct _KonProcedure {
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

struct _KnOp {
    uint8_t code;

    union {
        struct {
            int a:24;
        } one;

        struct {
            int a:8;
            int b:16;
        } two;
        struct {
            int a:6;
            int b:6;
            int c:12;
        } three;
    };

};

typedef struct _KnOp KnOp;


struct _KnFrame {
    KonContinuation* curCont;
    KonEnv* curEnv;
    KnOp* codeSegment;
    int pc; // program counter, offset of codesegment
    KnOp* ir;   // instructinon register
    struct _KnFrame* prev;
};
typedef struct _KnFrame KnFrame;

typedef enum {
    // should be the first continuation created
    KN_CONT_RETURN,
    KN_CONT_QUIT_BOUNCE_MODE,

    KN_CONT_NATIVE_HANDLER,
} KonContinuationType;


typedef void (*KonContFuncRef)(Kana* kana, KonContinuation* curCont);

struct _KonContinuation {
    KonBase base;
    KonContinuationType type;
    // a Return continuation's Cont is empty
    KonContinuation* next;

    // use which function to run this continuation
    KonContFuncRef contHandler;

    KonEnv* env;
    KnList* pendingJobs;
    KnList* finishedJobs;
    int status;
    KN memo[4];
};


struct _KonFlonum {
    KonBase base;
    double flonum;
};

// TODO replace to kon string impl
struct _KonString {
    KonBase base;
    KxStringBuffer* string;
};

struct _KonVector {
    KonBase base;
    KnVector* vector;
};

struct _KonTable {
    KonBase base;
    KnHashTable* table;
};

struct _KonParam {
    KonBase base;
    KnHashTable* table;
};


union _KonValue {
    struct _KonBase konBase;
    struct _KonSymbol konSymbol;
    struct _KonSyntaxMarker konSyntaxMarker;
    struct _KonString konString;
    struct _KonTable konTable;
    struct _KonVector konVector;
    struct _KonPair konPair;
    struct _KonMap konMap;
    struct _KonCell konCell;
    struct _KonTxtMarcro konTxtMarcro;
    struct _KonObjBuilder konObjBuilder;
    struct _KonPrefix konPrefix;
    struct _KonSuffix konSuffix;
    struct _KonQuote konQuote;
    struct _KonQuasiquote konQuasiquote;
    struct _KonExpand konExpand;
    struct _KonMsgDispatcher konMsgDispatcher;
    struct _KonEnv konEnv;
    struct _KonAccessor konAccessor;
    struct _KonProcedure konProcedure;
    struct _KonContinuation konContinuation;
};


// union _KonValue {
//     uint64_t asI3264;
//     double asF64;
//     KonBase* KonBase;
//     KonSymbol* KonSymbol;
//     KonSyntaxMarker* KonSyntaxMarker;
//     KonString* KonString;
//     KonTable* KonTable;
//     KonVector* asKonVector;
//     KonPair* KonPair;
//     KonMap* KonMap;
//     KonCell* KonCell;
//     KonQuote* KonQuote;
//     KonQuasiquote* KonQuasiquote;
//     KonExpand* KonExpand;
//     KonMsgDispatcher* KonMsgDispatcher;
//     KonEnv* KonEnv;
//     KonAccessor* KonAccessor;
//     KonProcedure* KonProcedure;
//     KonContinuation* KonContinuation;
// };

// #undef NUNBOX_OBJ_PTR_TYPE
// #define NUNBOX_OBJ_PTR_TYPE KonValue
// #include "../box/box.h"
// #include "../gc/gc.h"

struct _GcState;

struct _Kana {
    // KonBase base;
    KonEnv* rootEnv;

    ////
    // gc root source start

    // how dispatch a message
    KnVector* msgDispatchers;
    unsigned int nextMsgDispatcherId;

    struct _KonContinuation* currCont;
    // temp pointer type KN list allocated between two
    // continuation switch
    KnList* writeBarrierGen;
    KN currCode;

    // gc root source end
    ////

    // KN* knRegs;
    KN knRegs[16];
    KnOp NEXT_OP;    // next instructinon register, used in bounce-trampoline mode
    KnFrame* FRAME;  // current frame
    
    KnOp* CS;   // byte code segment register
    int* PC;   // program counter register, offset of CS register
    // usually, IR = CS + *PC
    // but when in bounce-trampoline interprete mode, IR is pointed to NEXT_OP
    KnOp** IR; // instructinon register
    

    struct _GcState* gcState;

    // a list of KnVector. store heap pointers
    KnList* heapPtrSegs;
    KnVector* segmentMaxSizeVec;    // buffsize of each ptr segment
    unsigned long maxObjCntLimit;    // how many objs can be stored in ptr seg
    unsigned long gcThreshold;  // trigger gc when obj count bigger than this number
    bool needGc;

    // mark task queue. mark the value grey before add to this queue
    KnList* markTaskQueue;
    
    tb_allocator_ref_t largeConstAllocator;
    tb_allocator_ref_t constAllocator;
    tb_allocator_ref_t largeAllocator;
    tb_allocator_ref_t dynamicAllocator;   // default allocator


};

// types end
////


KN_API unsigned int KN_NodeDispacherId(Kana* kana, KN obj);

#define KN_NEW_CONST_OBJ(kana,t,tag)  ((t *)(KN_NewConstMemObj(kana, sizeof(t), tag).asKon))
#define KN_NEW_DYNAMIC_OBJ(kana,t,tag)  ((t *)(KN_NewDynamicMemObj(kana, sizeof(t), tag).asKon))
#define KN_FREE(kana, ptr) KN_GC_FREE(ptr)


#define KON_2_KN(v)  ((KN)(KonValue*)v)
#define KN_2_KNBOX(v)  ((KnBox)(v.asF64))

// alt to CAST_Kon
#define KN_2_KON(v, t)  ((Kon##t *)((v).asKon))
//#define KN_2_KON(v, t)  ((v).asObjPtr.asKon->asKon##t)
//#define KN_2_KON(v, t)  ((v).asKon->asKon##t)



#define KN_IS_EMPTY(x)    ((x.asU64) != KNBOX_EMPTY)
#define KN_IS_FALSE(x)      ((x.asU64) == KNBOX_FALSE)
#define KN_IS_TRUE(x)    ((x.asU64) != KNBOX_TRUE)
#define KN_IS_NIL(x)    ((x.asU64) == KNBOX_NIL || (KN_CHECK_TAG(x, KN_T_QUOTE) && KN_QUOTE_TYPE(x) == KN_QUOTE_LIST && (KN_FIELD(x, Quote, inner) ).asU64 == KNBOX_NIL))
#define KN_IS_UNDEF(x)    ((x.asU64) == KNBOX_UNDEF)
#define KN_IS_UKN(x)    ((x.asU64) == KNBOX_UKN)
#define KN_IS_DELETED(x)    ((x.asU64) == KNBOX_DELETED)

#define KN_IS_BOOLEAN(x) KnBox_IsBoolean(KN_2_KNBOX(x))

#define KN_EMPTY       ((KN)KNBOX_EMPTY)
#define KN_FALSE       ((KN)KNBOX_FALSE)
#define KN_TRUE        ((KN)KNBOX_TRUE)
#define KN_NIL         ((KN)KNBOX_NIL)
#define KN_UNDEF       ((KN)KNBOX_UNDEF)
#define KN_UKN         ((KN)KNBOX_UKN)
#define KN_DELETED     ((KN)KNBOX_DELETED)

// #define KN_EMPTY       KNBOX_EMPTY
// #define KN_FALSE       KNBOX_FALSE
// #define KN_TRUE        KNBOX_TRUE
// #define KN_NIL         KNBOX_NIL
// #define KN_UNDEF       KNBOX_UNDEF
// #define KN_UKN         KNBOX_UKN
// #define KN_DELETED     KNBOX_DELETED

#define KN_NEG_ONE KN_BOX_INT(-1)
#define KN_ZERO    KN_BOX_INT(0)
#define KN_ONE     KN_BOX_INT(1)
#define KN_TWO     KN_BOX_INT(2)
#define KN_THREE   KN_BOX_INT(3)
#define KN_FOUR    KN_BOX_INT(4)
#define KN_FIVE    KN_BOX_INT(5)
#define KN_SIX     KN_BOX_INT(6)
#define KN_SEVEN   KN_BOX_INT(7)
#define KN_EIGHT   KN_BOX_INT(8)
#define KN_NINE    KN_BOX_INT(9)
#define KN_TEN     KN_BOX_INT(10)

// alt to KN_MAKE_FIXNUM, KN_UNBOX_FIXNUM
#define KN_BOX_INT(n) ((KN)(KnBox_FromInt(n).asF64))
#define KN_UNBOX_INT(n) KnBox_ToInt((KnBox)n.asF64)
#define KN_IS_INT(x)  KnBox_IsInt(KN_2_KNBOX(x))
#define KN_IS_FIXNUM  KN_IS_INT
#define KN_MAKE_FIXNUM KN_BOX_INT
#define KN_UNBOX_FIXNUM KN_UNBOX_INT

#define KN_IS_DOUBLE(x)      KnBox_IsDouble(KN_2_KNBOX(x))
#define KN_BOX_DOUBLE(n) ((KN)(KnBox_FromDouble(n).asF64))
#define KN_UNBOX_DOUBLE(n) KnBox_ToDouble((KnBox)n.asF64)
#define KN_IS_FLONUM KN_IS_DOUBLE
#define KN_MAKE_FLONUM KN_BOX_DOUBLE
#define KN_UNBOX_FLONUM KN_UNBOX_DOUBLE

#define KN_IS_CHAR(x)    KnBox_IsChar(KN_2_KNBOX(x))
#define KN_BOX_CHAR(n)  ((KN)(KnBox_FromChar(n).asF64))
#define KN_UNBOX_CHAR(n) KnBox_ToChar((KnBox)n.asF64)


#define KN_IS_EXT_POINTER(x) KnBox_IsExtPtr(KN_2_KNBOX(x))
// alt to KN_MAKE_EXT_POINTER, KN_UNBOX_EXT_POINTER
#define KN_BOX_EXT_PTR(v) ((KN)(KnBox_FromExtPtr(v).asF64))
#define KN_UNBOX_EXT_PTR(v) KnBox_ToExtPtr((KnBox)(v.asF64))



#define KN_IS_POINTER(x) KnBox_IsObjPtr(KN_2_KNBOX(x))
#define KN_FIELD(x, type, field) KN_2_KON(x, type)->field
#define KN_OBJ_PTR_TYPE(x)     KN_2_KON(x, Base)->tag
#define KN_PTR_TYPE KN_OBJ_PTR_TYPE
#define KN_GET_PTR_TAG KN_OBJ_PTR_TYPE

#define KN_CHECK_TAG(x,t)  (KN_IS_POINTER(x) && (KN_OBJ_PTR_TYPE(x) == (t)))


#define KN_IS_BYTES(x)          (KN_CHECK_TAG(x, KN_T_BYTES))

// is KonString
#define KN_IS_STRING(x)         (KN_CHECK_TAG(x, KN_T_STRING))
#define KN_UNBOX_STRING(x)      KN_FIELD(x, String, string)
#define KN_STRING_2_CSTR(x)      KxStringBuffer_Cstr(KN_FIELD(x, String, string))

#define KN_IS_SHORT_STR(x)    KnBox_IsShortStr(KN_2_KNBOX(x))
#define KN_BOX_SHORT_STR(n)  (KN)(KnBox_ShortStrCreate(n, strlen(n)).asF64)
// #define KN_UNBOX_SHORT_STR(n) KnBox_ShortStrChars((KnBox)n.asF64)
#ifdef NUNBOX_BIG_ENDIAN
#define KN_UNBOX_SHORT_STR(val) ((char*)(val.asBytes + 2))
#else
#define KN_UNBOX_SHORT_STR(val) ((char*)(val.asBytes))
#endif

// unbox short string or string object
#define KN_UNBOX_STR(val) (KN_IS_SHORT_STR(val) ? KN_UNBOX_SHORT_STR(val) : KN_STRING_2_CSTR(val))


#define KN_IS_SYMBOL(x)         (KN_CHECK_TAG(x, KN_T_SYMBOL))
#define KN_UNBOX_SYMBOL(x)      KN_FIELD(x, Symbol, data)

#define KN_IS_VARIABLE(x)      (KN_CHECK_TAG(x, KN_T_SYMBOL) && KN_FIELD(x, Symbol, type) == KN_SYM_VARIABLE)

#define KN_IS_IDENTIFIER(x)      (KN_CHECK_TAG(x, KN_T_SYMBOL) && KN_FIELD(x, Symbol, type) == KN_SYM_IDENTIFIER)

#define KN_IS_WORD(x)      (KN_CHECK_TAG(x, KN_T_SYMBOL) && KN_FIELD(x, Symbol, type) == KN_SYM_WORD)

// is a variable like @abc or a word like abc
#define KN_IS_REFERENCE(x)      (KN_CHECK_TAG(x, KN_T_SYMBOL) && KN_FIELD(x, Symbol, type) == KN_SYM_WORD || KN_FIELD(x, Symbol, type) == KN_SYM_VARIABLE || KN_FIELD(x, Symbol, type) == KN_SYM_MARCRO)

#define KN_IS_SYM_MARCRO(x) (KN_CHECK_TAG(x, KN_T_SYMBOL) && KN_FIELD(x, Symbol, type) == KN_SYM_MARCRO)

#define KN_IS_SYNTAX_MARKER(x) (KN_CHECK_TAG(x, KN_T_SYNTAX_MARKER))

#define KN_IS_ACCESSOR(x) (KN_CHECK_TAG(x, KN_T_ACCESSOR))

#define KN_IS_PAIR(x)       (KN_CHECK_TAG(x, KN_T_PAIR))

#define KN_IS_BLOCK(x)       (KN_CHECK_TAG(x, KN_T_BLOCK))

#define KN_IS_VECTOR(x)     (KN_CHECK_TAG(x, KN_T_VECTOR))
#define KN_UNBOX_VECTOR(x)  KN_FIELD(x, Vector, vector)

#define KN_IS_TABLE(x)     (KN_CHECK_TAG(x, KN_T_TABLE))
#define KN_UNBOX_TABLE(x)   KN_FIELD(x, Table, table)

#define KN_IS_PARAM(x)     (KN_CHECK_TAG(x, KN_T_PARAM))

#define KN_IS_MAP(x)     (KN_CHECK_TAG(x, KN_T_MAP))
#define KN_UNBOX_MAP(x)   KN_FIELD(x, Map, map)

#define KN_IS_CELL(x)     (KN_CHECK_TAG(x, KN_T_CELL))

#define KN_QUOTE_TYPE(x)  KN_FIELD(x, Quote, type)
#define KN_IS_QUOTE(x)    (KN_CHECK_TAG(x, KN_T_QUOTE))
#define KN_IS_QUOTE_LIST(x)    (KN_CHECK_TAG(x, KN_T_QUOTE) && KN_QUOTE_TYPE(x) == KN_QUOTE_LIST)
#define KN_IS_QUOTE_NIL(x)    (KN_CHECK_TAG(x, KN_T_QUOTE) && KN_QUOTE_TYPE(x) == KN_QUOTE_LIST && (KN_FIELD(x, Quote, inner).asU64 == KNBOX_NIL))
#define KN_IS_QUOTE_PAIR(x)    (KN_CHECK_TAG(x, KN_T_QUOTE) && KN_QUOTE_TYPE(x) == KN_QUOTE_LIST && (KN_FIELD(x, Quote, inner).asU64 != KNBOX_NIL))
#define KN_IS_QUOTE_CELL(x)    (KN_CHECK_TAG(x, KN_T_QUOTE) && KN_QUOTE_TYPE(x) == KN_QUOTE_CELL)
#define KN_IS_QUOTE_STR(x)    (KN_CHECK_TAG(x, KN_T_QUOTE) && KN_QUOTE_TYPE(x) == KN_SYM_STRING)
#define KN_UNBOX_QUOTE(x)       KN_FIELD(x, Quote, inner)

#define KN_QUASI_TYPE(x) KN_FIELD(x, Quasiquote, type)
#define KN_IS_QUASIQUOTE(x)    (KN_CHECK_TAG(x, KN_T_QUASIQUOTE))
#define KN_IS_QUASI_PAIR(x)    (KN_CHECK_TAG(x, KN_T_QUASIQUOTE) && KN_QUASI_TYPE(x) == KN_QUASI_LIST && (KN_FIELD(x, Quasiquote, inner).asU64 != KNBOX_NIL))
#define KN_UNBOX_QUASI(x) KN_FIELD(x, Quasiquote, inner)

#define KN_IS_EXPAND(x)    (KN_CHECK_TAG(x, KN_T_EXPAND))

#define KN_IS_UNQUOTE(x)    (KN_CHECK_TAG(x, KN_T_UNQUOTE))
#define KN_UNBOX_UNQUOTE(x) KN_FIELD(x, Unquote, inner)

#define KN_IS_PREFIX(x)    (KN_CHECK_TAG(x, KN_T_PREFIX))
#define KN_UNBOX_PREFIX(x) KN_FIELD(x, Prefix, inner)

#define KN_IS_SUFFIX(x)    (KN_CHECK_TAG(x, KN_T_SUFFIX))
#define KN_UNBOX_SUFFIX(x) KN_FIELD(x, Suffix, inner)

#define KN_IS_TXT_MARCRO(x)    (KN_CHECK_TAG(x, KN_T_TXT_MARCRO))
#define KN_UNBOX_TXT_MARCRO(x) KN_FIELD(x, TxtMarcro, inner)

#define KN_IS_OBJ_BUILDER(x)    (KN_CHECK_TAG(x, KN_T_OBJ_BUILDER))
#define KN_UNBOX_OBJ_BUILDER(x) KN_FIELD(x, ObjBuilder, inner)

#define KN_IS_ENV(x)        (KN_CHECK_TAG(x, KN_T_ENV))

#define KN_IS_PROCEDURE(x)        (KN_CHECK_TAG(x, KN_T_PROCEDURE))

#define KN_IS_CONTINUATION(x)        (KN_CHECK_TAG(x, KN_T_CONTINUATION))

#define KN_IS_EXCEPTION(x)  (KN_CHECK_TAG(x, KN_T_EXCEPTION))





// list
#define KN_CONS(kana, a, b) KN_Cons(kana, a, b)
#define KN_CAR(x)         (KN_FIELD(x, Pair, body))
#define KN_CDR(x)         (KN_FIELD(x, Pair, next))

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

#define KN_LIST1(kana,a)        KN_CONS((kana), (a), KN_NIL)

// cell core, similar to car
#define KN_DCR(x)         (KN_FIELD(x, Cell, core))
// cell next, similar to cdr
#define KN_DNR(x)         (KON_2_KN(KN_FIELD(x, Cell, next)))
// cell prev
#define KN_DPR(x)         (KON_2_KN(KN_FIELD(x, Cell, prev)))
// cell list
#define KN_DLR(x)         (KON_2_KN(KN_FIELD(x, Cell, list)))
// cell table
#define KN_DTR(x)         (KON_2_KN(KN_FIELD(x, Cell, table)))
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

KN_API KN KN_Stringify(Kana* kana, KN source);
KN KN_ToFormatString(Kana* kana, KN source, bool newLine, int depth, char* padding);
void KN_PrintNodeToStdio(Kana* kana, KN source);

// number
KN_API KN KN_FixnumStringify(Kana* kana, KN source);

KN_API KN KN_MakeFlonum(Kana* kana, double f);
KN_API KN KN_FlonumStringify(Kana* kana, KN source);

// char
KN_API KN KN_CharStringify(Kana* kana, KN source);

// string
KN_API KN KN_MakeString(Kana* kana, const char* str);
KN_API KN KN_MakeEmptyString(Kana* kana);
KN_API const char* KN_StringToCstr(KN str);
KN_API KN KN_StringStringify(Kana* kana, KN source);

KN_API KN KN_VectorStringify(Kana* kana, KN source, bool newLine, int depth, char* padding);

// symbol
KN_API KN KN_SymbolStringify(Kana* kana, KN source);
KN_API const char* KN_SymbolToCstr(KN sym);

// list
// KN_API KN KN_MakePairList(Kana* kana, ...);
KN_API KN KN_Cons(Kana* kana, KN head, KN tail);
KN_API KN KN_PairList2(Kana* kana, KN a, KN b);
KN_API KN KN_PairList3(Kana* kana, KN a, KN b, KN c);

KN_API bool KN_IsPairList(KN source);
KN_API bool KN_IsBlock(KN source);

KN KN_PairListStringify(Kana* kana, KN source, bool newLine, int depth, char* padding);
KN KN_PairListRevert(Kana* kana, KN source);
KN KN_PairListLength(Kana* kana, KN source);

// table
KN KN_TableStringify(Kana* kana, KN source, bool newLine, int depth, char* padding);

// cell
KN KN_CellStringify(Kana* kana, KN source, bool newLine, int depth, char* padding);
// eg: {sh ls -al} => [sh ls -al]
KN KN_CellCoresToList(Kana* kana, KN source);

// attribute accessor
KN KN_AccessorStringify(Kana* kana, KN source, bool newLine, int depth, char* padding);

KN KN_SyntaxMarkerStringify(Kana* kana, KN source);

// @
KN KN_QuoteStringify(Kana* kana, KN source, bool newLine, int depth, char* padding);
// $
KN KN_QuasiquoteStringify(Kana* kana, KN source, bool newLine, int depth, char* padding);
// eg $[].
KN KN_ExpandStringify(Kana* kana, KN source, bool newLine, int depth, char* padding);
// eg $[]e.
KN KN_UnquoteStringify(Kana* kana, KN source, bool newLine, int depth, char* padding);

KN KN_PrefixStringify(Kana* kana, KN source, bool newLine, int depth, char* padding);
KN KN_SuffixStringify(Kana* kana, KN source, bool newLine, int depth, char* padding);
// ^xx.""
KN KN_TxtMarcroStringify(Kana* kana, KN source, bool newLine, int depth, char* padding);
// #[] #hashmap.(:a xx)
KN KN_ObjBuilderStringify(Kana* kana, KN source, bool newLine, int depth, char* padding);


KN MakeNativeProcedure(Kana* kana, KonProcedureType type, KonNativeFuncRef funcRef, int paramNum, int hasVAList, int hasVAMap);
KonProcedure* MakeDispatchProc(Kana* kana, KN procAst, KonEnv* env);
KonMsgDispatcher* MakeMsgDispatcher(Kana* kana);
int KN_SetMsgDispatcher(Kana* kana, unsigned int dispatcherId, KonMsgDispatcher* dispatcher);
unsigned int KN_SetNextMsgDispatcher(Kana* kana, KonMsgDispatcher* dispatcher);
KonMsgDispatcher* KN_GetMsgDispatcher(Kana* kana, unsigned int dispatcherId);

KonAccessor* KN_InitAccessorWithMod(Kana* kana, char* mod);
KonAccessor* KN_MakePropertyAccessor(Kana* kana, KN value, char* mod, KonProcedure* setter);
KonAccessor* KN_MakeDirAccessor(Kana* kana, char* mod, KonProcedure* setter);
bool KN_DirAccessorPutKeyProperty(Kana* kana, KN dir, char* key, KN property);
bool KN_DirAccessorPutKeyValue(Kana* kana, KN dir, char* key, KN value, char* mod, KonProcedure* setter);

// data structure apis end

// common utils start
KN_API KxStringBuffer* KN_ReadFileContent(const char* filePathOrigin);
KN_API const char* KN_HumanFormatTime();

KN KN_VectorToKonPairList(Kana* kana, KnVector* vector);

// common utils end

KonContinuation* AllocContinuationWithType(Kana* kana, KonContinuationType type, KonEnv* env, KonContinuation* nextCont);


#endif