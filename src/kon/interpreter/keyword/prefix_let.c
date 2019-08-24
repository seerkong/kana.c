#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"


KonTrampoline* AfterLetValExprEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonEnv* env = contBeingInvoked->Env;
    KxHashTable* memo = contBeingInvoked->Native.MemoTable;
    char* varName = KxHashTable_AtKey(memo, "VarName");

    KN_EnvDefine(kstate, env, varName, evaledValue);

    KonTrampoline* bounce;

    bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
    bounce->Cont = contBeingInvoked->Cont;
    bounce->Run.Value = KN_TRUE;

    return bounce;
}

KonTrampoline* KN_EvalPrefixLet(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro let");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));
    KN varName = KN_DCR(expression);
    

    const char* varNameCstr = KN_UNBOX_SYMBOL(varName);

    KN_DEBUG("varName %s", varNameCstr);
    
    KonTrampoline* bounce;
    if ((KN)KN_DNR(expression) == KN_NIL) {
        KN_EnvDefine(kstate, env, varNameCstr, KN_UKN);
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->Run.Value = KN_TRUE;
        bounce->Cont = cont;
    }
    else {
        KN initVal = KN_DCNR(expression);
        KN_DEBUG("initVal %s", KN_StringToCstr(KN_ToFormatString(kstate, initVal, true, 0, "  ")));

        KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
        k->Cont = cont;
        k->Env = env;

        KxHashTable* memo = KxHashTable_Init(4);
        
        KxHashTable_PutKv(memo, "VarName", varNameCstr);
        k->Native.MemoTable = memo;
        k->Native.Callback = AfterLetValExprEvaled;

        bounce = KN_EvalExpression(kstate, initVal, env, k);
    }

    return bounce;
}
