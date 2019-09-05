#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* KN_EvalPrefixMacroLambda(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix marcro lambda");
    KN_DEBUG("rest words %s", KN_StringToCstr(KN_ToFormatString(kstate, expression, true, 0, "  ")));

    KN param = KN_DTR(expression);
    KN funcName = KN_UNDEF;
    KN body = KN_NIL;
    KN captureList = KN_UNDEF;

    if (param == KN_UNDEF) {
        funcName = KN_DCNR(expression);
        param = KN_DTNR(expression);
        body = KN_DLNR(expression);
        KonCell* nextCell = (KonCell*)KN_DNR(expression);
        if (nextCell->map != KN_UNDEF) {
            KxHashTable* unboxedMap = nextCell->map->map;
            captureList = KxHashTable_AtKey(unboxedMap, "capture");
        }
    }
    else {
        body = KN_DLR(expression);
        KonCell* currCell = CAST_Kon(Cell, expression);
        if ((KN)currCell->map != KN_UNDEF) {
            KxHashTable* unboxedMap = currCell->map->map;
            captureList = KxHashTable_AtKey(unboxedMap, "capture");
        }
    }

    param = KN_ParamTableToList(kstate, param);

    KN_DEBUG("funcName %s", KN_StringToCstr(KN_ToFormatString(kstate, funcName, true, 0, "  ")));
    KN_DEBUG("param %s", KN_StringToCstr(KN_ToFormatString(kstate, param, true, 0, "  ")));
    KN_DEBUG("capture list %s", KN_StringToCstr(KN_ToFormatString(kstate, captureList, true, 0, "  ")));
    KN_DEBUG("body %s", KN_StringToCstr(KN_ToFormatString(kstate, body, true, 0, "  ")));

    KonProcedure* proc = KN_ALLOC_TYPE_TAG(kstate, KonProcedure, KN_T_PROCEDURE);
    proc->type = KN_COMPOSITE_MACRO_LAMBDA;
    proc->composite.lexicalEnv = env;
    proc->composite.argList = param;
    proc->composite.body = body;
    proc->composite.captureList = captureList;

    if (KN_IS_WORD(funcName)) {
        const char* varNameCstr = KN_UNBOX_SYMBOL(funcName);
        KN_EnvDefine(kstate, env, varNameCstr, proc);
    }

    KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
    bounce->run.value = proc;
    bounce->cont = cont;
    return bounce;
}
