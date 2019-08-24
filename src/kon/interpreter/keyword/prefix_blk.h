
#include "../../prefix.h"
#include "../types.h"

KonTrampoline* KON_EvalPrefixBlk(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont);
KonTrampoline* KON_ApplyCompositeBlk(KonState* kstate, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* cont);
