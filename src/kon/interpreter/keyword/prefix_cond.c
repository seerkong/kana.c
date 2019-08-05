#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

bool IsElseTag(KN predicate)
{
    if (KON_IS_WORD(predicate)
        && strcmp(KON_UNBOX_SYMBOL(predicate), "else") == 0
    ) {
        return true;
    }
    else {
        return false;
    }
}

KN AfterCondClauseEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KN env = contBeingInvoked->Env;
    KxHashTable* memo = contBeingInvoked->MemoTable;
    KN restPairs = KxHashTable_AtKey(memo, "RestPairs");
    KN ifTrueAction = KxHashTable_AtKey(memo, "IfTrue");

    KonTrampoline* bounce;
    if (kon_is_true(evaledValue)) {
        bounce = KON_EvalExpression(kstate, ifTrueAction, env, contBeingInvoked->Cont);
    }
    else if (restPairs == KON_NIL) {
        bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
        bounce->Run.Cont = contBeingInvoked->Cont;
        bounce->Run.Value = KON_FALSE;
    }
    else {
        KN condPair = KON_CAR(restPairs);
        KN predicate = KON_CAR(condPair);
        KN action = KON_CADR(condPair);

        if (IsElseTag(predicate)) {
            // do else action
            bounce = KON_EvalExpression(kstate, action, env, contBeingInvoked->Cont);
        }
        else {
            // next condition
            KonContinuation* k = AllocContinuationWithType(KON_CONT_NATIVE_CALLBACK);
            k->Cont = contBeingInvoked->Cont;
            k->Env = env;

            KxHashTable* memo = KxHashTable_Init(4);
            KxHashTable_PutKv(memo, "RestPairs", KON_CDR(restPairs));
            KxHashTable_PutKv(memo, "IfTrue", action);

            k->MemoTable = memo;
            k->NativeCallback = AfterCondClauseEvaled;

            bounce = KON_EvalExpression(kstate, predicate, env, k);
        }
    }

    return bounce;
}



KonTrampoline* KON_EvalPrefixCond(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro cond");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    
    

    KN condPair = KON_CAR(expression);
    KN predicate = KON_CAR(condPair);
    KN action = KON_CADR(condPair);

    KonTrampoline* bounce;
    if (IsElseTag(predicate)) {
        // do else action
        bounce = KON_EvalExpression(kstate, action, env, cont);
    }
    else {
        KonContinuation* k = AllocContinuationWithType(KON_CONT_NATIVE_CALLBACK);
        k->Cont = cont;
        k->Env = env;

        KxHashTable* memo = KxHashTable_Init(4);
        KxHashTable_PutKv(memo, "RestPairs", KON_CDR(expression));
        KxHashTable_PutKv(memo, "IfTrue", action);

        k->MemoTable = memo;
        k->NativeCallback = AfterCondClauseEvaled;

        bounce = KON_EvalExpression(kstate, predicate, env, k);
    }

    return bounce;
}
