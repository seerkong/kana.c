#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"


KonTrampoline* KN_EvalPrefixDefDispatcher(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro def dispatcher");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));
    KN config = KN_DCR(expression);

    KonMsgDispatcher* dispatcher = MakeMsgDispatcher(kstate);

    KxHashTable* configTable = ((KonTable*)config)->table;
    
    KN onSymbol = KxHashTable_AtKey(configTable, "on-symbol");  // [obj key1 = 2]
    KN onSyntaxMarker = KxHashTable_AtKey(configTable, "on-syntax-marker");  // % p1 p2;
    KN onMethodCall = KxHashTable_AtKey(configTable, "on-method-call"); // .push 1 2;
    KN onVisitList = KxHashTable_AtKey(configTable, "on-visit-list");
    KN onVisitVector = KxHashTable_AtKey(configTable, "on-visit-vector");  // <>
    KN onVisitTable = KxHashTable_AtKey(configTable, "on-visit-table"); // ()
    KN onVisitCell = KxHashTable_AtKey(configTable, "on-visit-cell");  // {}
    KN onOtherType = KxHashTable_AtKey(configTable, "on-other-type");

    dispatcher->onSymbol = MakeDispatchProc(kstate, onSymbol, env);
    dispatcher->onSyntaxMarker = MakeDispatchProc(kstate, onSyntaxMarker, env);
    dispatcher->onVisitList = MakeDispatchProc(kstate, onVisitList, env);
    dispatcher->onMethodCall = MakeDispatchProc(kstate, onMethodCall, env);
    dispatcher->onVisitVector = MakeDispatchProc(kstate, onVisitVector, env);
    dispatcher->onVisitTable = MakeDispatchProc(kstate, onVisitTable, env);
    dispatcher->onVisitCell = MakeDispatchProc(kstate, onVisitCell, env);
    dispatcher->onOtherType = MakeDispatchProc(kstate, onOtherType, env);

    unsigned int dispatcherId = KN_SetNextMsgDispatcher(kstate, dispatcher);

    KN boxedDispacherId = KN_MAKE_FIXNUM(dispatcherId);

    KonTrampoline* bounce;
    bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
    bounce->run.value = boxedDispacherId;
    bounce->cont = cont;

    return bounce;
}
