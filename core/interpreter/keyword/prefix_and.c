#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KN AfterAndConditionEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonEnv* env = contBeingInvoked->env;
    KxHashTable* memo = contBeingInvoked->native.memoTable;
    KN restConditon = KxHashTable_AtKey(memo, "RestCondition");

    KonTrampoline* bounce;
    if (KN_IS_FALSE(evaledValue) || evaledValue == KN_NIL) {
        KN_DEBUG("break and");
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->cont = contBeingInvoked->cont;
        bounce->run.value = KN_FALSE;
    }
    else if (restConditon == KN_NIL) {
        // all conditions passed, return true
        KN_DEBUG("all and condition return true");
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->cont = contBeingInvoked->cont;
        bounce->run.value = KN_TRUE;
    }
    else {
        // next condition
        KN nextExpr = KN_CAR(restConditon);
        KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
        k->cont = contBeingInvoked->cont;
        k->env = env;

        KxHashTable* newMemo = KxHashTable_Init(4);
        KxHashTable_PutKv(newMemo, "RestCondition", KN_CDR(restConditon));
        k->native.memoTable = newMemo;
        k->native.callback = AfterAndConditionEvaled;

        bounce = KN_EvalExpression(kstate, nextExpr, env, k);
        return bounce;
    }

    return bounce;
}

KonTrampoline* KN_EvalPrefixAnd(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro and");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));
    KN arguments = KN_CellCoresToList(kstate, expression);
    KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    k->cont = cont;
    k->env = env;

    KxHashTable* memo = KxHashTable_Init(4);
    KxHashTable_PutKv(memo, "RestCondition", KN_CDR(arguments));
    k->native.memoTable = memo;
    k->native.callback = AfterAndConditionEvaled;
    
    KonTrampoline* bounce;
    bounce = KN_EvalExpression(kstate, KN_CAR(arguments), env, k);

    return bounce;
}
