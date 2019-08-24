#include "stdio.h"
#include "stdlib.h"
#include "prefix_lambda.h"
#include "../cps_interpreter.h"

KonTrampoline* KN_EvalPrefixContinue(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro continue");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));

    KN loopCont = KN_EnvLookup(kstate, env, "continue");
    
    KonTrampoline* bounce;
    if (KN_IS_CONTINUATION(loopCont)) {
        // loop's continuation, just receive 1 argument;
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->Run.Value = KN_TRUE;
        // goto this continuation directly. skip next exprs
        bounce->Cont = loopCont;
        return bounce;
    }
    else {
        // TODO error occurred, throw exception?
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->Run.Value = KN_TRUE;
        bounce->Cont = cont;
        return bounce;
    }
}
