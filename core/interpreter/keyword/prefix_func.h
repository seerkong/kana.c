
#include "../../prefix.h"
#include "../types.h"

KonTrampoline* KN_EvalPrefixFunc(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont);
KonTrampoline* KN_ApplyCompositeFunc(KonState* kstate, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* cont);
