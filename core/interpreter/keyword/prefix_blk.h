
#include "../../prefix.h"
#include "../types.h"

KonTrampoline* KN_EvalPrefixBlk(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont);
KonTrampoline* KN_ApplyCompositeBlk(KonState* kstate, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* cont);
