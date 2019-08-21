#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KN AfterBuilderDispatcherIdEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    unsigned int dispatcherId = KON_UNBOX_FIXNUM(evaledValue);
    KN env = contBeingInvoked->Env;
    KxHashTable* memo = contBeingInvoked->Native.MemoTable;
    KonCell* sourceCode = (KonCell*)KxHashTable_AtKey(memo, "BuilderSourceCode");
    KN className = (KonCell*)KxHashTable_AtKey(memo, "BuilderName");
    const char* classNameCstr = KON_UNBOX_SYMBOL(className);
    KON_DEBUG("dispatcherId %d classNameCstr %s", dispatcherId, classNameCstr);
    ((KonBase*)sourceCode)->MsgDispatcherId = dispatcherId;

    KonTrampoline* bounce;

    KON_EnvDefine(kstate, env, classNameCstr, sourceCode);
    bounce = AllocBounceWithType(kstate, KON_TRAMPOLINE_RUN);

    bounce->Run.Value = sourceCode;
    bounce->Cont = contBeingInvoked->Cont;

    return bounce;
}


KonTrampoline* KON_EvalPrefixDefBuilder(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro def builder");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    KN builderName = KON_DCR(expression);
    KN dispatcherIdExpr = KON_DCNR(expression);

    KN config = KON_DCNNR(expression);


    KonContinuation* k = AllocContinuationWithType(kstate, KON_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = env;

    KxHashTable* memo = KxHashTable_Init(2);
    KxHashTable_PutKv(memo, "BuilderSourceCode", config);
    KxHashTable_PutKv(memo, "BuilderName", builderName);
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterBuilderDispatcherIdEvaled;
    
    KonTrampoline* bounce;
    bounce = KON_EvalExpression(kstate, dispatcherIdExpr, env, k);

    return bounce;
}
