#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* KON_ApplyCompositeBlk(KonState* kstate, KonProcedure* proc, KN argList, KN env, KonContinuation* cont)
{
    KN body = proc->Composite.Body;
    kon_debug("body %s", KON_StringToCstr(KON_ToFormatString(kstate, body, true, 0, "  ")));

    // KonEnv* procBindEnv = KON_MakeChildEnv(kstate, dynamicEnv);
    KonEnv* procBindEnv = env;
    KonTrampoline* bounce = KON_EvalSentences(kstate, body, procBindEnv, cont);
    return bounce;
}

KonTrampoline* KON_EvalPrefixBlk(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    kon_debug("meet prefix marcro blk");
    KN body = expression;

    kon_debug("body %s", KON_StringToCstr(KON_ToFormatString(kstate, body, true, 0, "  ")));

    KonProcedure* proc = KON_ALLOC_TYPE_TAG(kstate, KonProcedure, KON_T_PROCEDURE);
    proc->Type = KON_COMPOSITE_BLK;
    proc->Composite.LexicalEnv = env;
    proc->Composite.Body = body;

    KonTrampoline* bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
    bounce->Run.Value = proc;
    bounce->Run.Cont = cont;
    return bounce;
}
