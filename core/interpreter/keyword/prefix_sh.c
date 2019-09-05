#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KN CmdWordToIdentifier(KN source)
{
    if (KN_IS_WORD(source)) {
        ((KonSymbol*)source)->type = KN_SYM_IDENTIFIER;
    }
    return source;
}

KN AfterShellCmdEvaled(KonState* kstate, KN evaledValue, KonContinuation* contBeingInvoked)
{
    KonEnv* env = contBeingInvoked->env;
    KxHashTable* memo = KxHashTable_ShadowClone(contBeingInvoked->native.memoTable);
    KN restCmdWords = KxHashTable_AtKey(memo, "RestCmdWords");
    KN evaled = KxHashTable_AtKey(memo, "Evaled");
    evaled = KN_CONS(kstate, evaledValue, evaled);
    KN_DEBUG("restCmdWords %s", KN_StringToCstr(KN_ToFormatString(kstate, restCmdWords, true, 0, "  ")));
    
    KN_DEBUG("evaled %s", KN_StringToCstr(KN_ToFormatString(kstate, evaled, true, 0, "  ")));
    

    KonTrampoline* bounce;
    if (restCmdWords == KN_NIL) {
        KxStringBuffer* sb = KxStringBuffer_New();

        KN iter = KN_PairListRevert(kstate, evaled);
        while (iter != KN_NIL) {
            KN next = KN_CDR(iter);
            KN item = KN_CAR(iter);
            if (KN_IS_SYMBOL(item)) {
                KxStringBuffer_AppendCstr(sb, KN_UNBOX_SYMBOL(item));
            }
            else if (KN_IS_STRING(item)) {
                KxStringBuffer_AppendCstr(sb, KxStringBuffer_Cstr(KN_UNBOX_STRING(item)));
            }

            if (next != KN_NIL) {
                KxStringBuffer_AppendCstr(sb, " ");
            }
            iter = next;
        }

        KN_DEBUG("exec shell cmd: %s", KxStringBuffer_Cstr(sb));
        system(KxStringBuffer_Cstr(sb));
        KN_DEBUG("exec shell cmd finished");

        bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
        bounce->run.value = KN_TRUE;
        bounce->cont = contBeingInvoked->cont;
    }
    else {
        // next condition
        KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
        k->cont = contBeingInvoked->cont;
        k->env = env;

        KxHashTable* memo = KxHashTable_Init(4);
        KxHashTable_PutKv(memo, "RestCmdWords", KN_CDR(restCmdWords));
        KxHashTable_PutKv(memo, "Evaled", evaled);

        k->native.memoTable = memo;
        k->native.callback = AfterShellCmdEvaled;

        bounce = KN_EvalExpression(kstate, CmdWordToIdentifier(KN_CAR(restCmdWords)), env, k);
    }

    return bounce;
}

KonTrampoline* KN_EvalPrefixSh(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro sh");
    KN arguments = KN_CellCoresToList(kstate, expression);
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, arguments, true, 0, "  ")));

    KonContinuation* k = AllocContinuationWithType(kstate, KN_CONT_NATIVE_CALLBACK);
    k->cont = cont;
    k->env = env;

    KxHashTable* memo = KxHashTable_Init(4);
    KxHashTable_PutKv(memo, "RestCmdWords", KN_CDR(arguments));
    KxHashTable_PutKv(memo, "Evaled", KN_NIL);

    k->native.memoTable = memo;
    k->native.callback = AfterShellCmdEvaled;

    KonTrampoline* bounce = KN_EvalExpression(kstate, CmdWordToIdentifier(KN_CAR(arguments)), env, k);

    return bounce;
}
