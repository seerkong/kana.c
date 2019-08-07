#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"


KN AfterLetValExprEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KN env = contBeingInvoked->Env;
    KxHashTable* memo = contBeingInvoked->Native.MemoTable;
    char* varName = KxHashTable_AtKey(memo, "VarName");

    KON_EnvDefine(kstate, env, varName, evaledValue);

    KonTrampoline* bounce;

    bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
    bounce->Run.Cont = contBeingInvoked->Cont;
    bounce->Run.Value = KON_TWO;

    return bounce;
}

KonTrampoline* KON_EvalPrefixLet(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro let");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    KN varName = KON_CAR(expression);
    

    const char* varNameCstr = KON_UNBOX_SYMBOL(varName);

    KON_DEBUG("varName %s", varNameCstr);
    
    KonTrampoline* bounce;
    if ((KN)KON_CDR(expression) == KON_NIL) {
        KON_EnvDefine(kstate, env, varNameCstr, KON_NULL);
        bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
        bounce->Run.Value = KON_TRUE;
        bounce->Run.Cont = cont;
    }
    else {
        KN initVal = KON_CADR(expression);
        KON_DEBUG("initVal %s", KON_StringToCstr(KON_ToFormatString(kstate, initVal, true, 0, "  ")));

        KonContinuation* k = AllocContinuationWithType(KON_CONT_NATIVE_CALLBACK);
        k->Cont = cont;
        k->Env = env;

        KxHashTable* memo = KxHashTable_Init(4);
        
        KxHashTable_PutKv(memo, "VarName", varNameCstr);
        k->Native.MemoTable = memo;
        k->Native.Callback = AfterLetValExprEvaled;

        bounce = KON_EvalExpression(kstate, initVal, env, k);
    }

    return bounce;
}
