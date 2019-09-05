#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KN AfterEvalExprBodyEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    // evaledValue is a data
    if (KN_IS_QUOTE_LIST(evaledValue)) {
        evaledValue = KN_UNBOX_QUOTE(evaledValue);
    }

    KN_DEBUG("evaledValue %s", KN_StringToCstr(KN_ToFormatString(kstate, evaledValue, true, 0, "  ")));

    KonTrampoline* bounce = KN_EvalExpression(
        kstate, evaledValue,
        contBeingInvoked->env, 
        contBeingInvoked->cont);

    return bounce;
}


KonTrampoline* KN_EvalPrefixEval(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro eval");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));

    KN expr = KN_DCNR(expression);
    KN evalEnv = env;

    KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    k->cont = cont;
    k->env = evalEnv;

    k->native.callback = AfterEvalExprBodyEvaled;

    KonTrampoline* bounce;
    bounce = KN_EvalExpression(kstate, expr, env, k);


    return bounce;
}
