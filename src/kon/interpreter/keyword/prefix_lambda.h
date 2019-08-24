
#include "../../prefix.h"
#include "../types.h"

KonTrampoline* KON_EvalPrefixLambda(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont);
KonTrampoline* KON_ApplyCompositeLambda(KonState* kstate, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* cont);
