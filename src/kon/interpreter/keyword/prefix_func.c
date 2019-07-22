#include "stdio.h"
#include "stdlib.h"
#include "prefix_if.h"
#include "../cps_interpreter.h"

KonTrampoline* KON_ApplyCompositeFunc(KonState* kstate, KonProcedure* proc, KN argList, KN env, KonContinuation* cont)
{
    KonEnv* parentEnv = env;
    KN param = proc->Composite.ArgList;
    KN body = proc->Composite.Body;

    KON_DEBUG("param def %s", KON_StringToCstr(KON_ToFormatString(kstate, param, true, 0, "  ")));
    KON_DEBUG("argList %s", KON_StringToCstr(KON_ToFormatString(kstate, argList, true, 0, "  ")));
    KON_DEBUG("body %s", KON_StringToCstr(KON_ToFormatString(kstate, body, true, 0, "  ")));

    KonEnv* procBindEnv = KON_MakeChildEnv(kstate, parentEnv);

    KonListNode* iterParam = param;
    KonListNode* iterArg = kon_cdr(argList);
    while (iterParam != KON_NIL) {
        KN param = kon_car(iterParam);
        KN arg = kon_car(iterArg);
        // if this param is the last, the rest args should bind this param
        if (kon_cdr(iterParam) == KON_NIL
            && kon_cdr(iterArg) != KON_NIL
        ) {
            arg = iterArg;
        }
        const char* varName = KON_UNBOX_SYMBOL(KON_UNBOX_QUOTE(param));
        KON_DEBUG("arg %s cstr %s, bind value %s",
            KON_StringToCstr(KON_ToFormatString(kstate, param, true, 0, "  ")),
            varName,
            KON_StringToCstr(KON_ToFormatString(kstate, arg, true, 0, "  "))
        );
        KON_EnvDefine(kstate, procBindEnv, varName, arg);

        iterParam = kon_cdr(iterParam);
        iterArg = kon_cdr(iterArg);
    };

    KonTrampoline* bounce = KON_EvalSentences(kstate, body, procBindEnv, cont);
    return bounce;
}

KonTrampoline* KON_EvalPrefixFunc(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro func");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    KN first = kon_car(expression);
    KN param = KON_NIL;
    KN funcName = KON_NULL;
    KN body = KON_NIL;
    if (KON_IS_IDENTIFER(first)) {
        funcName = first;
        param = kon_cadr(expression);
        body = kon_cddr(expression);
    }
    else if (KON_IsList(first)) {
        param = first;
        body = kon_cdr(expression);
    }

    KON_DEBUG("funcName %s", KON_StringToCstr(KON_ToFormatString(kstate, funcName, true, 0, "  ")));
    KON_DEBUG("param %s", KON_StringToCstr(KON_ToFormatString(kstate, param, true, 0, "  ")));
    KON_DEBUG("body %s", KON_StringToCstr(KON_ToFormatString(kstate, body, true, 0, "  ")));

    KonProcedure* proc = KON_ALLOC_TYPE_TAG(kstate, KonProcedure, KON_T_PROCEDURE);
    proc->Type = KON_COMPOSITE_FUNC;
    proc->Composite.LexicalEnv = env;
    proc->Composite.ArgList = param;
    proc->Composite.Body = body;

    if (KON_IS_IDENTIFER(funcName)) {
        const char* varNameCstr = KON_UNBOX_SYMBOL(funcName);
        KON_EnvDefine(kstate, env, varNameCstr, proc);
    }

    KonTrampoline* bounce = AllocBounceWithType(KON_TRAMPOLINE_RUN);
    bounce->Run.Value = proc;
    bounce->Run.Cont = cont;
    return bounce;
}
