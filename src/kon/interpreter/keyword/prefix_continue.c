#include "stdio.h"
#include "stdlib.h"
#include "prefix_lambda.h"
#include "../cps_interpreter.h"

KonTrampoline* KON_EvalPrefixContinue(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro continue");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));

    KN loopCont = KON_EnvLookup(kstate, env, "continue");
    
    KonTrampoline* bounce;
    if (KON_IS_CONTINUATION(loopCont)) {
        // loop's continuation, just receive 1 argument;
        bounce = AllocBounceWithType(kstate, KON_TRAMPOLINE_RUN);
        bounce->Run.Value = KON_TRUE;
        // goto this continuation directly. skip next exprs
        bounce->Cont = loopCont;
        return bounce;
    }
    else {
        // TODO error occurred, throw exception?
        bounce = AllocBounceWithType(kstate, KON_TRAMPOLINE_RUN);
        bounce->Run.Value = KON_TRUE;
        bounce->Cont = cont;
        return bounce;
    }
}
