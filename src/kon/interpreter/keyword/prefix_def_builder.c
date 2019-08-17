#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KN AfterBuilderDispatcherIdEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    unsigned int dispatcherId = KON_UNBOX_FIXNUM(evaledValue);
    KN env = contBeingInvoked->Env;
    KxHashTable* memo = contBeingInvoked->Native.MemoTable;
    KonCell* sourceCode = (KonCell*)KxHashTable_AtKey(memo, "BuilderSourceCode");

    KN className = ((KonCell*)sourceCode)->Core;
    const char* classNameCstr = KON_UNBOX_SYMBOL(className);
    KON_DEBUG("dispatcherId %d classNameCstr %s", dispatcherId, classNameCstr);

    KonAttrSlot* objectBuilder = (KonAttrSlot*)MakeAttrSlotFolder(kstate, "");
    
    // set object instance dispatcher id
    ((KonBase*)objectBuilder)->MsgDispatcherId = dispatcherId;
    
    // if have a __proto config
    KxHashTable* configTable = ((KonCell*)sourceCode)->Table->Table;
    
    KxHashTableIter iter = KxHashTable_IterHead(configTable);
    while (iter != KX_HASH_TABLE_NIL) {
        KxHashTableIter next = KxHashTable_IterNext(configTable, iter);
        const char* key = KxHashTable_IterGetKey(configTable, iter);
        KN val = (KN)KxHashTable_IterGetVal(configTable, iter);
        
        KxHashTable_PutKv(objectBuilder->Folder,
            key,
            val
        );

        iter = next;
    }

    KonTrampoline* bounce;

    KON_EnvDefine(kstate, env, classNameCstr, objectBuilder);
    bounce = AllocBounceWithType(kstate, KON_TRAMPOLINE_RUN);
    bounce->Run.Value = objectBuilder;
    bounce->Cont = contBeingInvoked->Cont;

    return bounce;
}


KonTrampoline* KON_EvalPrefixDefBuilder(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro def builder");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    
    KN dispatcherIdExpr = KON_CAR(expression);

    KN config = KON_CADR(expression);


    KonContinuation* k = AllocContinuationWithType(kstate, KON_CONT_NATIVE_CALLBACK);
    k->Cont = cont;
    k->Env = env;

    KxHashTable* memo = KxHashTable_Init(2);
    KxHashTable_PutKv(memo, "BuilderSourceCode", config);
    k->Native.MemoTable = memo;
    k->Native.Callback = AfterBuilderDispatcherIdEvaled;
    
    KonTrampoline* bounce;
    bounce = KON_EvalExpression(kstate, dispatcherIdExpr, env, k);

    return bounce;
}
