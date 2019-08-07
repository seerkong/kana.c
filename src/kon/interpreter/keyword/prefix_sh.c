#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* KON_EvalPrefixSh(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro sh");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    
    KxStringBuffer* sb = KxStringBuffer_New();

    KN iter = expression;
    while (iter != KON_NIL) {
        KN next = KON_CDR(iter);
        KN item = KON_CAR(iter);
        if (KON_IS_SYMBOL(item)) {
            KxStringBuffer_AppendCstr(sb, KON_UNBOX_SYMBOL(item));
        }
        else if (KON_IS_STRING(item)) {
            KxStringBuffer_AppendCstr(sb, KxStringBuffer_Cstr(KON_UNBOX_STRING(item)));
        }

        if (next != KON_NIL) {
            KxStringBuffer_AppendCstr(sb, " ");
        }
        iter = next;
    }

    KON_DEBUG("exec shell cmd: %s", KxStringBuffer_Cstr(sb));
    system(KxStringBuffer_Cstr(sb));

    KonTrampoline* bounce;
    bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
    bounce->Run.Value = KON_TRUE;
    bounce->Run.Cont = cont;

    return bounce;
}
