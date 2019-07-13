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

Kon* KON_PrimaryPlus(Kon* kstate, Kon* args);
Kon* KON_PrimaryMinus(Kon* kstate, Kon* args);
Kon* KON_PrimaryMultiply(Kon* kstate, Kon* args);
Kon* KON_PrimaryDivide(Kon* kstate, Kon* args);
Kon* KON_PrimaryMod(Kon* kstate, Kon* args);
Kon* KON_PrimaryLowerThan(Kon* kstate, Kon* args);
Kon* KON_PrimaryLowerOrEqual(Kon* kstate, Kon* args);
Kon* KON_PrimaryGreaterThan(Kon* kstate, Kon* args);
Kon* KON_PrimaryGreaterOrEqual(Kon* kstate, Kon* args);


Kon* KON_PrimaryNewline(Kon* kstate, Kon* args);
Kon* KON_PrimaryDisplay(Kon* kstate, Kon* args);
Kon* KON_PrimaryDisplayln(Kon* kstate, Kon* args);
Kon* KON_PrimaryWrite(Kon* kstate, Kon* args);
Kon* KON_PrimaryWriteln(Kon* kstate, Kon* args);
Kon* KON_PrimaryStringify(Kon* kstate, Kon* args);

#ifdef __cplusplus
}
#endif

#endif