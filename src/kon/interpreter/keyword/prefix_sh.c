#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* KN_EvalPrefixSh(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro sh");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));
    KN arguments = KN_CellCoresToList(kstate, expression);

    KxStringBuffer* sb = KxStringBuffer_New();

    KN iter = arguments;
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

    KonTrampoline* bounce;
    bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
    bounce->Run.Value = KN_TRUE;
    bounce->Cont = cont;

    return bounce;
}
