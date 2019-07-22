#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KN AfterOrConditionEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KN env = contBeingInvoked->Env;
    KonHashTable* memo = contBeingInvoked->MemoTable;
    KN restConditon = KonHashTable_AtKey(memo, "RestCondition");

    KonTrampoline* bounce;
    if (kon_is_true(evaledValue)) {
        KON_DEBUG("break or");
        bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
        bounce->Run.Cont = contBeingInvoked->Cont;
        bounce->Run.Value = KON_TRUE;
    }
    else if (restConditon == KON_NIL) {
        // all conditions passed, return true
        KON_DEBUG("all or condition return true");
        bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
        bounce->Run.Cont = contBeingInvoked->Cont;
        bounce->Run.Value = KON_FALSE;
    }
    else {
        // next condition
        KN nextExpr = kon_car(restConditon);
        KonContinuation* k = AllocContinuationWithType(KON_CONT_NATIVE_CALLBACK);
        k->Cont = contBeingInvoked->Cont;
        k->Env = env;

        KonHashTable* memo = KonHashTable_Init(8);
        KonHashTable_PutKv(memo, "RestCondition", kon_cdr(restConditon));
        k->MemoTable = memo;
        k->NativeCallback = AfterOrConditionEvaled;

        bounce = KON_EvalExpression(kstate, nextExpr, env, k);
        return bounce;
    }

    return bounce;
}

KonTrampoline* KON_EvalPrefixOr(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro or");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    
    KonContinuation* k = AllocContinuationWithType(KON_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = env;

    KonHashTable* memo = KonHashTable_Init(8);
    KonHashTable_PutKv(memo, "RestCondition", kon_cdr(expression));
    k->MemoTable = memo;
    k->NativeCallback = AfterOrConditionEvaled;
    
    KonTrampoline* bounce;
    bounce = KON_EvalExpression(kstate, kon_car(expression), env, k);

    return bounce;
}