
#include "../../prefix.h"
#include "../types.h"

KonTrampoline* KON_EvalPrefixFunc(KonState* kstate, KN expression, KN env, KonContinuation* cont);
KonTrampoline* KON_ApplyCompositeFunc(KonState* kstate, KonProcedure* proc, KN argList, KN env, KonContinuation* cont);
