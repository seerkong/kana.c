
#include "../../prefix.h"
#include "../types.h"

KonTrampoline* KON_EvalPrefixBlk(KonState* kstate, KN expression, KN env, KonContinuation* cont);
KonTrampoline* KON_ApplyCompositeBlk(KonState* kstate, KonProcedure* proc, KN argList, KN env, KonContinuation* cont);
