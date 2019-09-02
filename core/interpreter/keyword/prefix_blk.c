#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* KN_EvalPrefixBlk(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro blk");
    KN body = expression;

    KN_DEBUG("body %s", KN_StringToCstr(KN_ToFormatString(kstate, body, true, 0, "  ")));

    KonEnv* procBindEnv = KN_MakeChildEnv(kstate, env);

    KonTrampoline* bounce = KN_EvalSentences(kstate, body, procBindEnv, cont);
    return bounce;
}
