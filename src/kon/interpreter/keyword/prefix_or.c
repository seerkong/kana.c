#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* AfterOrConditionEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonEnv* env = contBeingInvoked->Env;
    KxHashTable* memo = KxHashTable_ShadowClone(contBeingInvoked->Native.MemoTable);
    KN restConditon = KxHashTable_AtKey(memo, "RestCondition");

    KonTrampoline* bounce;
    if (KN_IS_TRUE(evaledValue)) {
        KN_DEBUG("break or");
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->Cont = contBeingInvoked->Cont;
        bounce->Run.Value = KN_TRUE;
    }
    else if (restConditon == KN_NIL) {
        KN_DEBUG("all or condition fail, return false");
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->Cont = contBeingInvoked->Cont;
        bounce->Run.Value = KN_FALSE;
    }
    else {
        // next condition
        KN nextExpr = KN_CAR(restConditon);
        KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
        k->Cont = contBeingInvoked->Cont;
        k->Env = env;

        KxHashTable* memo = KxHashTable_Init(4);
        KxHashTable_PutKv(memo, "RestCondition", KN_CDR(restConditon));
        k->Native.MemoTable = memo;
        k->Native.Callback = AfterOrConditionEvaled;

        bounce = KN_EvalExpression(kstate, nextExpr, env, k);
        return bounce;
    }

    return bounce;
}

KonTrampoline* KN_EvalPrefixOr(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro or");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));
    KN arguments = KN_CellCoresToList(kstate, expression);

    KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = env;

    KxHashTable* memo = KxHashTable_Init(4);
    KxHashTable_PutKv(memo, "RestCondition", KN_CDR(arguments));
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterOrConditionEvaled;
    
    KonTrampoline* bounce;
    bounce = KN_EvalExpression(kstate, KN_CAR(arguments), env, k);

    return bounce;
}