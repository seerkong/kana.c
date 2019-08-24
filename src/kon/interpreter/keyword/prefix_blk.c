#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* KON_ApplyCompositeBlk(KonState* kstate, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* cont)
{
    KN body = (KN)proc->Composite.Body;
    KON_DEBUG("body %s", KON_StringToCstr(KON_ToFormatString(kstate, body, true, 0, "  ")));

    // KonEnv* procBindEnv = KON_MakeChildEnv(kstate, dynamicEnv);
    KonEnv* procBindEnv = env;
    KonTrampoline* bounce = KON_EvalSentences(kstate, body, procBindEnv, cont);
    return bounce;
}

KonTrampoline* KON_EvalPrefixBlk(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro blk");
    KN body = (KN)KON_DLR(expression);

    KON_DEBUG("body %s", KON_StringToCstr(KON_ToFormatString(kstate, body, true, 0, "  ")));

    KonProcedure* proc = KON_ALLOC_TYPE_TAG(kstate, KonProcedure, KON_T_PROCEDURE);
    proc->Type = KON_COMPOSITE_BLK;
    proc->Composite.LexicalEnv = env;
    proc->Composite.Body = body;

    KonTrampoline* bounce = AllocBounceWithType(kstate, KON_TRAMPOLINE_RUN);
    bounce->Run.Value = (KN)proc;
    bounce->Cont = cont;
    return bounce;
}
