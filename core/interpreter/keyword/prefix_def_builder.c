#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KN AfterBuilderDispatcherIdEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    unsigned int dispatcherId = KN_UNBOX_FIXNUM(evaledValue);
    KonEnv* env = contBeingInvoked->Env;
    KxHashTable* memo = contBeingInvoked->Native.MemoTable;
    KonCell* sourceCode = (KonCell*)KxHashTable_AtKey(memo, "BuilderSourceCode");
    KN className = (KonCell*)KxHashTable_AtKey(memo, "BuilderName");
    const char* classNameCstr = KN_UNBOX_SYMBOL(className);
    KN_DEBUG("dispatcherId %d classNameCstr %s", dispatcherId, classNameCstr);
    ((KonBase*)sourceCode)->MsgDispatcherId = dispatcherId;

    KonTrampoline* bounce;

    KN_EnvDefine(kstate, env, classNameCstr, sourceCode);
    bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);

    bounce->Run.Value = sourceCode;
    bounce->Cont = contBeingInvoked->Cont;

    return bounce;
}


KonTrampoline* KN_EvalPrefixDefBuilder(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro def builder");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));
    KN builderName = KN_DCR(expression);
    KN dispatcherIdExpr = KN_DCNR(expression);

    KN config = KN_DCNNR(expression);


    KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = env;

    KxHashTable* memo = KxHashTable_Init(2);
    KxHashTable_PutKv(memo, "BuilderSourceCode", config);
    KxHashTable_PutKv(memo, "BuilderName", builderName);
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterBuilderDispatcherIdEvaled;
    
    KonTrampoline* bounce;
    bounce = KN_EvalExpression(kstate, dispatcherIdExpr, env, k);

    return bounce;
}
