#ifndef KON_INTERPRETER_TYPES_H
#define KON_INTERPRETER_TYPES_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../prefix.h"
#include "../container/hashtable/kx_hashtable.h"

typedef struct _KonTrampoline KonTrampoline;


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