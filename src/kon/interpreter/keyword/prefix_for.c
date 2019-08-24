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

    KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    k->Cont = contBeingInvoked->Cont;
    k->Env = env;
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterForPrediction;

    KonTrampoline* bounce = KN_EvalExpression(kstate, predictExpr, env, k);

    return bounce;
}

KonTrampoline* AfterForPrediction(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonEnv* env = contBeingInvoked->Env;
    KxHashTable* memo = KxHashTable_ShadowClone(contBeingInvoked->Native.MemoTable);
    KN bodyExprs = KxHashTable_AtKey(memo, "BodyExprs");

    KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    k->Cont = contBeingInvoked->Cont;
    k->Env = env;
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterForBodyEvaled;

    KonTrampoline* bounce;
    if (KN_IS_TRUE(evaledValue)) {
        bounce = KN_EvalSentences(kstate, bodyExprs, env, k);
    }
    else {
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->Cont = contBeingInvoked->Cont;
        bounce->Run.Value = KN_TRUE;
    }

    return bounce;
}

KonTrampoline* AfterForBodyEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonEnv* env = contBeingInvoked->Env;
    KxHashTable* memo = KxHashTable_ShadowClone(contBeingInvoked->Native.MemoTable);
    KN afterBodyExpr = KxHashTable_AtKey(memo, "AfterExpr");

    KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    k->Cont = contBeingInvoked->Cont;
    k->Env = env;
    k->Native.MemoTable = memo;
    k->Native.Callback = BeforeForPrediction;

    KonTrampoline* bounce = KN_EvalExpression(kstate, afterBodyExpr, env, k);

    return bounce;
}

KonTrampoline* KN_EvalPrefixFor(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro for");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));

    KonEnv* loopBindEnv = KN_MakeChildEnv(kstate, env);
    KN paramTable = KN_DTR(expression);
    KxHashTable* paramTableInner = KN_UNBOX_TABLE(paramTable);

    KN initExpr = KxHashTable_ValAtIndex(paramTableInner, 0);
    KN predictExpr = KxHashTable_ValAtIndex(paramTableInner, 1);
    KN afterExpr = KxHashTable_ValAtIndex(paramTableInner, 2);
    KN bodyExprs = KN_DLR(expression);

    KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = loopBindEnv;

    KxHashTable* memo = KxHashTable_Init(4);
    KxHashTable_PutKv(memo, "PredictExpr", predictExpr);
    KxHashTable_PutKv(memo, "AfterExpr", afterExpr);
    KxHashTable_PutKv(memo, "BodyExprs", bodyExprs);
    k->Native.MemoTable = memo;
    k->Native.Callback = BeforeForPrediction;

    // set break keyword continuation
    KN_EnvDefine(kstate, loopBindEnv, "break", cont);
    // set continue keyword continuation
    KonContinuation* continueKeywordCont = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    continueKeywordCont->Cont = cont;
    continueKeywordCont->Env = loopBindEnv;
    continueKeywordCont->Native.MemoTable = KxHashTable_ShadowClone(memo);
    continueKeywordCont->Native.Callback = AfterForBodyEvaled;
    KN_EnvDefine(kstate, loopBindEnv, "continue", continueKeywordCont);

    
    KonTrampoline* bounce;
    bounce = KN_EvalExpression(kstate, initExpr, loopBindEnv, k);


    return bounce;
}
