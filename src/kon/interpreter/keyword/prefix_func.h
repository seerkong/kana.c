
#include "../../prefix.h"
#include "../types.h"

KonTrampoline* KON_EvalPrefixFunc(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont);
KonTrampoline* KON_ApplyCompositeFunc(KonState* kstate, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* cont);
