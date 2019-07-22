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
        KN item = kon_car(iter);
        
        if (state == 1) {
            if (KON_IS_IDENTIFER(item) && strcmp(KON_UNBOX_SYMBOL(item), "else") == 0) {
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

        iter = kon_cdr(iter);
    } while (iter != KON_NIL);
    
    KN trueClause = KON_VectorToKonPairList(kstate, trueClauseVec);
    KN falseClause = KON_VectorToKonPairList(kstate, falseClauseVec);
    
    return KON_List2(kstate, trueClause, falseClause);
}

KN AfterIfConditionEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KN env = contBeingInvoked->Env;
    KxHashTable* memo = contBeingInvoked->MemoTable;
    KN trueClause = KxHashTable_AtKey(memo, "TrueClause");
    KN falseClause = KxHashTable_AtKey(memo, "FalseClause");

    KonTrampoline* bounce;
    if (kon_is_true(evaledValue)) {
        bounce = KON_EvalSentences(kstate, trueClause, env, contBeingInvoked->Cont);
    }
    else if (kon_is_false(evaledValue) && falseClause != KON_NIL) {
        bounce = KON_EvalSentences(kstate, falseClause, env, contBeingInvoked->Cont);
    }
    else {
        bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
        bounce->Run.Cont = contBeingInvoked->Cont;
        bounce->Run.Value = KON_FALSE;
    }

    return bounce;
}

KonTrampoline* KON_EvalPrefixIf(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro if");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    KN condition = kon_car(expression);
    KN parsed = SplitIfClauses(kstate, kon_cdr(expression));
    KN trueClause = kon_car(parsed);
    KN falseClause = kon_cadr(parsed);

    KON_DEBUG("condition %s", KON_StringToCstr(KON_ToFormatString(kstate, condition, true, 0, "  ")));
    KON_DEBUG("trueClause %s", KON_StringToCstr(KON_ToFormatString(kstate, trueClause, true, 0, "  ")));
    KON_DEBUG("falseClause %s", KON_StringToCstr(KON_ToFormatString(kstate, falseClause, true, 0, "  ")));

    KonContinuation* k = AllocContinuationWithType(KON_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = cont->Env;

    KxHashTable* memo = KxHashTable_Init(8);
    KxHashTable_PutKv(memo, "TrueClause", trueClause);
    KxHashTable_PutKv(memo, "FalseClause", falseClause);
    k->MemoTable = memo;
    k->NativeCallback = AfterIfConditionEvaled;
    
    KonTrampoline* bounce;
    bounce = KON_EvalExpression(kstate, condition, env, k);

    return bounce;
}
