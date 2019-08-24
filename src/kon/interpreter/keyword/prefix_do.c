#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* KON_EvalPrefixDo(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro do");
    KonEnv* procBindEnv = KON_MakeChildEnv(kstate, (KonEnv*)env);
    KN body = KON_DLR(expression);
    KonTrampoline* bounce = KON_EvalSentences(kstate, body, procBindEnv, cont);

    return bounce;
}
