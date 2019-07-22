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


KN KON_PrimaryPlus(KonState* kstate, KN args);
KN KON_PrimaryMinus(KonState* kstate, KN args);
KN KON_PrimaryMultiply(KonState* kstate, KN args);
KN KON_PrimaryDivide(KonState* kstate, KN args);
KN KON_PrimaryMod(KonState* kstate, KN args);
KN KON_PrimaryLowerThan(KonState* kstate, KN args);
KN KON_PrimaryLowerOrEqual(KonState* kstate, KN args);
KN KON_PrimaryGreaterThan(KonState* kstate, KN args);
KN KON_PrimaryGreaterOrEqual(KonState* kstate, KN args);


KN KON_PrimaryNewline(KonState* kstate, KN args);
KN KON_PrimaryDisplay(KonState* kstate, KN args);
KN KON_PrimaryDisplayln(KonState* kstate, KN args);
KN KON_PrimaryWrite(KonState* kstate, KN args);
KN KON_PrimaryWriteln(KonState* kstate, KN args);
KN KON_PrimaryStringify(KonState* kstate, KN args);

#ifdef __cplusplus
}
#endif

#endif