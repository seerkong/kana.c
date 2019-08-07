#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KN AfterForPrediction(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked);
KN AfterForBodyEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked);

KN BeforeForPrediction(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KN env = contBeingInvoked->Env;
    
    KxHashTable* memo = contBeingInvoked->Native.MemoTable;
    KN predictExpr = KxHashTable_AtKey(memo, "PredictExpr");

    KonContinuation* k = AllocContinuationWithType(KON_CONT_NATIVE_CALLBACK);
    k->Cont = contBeingInvoked->Cont;
    k->Env = env;
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterForPrediction;

    KonTrampoline* bounce = KON_EvalExpression(kstate, predictExpr, env, k);

    return bounce;
}

KN AfterForPrediction(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KN env = contBeingInvoked->Env;
    KxHashTable* memo = contBeingInvoked->Native.MemoTable;
    KN bodyExprs = KxHashTable_AtKey(memo, "BodyExprs");

    KonContinuation* k = AllocContinuationWithType(KON_CONT_NATIVE_CALLBACK);
    k->Cont = contBeingInvoked->Cont;
    k->Env = env;
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterForBodyEvaled;

    KonTrampoline* bounce;
    if (kon_is_true(evaledValue)) {
        bounce = KON_EvalSentences(kstate, bodyExprs, env, k);
    }
    else {
        bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
        bounce->Run.Cont = contBeingInvoked->Cont;
        bounce->Run.Value = KON_TRUE;
    }

    return bounce;
}

KN AfterForBodyEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KN env = contBeingInvoked->Env;
    
    KxHashTable* memo = contBeingInvoked->Native.MemoTable;
    KN afterBodyExpr = KxHashTable_AtKey(memo, "AfterExpr");

    KonContinuation* k = AllocContinuationWithType(KON_CONT_NATIVE_CALLBACK);
    k->Cont = contBeingInvoked->Cont;
    k->Env = env;
    k->Native.MemoTable = memo;
    k->Native.Callback = BeforeForPrediction;

    KonTrampoline* bounce = KON_EvalExpression(kstate, afterBodyExpr, env, k);

    return bounce;
}

KonTrampoline* KON_EvalPrefixFor(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro for");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));

    KonEnv* loopBindEnv = KON_MakeChildEnv(kstate, env);
    KON_EnvDefine(kstate, loopBindEnv, "break", cont);

    KN initExpr = KON_CAR(expression);
    KN predictExpr = KON_CADR(expression);
    KN afterExpr = KON_CADDR(expression);
    KN bodyExprs = KON_CDDDR(expression);

    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));

    KonContinuation* k = AllocContinuationWithType(KON_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = loopBindEnv;

    KxHashTable* memo = KxHashTable_Init(4);
    KxHashTable_PutKv(memo, "PredictExpr", predictExpr);
    KxHashTable_PutKv(memo, "AfterExpr", afterExpr);
    KxHashTable_PutKv(memo, "BodyExprs", bodyExprs);
    k->Native.MemoTable = memo;
    k->Native.Callback = BeforeForPrediction;
    
    KonTrampoline* bounce;
    bounce = KON_EvalExpression(kstate, initExpr, loopBindEnv, k);


    return bounce;
}
