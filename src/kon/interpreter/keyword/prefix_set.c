#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"


KN AfterSetValExprEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KN env = contBeingInvoked->Env;
    KonHashMap* memo = contBeingInvoked->MemoTable;
    char* varName = KON_HashMapGet(memo, "VarName");

    KON_EnvLookupSet(kstate, env, varName, evaledValue);

    KonTrampoline* bounce;

    bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
    bounce->Run.Cont = contBeingInvoked->Cont;
    bounce->Run.Value = KON_TWO;

    return bounce;
}

KonTrampoline* KON_EvalPrefixSet(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    kon_debug("meet prefix marcro set");
    kon_debug("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    KN varName = kon_car(expression);
    

    char* varNameCstr = tb_string_cstr(&KON_UNBOX_SYMBOL(varName));

    kon_debug("varName %s", varNameCstr);
    
    

    KonTrampoline* bounce;
    if ((KN)kon_cdr(expression) == KON_NIL) {
        KON_EnvLookupSet(kstate, env, varNameCstr, KON_NULL);

        bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
        bounce->Run.Value = KON_ONE;
        bounce->Run.Cont = cont;
    }
    else {
        KN initVal = kon_cadr(expression);
        kon_debug("initVal %s", KON_StringToCstr(KON_ToFormatString(kstate, initVal, true, 0, "  ")));


        KonContinuation* k = AllocContinuationWithType(KON_CONT_NATIVE_CALLBACK);
        k->Cont = cont;
        k->Env = env;

        KonHashMap* memo = KON_HashMapInit(10);
        
        KON_HashMapPut(memo, "VarName", varNameCstr);
        k->MemoTable = memo;
        k->NativeCallback = AfterSetValExprEvaled;

        bounce = KON_EvalExpression(kstate, initVal, env, k);
    }

    return bounce;
}
