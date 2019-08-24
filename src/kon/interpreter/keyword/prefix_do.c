#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* KN_EvalPrefixDo(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro do");
    KonEnv* procBindEnv = KN_MakeChildEnv(kstate, (KonEnv*)env);
    KN body = KN_DLR(expression);
    KonTrampoline* bounce = KN_EvalSentences(kstate, body, procBindEnv, cont);

    return bounce;
}
