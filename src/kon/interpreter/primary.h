#ifndef KON_INTERPRETER_PRIMARY_H
#define KON_INTERPRETER_PRIMARY_H

// primary functions are in a very high order
// if a func is a native function, first judge if is a primary func
// then look up in env

#ifdef __cplusplus
extern "C"{
#endif

#include "../prefix.h"
#include "types.h"

bool KON_IsPrimaryFunc(Kon* kstate, tb_string_ref_t funcName);
Kon* KON_PrimaryNewline(Kon* kstate, Kon* args);
Kon* KON_PrimaryWrite(Kon* kstate, Kon* args);
Kon* KON_PrimaryWriteln(Kon* kstate, Kon* args);

Kon* KON_ApplyPrimaryFunc(Kon* kstate, tb_string_ref_t funcName, Kon* args);

#ifdef __cplusplus
}
#endif

#endif