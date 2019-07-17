#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KN SplitIfClauses(KonState* kstate, KN sentenceRestWords)
{
    tb_vector_ref_t falseClauseVec = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());
    
    tb_vector_ref_t trueClauseVec = tb_vector_init(TB_VECTOR_GROW_SIZE, tb_element_long());

    KonListNode* iter = sentenceRestWords;
    
    int state = 1; // 1 parse true exprs, 2 parse false exprs
    do {
        KN item = kon_car(iter);
        
        if (state == 1) {
            if (KON_IS_IDENTIFER(item) && strcmp(tb_string_cstr(&KON_UNBOX_SYMBOL(item)), "else") == 0) {
                state = 2;
            }
            else {
                tb_vector_insert_tail(trueClauseVec, item);
            }
        }
        else {
            // else branch
            tb_vector_insert_tail(falseClauseVec, item);
        }

        iter = kon_cdr(iter);
    } while (iter != KON_NIL);
    
    KN trueClause = TbVectorToKonList(kstate, trueClauseVec);
    KN falseClause = TbVectorToKonList(kstate, falseClauseVec);
    
    return KON_List2(kstate, trueClause, falseClause);
}

KN AfterIfConditionEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KN env = contBeingInvoked->Env;
    KonHashMap* memo = contBeingInvoked->MemoTable;
    KN trueClause = KON_HashMapGet(memo, "TrueClause");
    KN falseClause = KON_HashMapGet(memo, "FalseClause");

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

    KonHashMap* memo = KON_HashMapInit(10);
    KON_HashMapPut(memo, "TrueClause", trueClause);
    KON_HashMapPut(memo, "FalseClause", falseClause);
    k->MemoTable = memo;
    k->NativeCallback = AfterIfConditionEvaled;
    
    KonTrampoline* bounce;
    bounce = KON_EvalExpression(kstate, condition, env, k);

    return bounce;
}
