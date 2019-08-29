#include "stdio.h"
#include "stdlib.h"
#include "prefix_lambda.h"
#include "../cps_interpreter.h"

KonTrampoline* KN_EvalPrefixCallcc(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro callcc");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));

    // set current continuation as argument
    KN argList = KN_CONS(kstate, (KN)cont, KN_NIL);
    KN proc = KN_DCR(expression);
    if (KN_IS_REFERENCE(proc)) {
        proc = KN_EnvLookup(kstate, env, KN_SymbolToCstr(proc));
    }
    // TODO assert proc is a procedure
    if (!KN_IS_PROCEDURE(proc)) {
        // throw exception
    }
    KonTrampoline* bounce = KN_ApplyCompositeLambda(kstate, proc, argList, env, cont);

    return bounce;
}
