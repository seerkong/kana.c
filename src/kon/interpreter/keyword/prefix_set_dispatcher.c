#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* AfterDispatcherIdEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    unsigned int dispatcherId = KN_UNBOX_FIXNUM(evaledValue);
    KonEnv* env = contBeingInvoked->Env;
    KxHashTable* memo = contBeingInvoked->Native.MemoTable;
    KonMsgDispatcher* dispatcher = (KonMsgDispatcher*)KxHashTable_AtKey(memo, "Dispatcher");

    int res = KN_SetMsgDispatcher(kstate, dispatcherId, dispatcher);
    
    KonTrampoline* bounce;
    bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
    bounce->Run.Value = KN_TRUE;
    bounce->Cont = contBeingInvoked->Cont;

    return bounce;
}


KonTrampoline* KN_EvalPrefixSetDispatcher(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro def dispatcher");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));
    
    KN dispatcherIdExpr = KN_DCR(expression);

    KN config = KN_DCNR(expression);

    KonMsgDispatcher* dispatcher = MakeMsgDispatcher(kstate);

    KxHashTable* configTable = ((KonTable*)config)->Table;
    
    KN onSymbol = KxHashTable_AtKey(configTable, "on-symbol");  // [obj key1 = 2]
    KN onApplyArgs = KxHashTable_AtKey(configTable, "on-apply-args");  // % p1 p2;
    KN onSelectPath = KxHashTable_AtKey(configTable, "on-select-path");  // /abc /efg
    KN onMethodCall = KxHashTable_AtKey(configTable, "on-method-call"); // .push 1 2;
    KN onVisitVector = KxHashTable_AtKey(configTable, "on-visit-vector");  // <>
    KN onVisitTable = KxHashTable_AtKey(configTable, "on-visit-table"); // ()
    KN onVisitCell = KxHashTable_AtKey(configTable, "on-visit-cell");  // {}

    dispatcher->OnSymbol = MakeDispatchProc(kstate, onSymbol, env);
    dispatcher->OnApplyArgs = MakeDispatchProc(kstate, onApplyArgs, env);
    dispatcher->OnSelectPath = MakeDispatchProc(kstate, onSelectPath, env);
    dispatcher->OnMethodCall = MakeDispatchProc(kstate, onMethodCall, env);
    dispatcher->OnVisitVector = MakeDispatchProc(kstate, onVisitVector, env);
    dispatcher->OnVisitTable = MakeDispatchProc(kstate, onVisitTable, env);
    dispatcher->OnVisitCell = MakeDispatchProc(kstate, onVisitCell, env);


    KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = env;

    KxHashTable* memo = KxHashTable_Init(2);
    KxHashTable_PutKv(memo, "Dispatcher", dispatcher);
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterDispatcherIdEvaled;
    
    KonTrampoline* bounce;
    bounce = KN_EvalExpression(kstate, dispatcherIdExpr, env, k);


    return bounce;
}
