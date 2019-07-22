#ifndef KON_INTERPRETER_TYPES_H
#define KON_INTERPRETER_TYPES_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../prefix.h"
#include "../container/hashtable/kx_hashtable.h"

typedef struct _KonTrampoline KonTrampoline;
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
    KON_CONT_EVAL_CLAUSE,
    KON_CONT_EVAL_CLAUSE_ARGS,

    // native callback, use a MemoTable to store info
    KON_CONT_NATIVE_CALLBACK

} KonContinuationType;

typedef KN (*KonContFuncRef)(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked);

struct _KonContinuation {
    KonContinuationType Type;
    KN Env;
    KonContinuation* Cont;
    KxHashTable* MemoTable;

    union {
        KonContFuncRef NativeCallback;

        struct {
            KN RestSentenceList;
        } EvalSentenceList;

        struct {
            KN WordList;
        } EvalSentence;

        struct {
            KN RestWordList;
        } EvalSubj;

        struct {
            KN RestClauseList;
        } EvalClauseList;

        struct {
            KN Subj;
            KN RestArgList;
            KN EvaledArgList;
        } EvalClauseArgs;
    };
};

typedef enum {
    // Land just returns the value. It should only ever be created
    // at the very beginning of process,
    // and will be the last Trampoline value called.
    KON_TRAMPOLINE_LAND,
    // Run doesn't evaluate the value, it just runs k with it.
    // It's similar to running inline,
    // but bounces to avoid growing the stack.
    KON_TRAMPOLINE_RUN,
    // push a block scope continuation
    KON_TRAMPOLINE_BLOCK,
    // push a subj scope continuation
    KON_TRAMPOLINE_SUBJ,
    // push a sentence scope continuation
    KON_TRAMPOLINE_CLAUSE_LIST,
    // push a clause scope continuation
    KON_TRAMPOLINE_ARG_LIST,
} KonBounceType;

struct _KonTrampoline {
    KonBounceType Type;
    union {
        struct {
            KN Value;
        } Land;

        struct {
            KN Value;
            KonContinuation* Cont;
        } Run;

        struct {
            KN Value;
            KonContinuation* Cont;
            KN Env;
        } Bounce;

        struct {
            KN Subj;
            KN Value;
            KonContinuation* Cont;
            KN Env;
        } SubjBounce;
    };
};

#define kon_bounce_type(x)      ((x)->Type)
#define kon_continuation_type(x)      ((x)->Type)


#ifdef __cplusplus
}
#endif

#endif