#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"
#include "../../container/kx_hashtable.h"

KonTrampoline* AfterForPrediction(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked);
KonTrampoline* AfterForBodyEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked);

KonTrampoline* BeforeForPrediction(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonEnv* env = contBeingInvoked->Env;
    KxHashTable* memo = KxHashTable_ShadowClone(contBeingInvoked->Native.MemoTable);
    KN predictExpr = KxHashTable_AtKey(memo, "PredictExpr");

    KonContinuation* k = AllocContinuationWithType(kstate, KON_CONT_NATIVE_CALLBACK);
    k->Cont = contBeingInvoked->Cont;
    k->Env = env;
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterForPrediction;

    KonTrampoline* bounce = KON_EvalExpression(kstate, predictExpr, env, k);

    return bounce;
}

KonTrampoline* AfterForPrediction(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonEnv* env = contBeingInvoked->Env;
    KxHashTable* memo = KxHashTable_ShadowClone(contBeingInvoked->Native.MemoTable);
    KN bodyExprs = KxHashTable_AtKey(memo, "BodyExprs");

    KonContinuation* k = AllocContinuationWithType(kstate, KON_CONT_NATIVE_CALLBACK);
    k->Cont = contBeingInvoked->Cont;
    k->Env = env;
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterForBodyEvaled;

    KonTrampoline* bounce;
    if (KON_IS_TRUE(evaledValue)) {
        bounce = KON_EvalSentences(kstate, bodyExprs, env, k);
    }
    else {
        bounce = AllocBounceWithType(kstate, KON_TRAMPOLINE_RUN);
        bounce->Cont = contBeingInvoked->Cont;
        bounce->Run.Value = KON_TRUE;
    }

    return bounce;
}

KonTrampoline* AfterForBodyEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonEnv* env = contBeingInvoked->Env;
    KxHashTable* memo = KxHashTable_ShadowClone(contBeingInvoked->Native.MemoTable);
    KN afterBodyExpr = KxHashTable_AtKey(memo, "AfterExpr");

    KonContinuation* k = AllocContinuationWithType(kstate, KON_CONT_NATIVE_CALLBACK);
    k->Cont = contBeingInvoked->Cont;
    k->Env = env;
    k->Native.MemoTable = memo;
    k->Native.Callback = BeforeForPrediction;

    KonTrampoline* bounce = KON_EvalExpression(kstate, afterBodyExpr, env, k);

    return bounce;
}

KonTrampoline* KON_EvalPrefixFor(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro for");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));

    KonEnv* loopBindEnv = KON_MakeChildEnv(kstate, env);
    KN paramTable = KON_DTR(expression);
    KxHashTable* paramTableInner = KON_UNBOX_TABLE(paramTable);

    KN initExpr = KxHashTable_ValAtIndex(paramTableInner, 0);
    KN predictExpr = KxHashTable_ValAtIndex(paramTableInner, 1);
    KN afterExpr = KxHashTable_ValAtIndex(paramTableInner, 2);
    KN bodyExprs = KON_DLR(expression);

    KonContinuation* k = AllocContinuationWithType(kstate, KON_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = loopBindEnv;

    KxHashTable* memo = KxHashTable_Init(4);
    KxHashTable_PutKv(memo, "PredictExpr", predictExpr);
    KxHashTable_PutKv(memo, "AfterExpr", afterExpr);
    KxHashTable_PutKv(memo, "BodyExprs", bodyExprs);
    k->Native.MemoTable = memo;
    k->Native.Callback = BeforeForPrediction;

    // set break keyword continuation
    KON_EnvDefine(kstate, loopBindEnv, "break", cont);
    // set continue keyword continuation
    KonContinuation* continueKeywordCont = AllocContinuationWithType(kstate, KON_CONT_NATIVE_CALLBACK);
    continueKeywordCont->Cont = cont;
    continueKeywordCont->Env = loopBindEnv;
    continueKeywordCont->Native.MemoTable = KxHashTable_ShadowClone(memo);
    continueKeywordCont->Native.Callback = AfterForBodyEvaled;
    KON_EnvDefine(kstate, loopBindEnv, "continue", continueKeywordCont);

    
    KonTrampoline* bounce;
    bounce = KON_EvalExpression(kstate, initExpr, loopBindEnv, k);


    return bounce;
}
