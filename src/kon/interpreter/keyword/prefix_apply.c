#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"
#include "prefix_func.h"
#include "prefix_lambda.h"
#include "prefix_blk.h"
extern KN UnBoxAccessorValue(KN konValue);

KonTrampoline* ApplyProcArguments(KonState* kstate, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* cont)
{
    KonTrampoline* bounce;
    // TODO assert subj is a procedure
    if (proc->Type == KN_NATIVE_FUNC) {
        KonNativeFuncRef funcRef = proc->NativeFuncRef;
        KN applyResult = (*funcRef)(kstate, argList);
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->Run.Value = applyResult;
        bounce->Cont = cont;
    }
    else if (proc->Type == KN_NATIVE_OBJ_METHOD) {
        // treat as plain procedure when apply arg list
        // the first item in arg list is the object
        KonNativeFuncRef funcRef = proc->NativeFuncRef;
        KN applyResult = (*funcRef)(kstate, argList);
        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->Run.Value = applyResult;
        bounce->Cont = cont;
    }
    else if (proc->Type == KN_COMPOSITE_LAMBDA) {
        bounce = KN_ApplyCompositeLambda(kstate, proc, argList, env, cont);
    }
    else if (proc->Type == KN_COMPOSITE_FUNC) {
        bounce = KN_ApplyCompositeFunc(kstate, proc, argList, env, cont);
    }
    else if (proc->Type == KN_COMPOSITE_BLK) {
        bounce = KN_ApplyCompositeBlk(kstate, proc, KN_NIL, env, cont);
    }
    else if (proc->Type == KN_COMPOSITE_OBJ_METHOD) {
        // treat as plain procedure when apply arg list
        // the first item in arg list is the object
        bounce = KN_ApplyCompositeLambda(kstate, proc, argList, env, cont);
    }
    return bounce;
}

KonTrampoline* AfterApplyArgsExprEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonEnv* env = contBeingInvoked->Env;
    KonContinuation* cont = contBeingInvoked->Cont;
    KxHashTable* memo = contBeingInvoked->Native.MemoTable;
    KN applySym = KxHashTable_AtKey(memo, "ApplySym");

    KN_DEBUG("applySym %s", KN_StringToCstr(KN_ToFormatString(kstate, applySym, true, 0, "  ")));

    KN proc = KN_UNDEF;
    // lookup procedure in env
    if (KN_IS_IDENTIFIER(applySym) || KN_IS_REFERENCE(applySym)) {
        proc = KN_EnvLookup(kstate, env, KN_SymbolToCstr(applySym));
    }
    else if (KN_IS_ACCESSOR(applySym)) {
        proc = UnBoxAccessorValue(applySym);
    }

    // evaledValue is a data
    if (KN_IS_QUOTE_LIST(evaledValue)) {
        evaledValue = KN_UNBOX_QUOTE(evaledValue);
    }

    KN_DEBUG("proc args %s", KN_StringToCstr(KN_ToFormatString(kstate, evaledValue, true, 0, "  ")));
    

    KonTrampoline* bounce = ApplyProcArguments(kstate, proc, evaledValue, env, cont);
    return bounce;
}

KonTrampoline* AfterApplySymExprEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonEnv* env = contBeingInvoked->Env;
    KxHashTable* memo =  KxHashTable_ShadowClone(contBeingInvoked->Native.MemoTable);
    KN applyArgsExpr = KxHashTable_AtKey(memo, "ApplyArgsExpr");

    KonTrampoline* bounce;
    KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    k->Cont = contBeingInvoked->Cont;
    k->Env = env;

    KxHashTable_PutKv(memo, "ApplySym", evaledValue);

    k->Native.MemoTable = memo;
    k->Native.Callback = AfterApplyArgsExprEvaled;

    bounce = KN_EvalExpression(kstate, applyArgsExpr, env, k);

    return bounce;
}


// first arg should be a expr that returns a symbol
// second arg should be a expr that returns a quoted list
// 3rd arg is optional. is the apply env
KonTrampoline* KN_EvalPrefixApply(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro apply");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));
    
    KN applySymExpr = KN_DCNR(expression);
    KN applyArgsExpr = KN_DCNNR(expression);

    KN_DEBUG("applySymExpr %s", KN_StringToCstr(KN_ToFormatString(kstate, applySymExpr, true, 0, "  ")));
    KN_DEBUG("applyArgsExpr %s", KN_StringToCstr(KN_ToFormatString(kstate, applyArgsExpr, true, 0, "  ")));


    KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = env;

    KxHashTable* memo = KxHashTable_Init(4);
    KxHashTable_PutKv(memo, "ApplyArgsExpr", applyArgsExpr);

    k->Native.MemoTable = memo;
    k->Native.Callback = AfterApplySymExprEvaled;
    KN_DEBUG("before KN_EvalExpression");
    KonTrampoline* bounce = KN_EvalExpression(kstate, applySymExpr, env, k);

    return bounce;
}
