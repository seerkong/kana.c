
#include "../../prefix.h"
#include "../types.h"

KonTrampoline* KON_EvalPrefixLambda(KonState* kstate, KN expression, KN env, KonContinuation* cont);
KonTrampoline* KON_ApplyCompositeLambda(KonState* kstate, KonProcedure* proc, KN argList, KN env, KonContinuation* cont);
