#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KN SplitIfClauses(KonState* kstate, KN sentenceRestWords)
{
    KxVector* falseClauseVec = KxVector_Init();
    
    KxVector* trueClauseVec = KxVector_Init();

    KonPair* iter = sentenceRestWords;
    
    int state = 1; // 1 parse true exprs, 2 parse false exprs
    do {
        KN item = KN_CAR(iter);
        
        if (state == 1) {
            if (KN_IS_WORD(item) && strcmp(KN_UNBOX_SYMBOL(item), "else") == 0) {
                state = 2;
            }
            else {
                KxVector_Push(trueClauseVec, item);
            }
        }
        else {
            // else branch
            KxVector_Push(falseClauseVec, item);
        }

        iter = KN_CDR(iter);
    } while ((KN)iter != KN_NIL);
    
    KN trueClause = KN_VectorToKonPairList(kstate, trueClauseVec);
    KN falseClause = KN_VectorToKonPairList(kstate, falseClauseVec);
    
    return KN_PairList2(kstate, trueClause, falseClause);
}

KonTrampoline* AfterIfConditionEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonEnv* env = contBeingInvoked->env;
    KxHashTable* memo = contBeingInvoked->native.memoTable;
    KN trueClause = KxHashTable_AtKey(memo, "TrueClause");
    KN falseClause = KxHashTable_AtKey(memo, "FalseClause");

    KonTrampoline* bounce;
    if (KN_IS_TRUE(evaledValue)) {
        bounce = KN_EvalSentences(kstate, trueClause, env, contBeingInvoked->cont);
    }
    else if (KN_IS_FALSE(evaledValue) && falseClause != KN_NIL) {
        bounce = KN_EvalSentences(kstate, falseClause, env, contBeingInvoked->cont);
    }
    else {
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->cont = contBeingInvoked->cont;
        bounce->run.value = KN_FALSE;
    }

    return bounce;
}

KonTrampoline* KN_EvalPrefixIf(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro if");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));
    KN condition = KN_DCR(expression);
    KN trueClause = KN_DLR(expression);
    KN falseClause = KN_NIL;
    if (KN_DNR(expression) != KN_NIL) {
        falseClause = KN_DLNR(expression);
    }

    KN_DEBUG("condition %s", KN_StringToCstr(KN_ToFormatString(kstate, condition, true, 0, "  ")));
    // KN_DEBUG("trueClause %s", KN_StringToCstr(KN_ToFormatString(kstate, trueClause, true, 0, "  ")));
    // KN_DEBUG("falseClause %s", KN_StringToCstr(KN_ToFormatString(kstate, falseClause, true, 0, "  ")));

    KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    k->cont = cont;
    k->env = env;

    KxHashTable* memo = KxHashTable_Init(4);
    KxHashTable_PutKv(memo, "TrueClause", trueClause);
    KxHashTable_PutKv(memo, "FalseClause", falseClause);
    k->native.memoTable = memo;
    k->native.callback = AfterIfConditionEvaled;
    
    KonTrampoline* bounce;
    bounce = KN_EvalExpression(kstate, condition, env, k);

    return bounce;
}
