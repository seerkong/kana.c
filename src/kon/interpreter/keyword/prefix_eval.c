#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KN AfterEvalExprBodyEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    // evaledValue is a data
    if (KON_IS_QUOTE_LIST(evaledValue)) {
        evaledValue = KON_UNBOX_QUOTE(evaledValue);
    }

    KON_DEBUG("evaledValue %s", KON_StringToCstr(KON_ToFormatString(kstate, evaledValue, true, 0, "  ")));

    KonTrampoline* bounce = KON_EvalExpression(
        kstate, evaledValue,
        contBeingInvoked->Env, 
        contBeingInvoked->Cont);

    return bounce;
}


KonTrampoline* KON_EvalPrefixEval(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro eval");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));

    KN expr = KON_DCNR(expression);
    KN evalEnv = env;

    KonContinuation* k = AllocContinuationWithType(kstate, KON_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = evalEnv;

    k->Native.Callback = AfterEvalExprBodyEvaled;

    KonTrampoline* bounce;
    bounce = KON_EvalExpression(kstate, expr, env, k);


    return bounce;
}
