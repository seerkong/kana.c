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
        KN item = KON_CAR(iter);
        
        if (state == 1) {
            if (KON_IS_WORD(item) && strcmp(KON_UNBOX_SYMBOL(item), "else") == 0) {
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

        iter = KON_CDR(iter);
    } while (iter != KON_NIL);
    
    KN trueClause = KON_VectorToKonPairList(kstate, trueClauseVec);
    KN falseClause = KON_VectorToKonPairList(kstate, falseClauseVec);
    
    return KON_PairList2(kstate, trueClause, falseClause);
}

KonTrampoline* AfterIfConditionEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KN env = contBeingInvoked->Env;
    KxHashTable* memo = contBeingInvoked->Native.MemoTable;
    KN trueClause = KxHashTable_AtKey(memo, "TrueClause");
    KN falseClause = KxHashTable_AtKey(memo, "FalseClause");

    KonTrampoline* bounce;
    if (KON_IS_TRUE(evaledValue)) {
        bounce = KON_EvalSentences(kstate, trueClause, env, contBeingInvoked->Cont);
    }
    else if (KON_IS_FALSE(evaledValue) && falseClause != KON_NIL) {
        bounce = KON_EvalSentences(kstate, falseClause, env, contBeingInvoked->Cont);
    }
    else {
        bounce = AllocBounceWithType(kstate, KON_TRAMPOLINE_RUN);
        bounce->Cont = contBeingInvoked->Cont;
        bounce->Run.Value = KON_FALSE;
    }

    return bounce;
}

KonTrampoline* KON_EvalPrefixIf(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro if");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    KN condition = KON_DCR(expression);
    KN trueClause = KON_DLR(expression);
    KN falseClause = KON_NIL;
    if (KON_DNR(expression) != KON_NIL) {
        falseClause = KON_DLNR(expression);
    }

    KON_DEBUG("condition %s", KON_StringToCstr(KON_ToFormatString(kstate, condition, true, 0, "  ")));
    // KON_DEBUG("trueClause %s", KON_StringToCstr(KON_ToFormatString(kstate, trueClause, true, 0, "  ")));
    // KON_DEBUG("falseClause %s", KON_StringToCstr(KON_ToFormatString(kstate, falseClause, true, 0, "  ")));

    KonContinuation* k = AllocContinuationWithType(kstate, KON_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = env;

    KxHashTable* memo = KxHashTable_Init(4);
    KxHashTable_PutKv(memo, "TrueClause", trueClause);
    KxHashTable_PutKv(memo, "FalseClause", falseClause);
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterIfConditionEvaled;
    
    KonTrampoline* bounce;
    bounce = KON_EvalExpression(kstate, condition, env, k);

    return bounce;
}
