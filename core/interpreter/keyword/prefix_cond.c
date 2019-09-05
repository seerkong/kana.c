#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

bool IsElseTag(KN predicate)
{
    if (KN_IS_WORD(predicate)
        && strcmp(KN_UNBOX_SYMBOL(predicate), "else") == 0
    ) {
        return true;
    }
    else {
        return false;
    }
}

KN AfterCondClauseEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonEnv* env = contBeingInvoked->env;
    KxHashTable* memo = KxHashTable_ShadowClone(contBeingInvoked->native.memoTable);
    KN restPairs = KxHashTable_AtKey(memo, "RestPairs");
    KN ifTrueAction = KxHashTable_AtKey(memo, "IfTrue");

    KonTrampoline* bounce;
    if (KN_IS_TRUE(evaledValue)) {
        bounce = KN_EvalSentences(kstate, ifTrueAction, env, contBeingInvoked->cont);
    }
    else if (restPairs == KN_NIL) {
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->cont = contBeingInvoked->cont;
        bounce->run.value = KN_FALSE;
    }
    else {
        KN predicate = KN_DCR(restPairs);
        KN action = KN_DLR(restPairs);

        if (IsElseTag(predicate)) {
            // do else action
            bounce = KN_EvalSentences(kstate, action, env, contBeingInvoked->cont);
        }
        else {
            // next condition
            KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
            k->cont = contBeingInvoked->cont;
            k->env = env;

            KxHashTable* memo = KxHashTable_Init(4);
            KxHashTable_PutKv(memo, "RestPairs", KN_DNR(restPairs));
            KxHashTable_PutKv(memo, "IfTrue", action);

            k->native.memoTable = memo;
            k->native.callback = AfterCondClauseEvaled;

            bounce = KN_EvalExpression(kstate, predicate, env, k);
        }
    }

    return bounce;
}



KonTrampoline* KN_EvalPrefixCond(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro cond");
    // KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));
    
    KN predicate = KN_DCR(expression);
    KN action = KN_DLR(expression);

    KonTrampoline* bounce;
    if (IsElseTag(predicate)) {
        // do else action
        bounce = KN_EvalSentences(kstate, action, env, cont);
    }
    else {
        KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
        k->cont = cont;
        k->env = env;

        KxHashTable* memo = KxHashTable_Init(4);
        KxHashTable_PutKv(memo, "RestPairs", KN_DNR(expression));
        KxHashTable_PutKv(memo, "IfTrue", action);

        k->native.memoTable = memo;
        k->native.callback = AfterCondClauseEvaled;

        bounce = KN_EvalExpression(kstate, predicate, env, k);
    }

    return bounce;
}
