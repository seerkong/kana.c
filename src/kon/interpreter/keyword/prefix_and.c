#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KN AfterAndConditionEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonContinuation* cont = contBeingInvoked;//CAST_Kon(Continuation, contBeingInvoked);
    KN env = cont->Env;
    KonHashMap* memo = cont->MemoTable;
    KN restConditon = KON_HashMapGet(memo, "RestCondition");

    KonTrampoline* bounce;
    if (kon_is_false(evaledValue) || evaledValue == KON_NIL) {
        kon_debug("break and");
        bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
        bounce->Run.Cont = cont->Cont;
        bounce->Run.Value = KON_FALSE;
    }
    else if (restConditon == KON_NIL) {
        // all conditions passed, return true
        kon_debug("all and condition return true");
        bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
        bounce->Run.Cont = cont->Cont;
        bounce->Run.Value = KON_TRUE;
    }
    else {
        // next condition
        KN nextExpr = kon_car(restConditon);
        KonContinuation* k = AllocContinuationWithType(KON_CONT_NATIVE_CALLBACK);
        k->Cont = cont->Cont;
        k->Env = env;

        KonHashMap* memo = KON_HashMapInit(10);
        KON_HashMapPut(memo, "RestCondition", kon_cdr(restConditon));
        k->MemoTable = memo;
        k->NativeCallback = AfterAndConditionEvaled;

        bounce = KON_EvalExpression(kstate, nextExpr, env, k);
        return bounce;
    }

    return bounce;
}

KonTrampoline* KON_EvalPrefixAnd(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    kon_debug("meet prefix marcro and");
    kon_debug("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    
    KonContinuation* k = AllocContinuationWithType(KON_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = cont->Env;

    KonHashMap* memo = KON_HashMapInit(10);
    KON_HashMapPut(memo, "RestCondition", kon_cdr(expression));
    k->MemoTable = memo;
    k->NativeCallback = AfterAndConditionEvaled;
    
    KonTrampoline* bounce;
    bounce = KON_EvalExpression(kstate, kon_car(expression), env, k);

    return bounce;
}
