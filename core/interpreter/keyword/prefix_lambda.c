#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* KN_ApplyCompositeLambda(KonState* kstate, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* cont)
{
    KonEnv* parentEnv = proc->composite.lexicalEnv;
    KN param = proc->composite.argList;
    KN body = proc->composite.body;
    KN captureList = proc->composite.captureList;

    // KN_DEBUG("param def %s", KN_StringToCstr(KN_ToFormatString(kstate, param, true, 0, "  ")));
    // KN_DEBUG("argList %s", KN_StringToCstr(KN_ToFormatString(kstate, argList, true, 0, "  ")));
    // KN_DEBUG("body %s", KN_StringToCstr(KN_ToFormatString(kstate, body, true, 0, "  ")));

    KonEnv* procBindEnv = KN_MakeChildEnv(kstate, parentEnv);
    KN_EnvDefine(kstate, procBindEnv, "return", cont);

    // bind caputure var to dynamic env
    KN captureVarIter = ((KN)captureList == KN_UNDEF) ? KN_NIL : captureList;
    while ((KN)captureVarIter != KN_NIL) {
        KN outerVar = KN_CAR(captureVarIter);

        const char* varName = KN_UNBOX_SYMBOL(outerVar);
        KN varValue = KN_EnvLookup(kstate, env, varName);
        KN_DEBUG("capture varName  %s varValue %s",
            varName,
            KN_StringToCstr(KN_ToFormatString(kstate, varValue, true, 0, "  "))
        );
        KN_EnvDefine(kstate, procBindEnv, varName, varValue);

        captureVarIter = KN_CDR(captureVarIter);
    }

    KonPair* iterParam = param;
    KonPair* iterArg = argList;
    while ((KN)iterParam != KN_NIL) {
        KN param = KN_CAR(iterParam);
        KN arg = KN_CAR(iterArg);
        // if this param is the last, the rest args should bind this param
        if (KN_CDR(iterParam) == KN_NIL
            && KN_CDR(iterArg) != KN_NIL
        ) {
            arg = iterArg;
        }
        const char* varName = KN_UNBOX_SYMBOL(param);
        KN_DEBUG("arg %s cstr %s, bind value %s",
            KN_StringToCstr(KN_ToFormatString(kstate, param, true, 0, "  ")),
            varName,
            KN_StringToCstr(KN_ToFormatString(kstate, arg, true, 0, "  "))
        );
        KN_EnvDefine(kstate, procBindEnv, varName, arg);

        iterParam = KN_CDR(iterParam);
        iterArg = KN_CDR(iterArg);
    };

    KonTrampoline* bounce = KN_EvalSentences(kstate, body, procBindEnv, cont);
    return bounce;
}

KonTrampoline* KN_EvalPrefixLambda(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont)
{
    KN_DEBUG("meet prefix lambda");
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
    proc->type = KN_COMPOSITE_LAMBDA;
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
