#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* AfterOrConditionEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KN env = contBeingInvoked->Env;
    KxHashTable* memo = KxHashTable_ShadowClone(contBeingInvoked->Native.MemoTable);
    KN restConditon = KxHashTable_AtKey(memo, "RestCondition");

    KonTrampoline* bounce;
    if (KON_IS_TRUE(evaledValue)) {
        KON_DEBUG("break or");
        bounce = AllocBounceWithType(kstate, KON_TRAMPOLINE_RUN);
        bounce->Cont = contBeingInvoked->Cont;
        bounce->Run.Value = KON_TRUE;
    }
    else if (restConditon == KON_NIL) {
        KON_DEBUG("all or condition fail, return false");
        bounce = AllocBounceWithType(kstate, KON_TRAMPOLINE_RUN);
        bounce->Cont = contBeingInvoked->Cont;
        bounce->Run.Value = KON_FALSE;
    }
    else {
        // next condition
        KN nextExpr = KON_CAR(restConditon);
        KonContinuation* k = AllocContinuationWithType(kstate, KON_CONT_NATIVE_CALLBACK);
        k->Cont = contBeingInvoked->Cont;
        k->Env = env;

        KxHashTable* memo = KxHashTable_Init(4);
        KxHashTable_PutKv(memo, "RestCondition", KON_CDR(restConditon));
        k->Native.MemoTable = memo;
        k->Native.Callback = AfterOrConditionEvaled;

        bounce = KON_EvalExpression(kstate, nextExpr, env, k);
        return bounce;
    }

    return bounce;
}

KonTrampoline* KON_EvalPrefixOr(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro or");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    
    KonContinuation* k = AllocContinuationWithType(kstate, KON_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = env;

    KxHashTable* memo = KxHashTable_Init(4);
    KxHashTable_PutKv(memo, "RestCondition", KON_CDR(expression));
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterOrConditionEvaled;
    
    KonTrampoline* bounce;
    bounce = KON_EvalExpression(kstate, KON_CAR(expression), env, k);

    return bounce;
}