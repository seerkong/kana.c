#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* KN_ApplyCompositeBlk(KonState* kstate, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* cont)
{
    KN body = (KN)proc->Composite.Body;
    KN_DEBUG("body %s", KN_StringToCstr(KN_ToFormatString(kstate, body, true, 0, "  ")));

    // KonEnv* procBindEnv = KN_MakeChildEnv(kstate, dynamicEnv);
    KonEnv* procBindEnv = env;
    KonTrampoline* bounce = KN_EvalSentences(kstate, body, procBindEnv, cont);
    return bounce;
}

KonTrampoline* KN_EvalPrefixBlk(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro blk");
    KN body = (KN)KN_DLR(expression);

    KN_DEBUG("body %s", KN_StringToCstr(KN_ToFormatString(kstate, body, true, 0, "  ")));

    KonProcedure* proc = KN_ALLOC_TYPE_TAG(kstate, KonProcedure, KN_T_PROCEDURE);
    proc->Type = KN_COMPOSITE_BLK;
    proc->Composite.LexicalEnv = env;
    proc->Composite.Body = body;

    KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
    bounce->Run.Value = (KN)proc;
    bounce->Cont = cont;
    return bounce;
}
