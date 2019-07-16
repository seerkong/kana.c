
#include "../../prefix.h"
#include "../types.h"

// like scheme (begin expr1 expr2 ...)
KonTrampoline* KON_EvalPrefixDo(KonState* kstate, KN expression, KN env, KonContinuation* cont);
