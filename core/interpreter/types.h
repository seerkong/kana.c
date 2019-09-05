#ifndef KN_INTERPRETER_TYPES_H
#define KN_INTERPRETER_TYPES_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../prefix.h"
#include "../container/kx_hashtable.h"

typedef struct _KonTrampoline KonTrampoline;


typedef enum {
    // Land just returns the value. It should only ever be created
    // at the very beginning of process,
    // and will be the last Trampoline value called.
    KN_TRAMPOLINE_LAND,
    // Run doesn't evaluate the value, it just runs k with it.
    // It's similar to running inline,
    // but bounces to avoid growing the stack.
    KN_TRAMPOLINE_RUN,
    // push a block scope continuation
    KN_TRAMPOLINE_BLOCK,
    // push a subj scope continuation
    KN_TRAMPOLINE_SUBJ,
    // push a sentence scope continuation
    KN_TRAMPOLINE_CLAUSE_LIST,
    // push a clause scope continuation
    KN_TRAMPOLINE_ARG_LIST,
    // eval quasiquote list item
    KN_TRAMPOLINE_QUASI_ITEM,
} KonBounceType;

struct _KonTrampoline {
    KonBounceType type;
    // cont is empty when is a land trampoline
    KonContinuation* cont;

    union {
        struct {
            KN value;
        } land;

        struct {
            KN value;
        } run;

        struct {
            KN value;
            KonEnv* env;
        } bounce;
    };
};

#define kon_bounce_type(x)      ((x)->type)
#define kon_continuation_type(x)      ((x)->type)


#ifdef __cplusplus
}
#endif

#endif