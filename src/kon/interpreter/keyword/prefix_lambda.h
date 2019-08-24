
#include "../../prefix.h"
#include "../types.h"

KonTrampoline* KN_EvalPrefixLambda(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont);
KonTrampoline* KN_ApplyCompositeLambda(KonState* kstate, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* cont);
