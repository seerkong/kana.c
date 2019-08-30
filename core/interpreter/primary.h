#ifndef KN_INTERPRETER_PRIMARY_H
#define KN_INTERPRETER_PRIMARY_H

// primary functions are in a very high order
// if a func is a native function, first judge if is a primary func
// then look up in env

#ifdef __cplusplus
extern "C"{
#endif

#include "../prefix.h"
#include "types.h"


KN KN_PrimaryNewline(KonState* kstate);
KN KN_PrimaryDisplay(KonState* kstate, KN args);
KN KN_PrimaryWrite(KonState* kstate, KN args);
KN KN_PrimaryStringify(KonState* kstate, KN args);

KN KN_PrimaryOpExport(KonState* kstate, KonEnv* env);

#ifdef __cplusplus
}
#endif

#endif