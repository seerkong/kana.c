#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KN AfterDispatcherIdEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    unsigned int dispatcherId = KON_UNBOX_FIXNUM(evaledValue);
    KN env = contBeingInvoked->Env;
    KxHashTable* memo = contBeingInvoked->Native.MemoTable;
    KonMsgDispatcher* dispatcher = (KonMsgDispatcher*)KxHashTable_AtKey(memo, "Dispatcher");

    int res = KON_SetMsgDispatcher(kstate, dispatcherId, dispatcher);
    
    KonTrampoline* bounce;
    bounce = AllocBounceWithType(kstate, KON_TRAMPOLINE_RUN);
    bounce->Run.Value = KON_TRUE;
    bounce->Cont = contBeingInvoked->Cont;

    return bounce;
}


KonTrampoline* KON_EvalPrefixSetDispatcher(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro def dispatcher");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    
    KN dispatcherIdExpr = KON_CAR(expression);

    KN config = KON_CADR(expression);

    KonMsgDispatcher* dispatcher = MakeMsgDispatcher(kstate);

    KxHashTable* configTable = ((KonTable*)config)->Table;
    
    KN onApplyArgs = KxHashTable_AtKey(configTable, "on-apply-args");  // % p1 p2;
    KN onSelectPath = KxHashTable_AtKey(configTable, "on-select-path");  // /abc /efg
    KN onMethodCall = KxHashTable_AtKey(configTable, "on-method-call"); // .push 1 2;
    KN onVisitVector = KxHashTable_AtKey(configTable, "on-visit-vector");  // <>
    KN onVisitTable = KxHashTable_AtKey(configTable, "on-visit-table"); // ()
    KN onVisitCell = KxHashTable_AtKey(configTable, "on-visit-cell");  // {}


    dispatcher->OnApplyArgs = MakeDispatchProc(kstate, onApplyArgs, env);
    dispatcher->OnSelectPath = MakeDispatchProc(kstate, onSelectPath, env);
    dispatcher->OnMethodCall = MakeDispatchProc(kstate, onMethodCall, env);
    dispatcher->OnVisitVector = MakeDispatchProc(kstate, onVisitVector, env);
    dispatcher->OnVisitTable = MakeDispatchProc(kstate, onVisitTable, env);
    dispatcher->OnVisitCell = MakeDispatchProc(kstate, onVisitCell, env);


    KonContinuation* k = AllocContinuationWithType(kstate, KON_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = env;

    KxHashTable* memo = KxHashTable_Init(2);
    KxHashTable_PutKv(memo, "Dispatcher", dispatcher);
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterDispatcherIdEvaled;
    
    KonTrampoline* bounce;
    bounce = KON_EvalExpression(kstate, dispatcherIdExpr, env, k);


    return bounce;
}
