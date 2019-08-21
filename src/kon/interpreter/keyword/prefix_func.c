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
    KON_EnvDefine(kstate, procBindEnv, "return", cont);
    
    KonPair* iterParam = param;
    KonPair* iterArg = argList;
    while (iterParam != KON_NIL) {
        KN param = KON_CAR(iterParam);
        KN arg = KON_CAR(iterArg);
        // if this param is the last, the rest args should bind this param
        if (KON_CDR(iterParam) == KON_NIL
            && KON_CDR(iterArg) != KON_NIL
        ) {
            arg = iterArg;
        }
        const char* varName = KON_UNBOX_SYMBOL(param);
        KON_DEBUG("arg %s cstr %s, bind value %s",
            KON_StringToCstr(KON_ToFormatString(kstate, param, true, 0, "  ")),
            varName,
            KON_StringToCstr(KON_ToFormatString(kstate, arg, true, 0, "  "))
        );
        KON_EnvDefine(kstate, procBindEnv, varName, arg);

        iterParam = KON_CDR(iterParam);
        iterArg = KON_CDR(iterArg);
    };

    KonTrampoline* bounce = KON_EvalSentences(kstate, body, procBindEnv, cont);
    return bounce;
}

KonTrampoline* KON_EvalPrefixFunc(KonState* kstate, KN expression, KN env, KonContinuation* cont)
{
    KON_DEBUG("meet prefix marcro func");
    KON_DEBUG("rest words %s", KON_StringToCstr(KON_ToFormatString(kstate, expression, true, 0, "  ")));
    KN param = KON_DTR(expression);
    KN funcName = KON_UNDEF;
    KN body = KON_NIL;

    if (param == KON_UNDEF) {
        funcName = KON_DCNR(expression);
        param = KON_DTNR(expression);
        body = KON_DLNR(expression);
    }
    else {
        body = KON_DLR(expression);
    }

    param = KON_ParamTableToList(kstate, param);

    KON_DEBUG("funcName %s", KON_StringToCstr(KON_ToFormatString(kstate, funcName, true, 0, "  ")));
    KON_DEBUG("param %s", KON_StringToCstr(KON_ToFormatString(kstate, param, true, 0, "  ")));
    KON_DEBUG("body %s", KON_StringToCstr(KON_ToFormatString(kstate, body, true, 0, "  ")));

    KonProcedure* proc = KON_ALLOC_TYPE_TAG(kstate, KonProcedure, KON_T_PROCEDURE);
    proc->Type = KON_COMPOSITE_FUNC;
    proc->Composite.LexicalEnv = env;
    proc->Composite.ArgList = param;
    proc->Composite.Body = body;

    if (KON_IS_WORD(funcName)) {
        const char* varNameCstr = KON_UNBOX_SYMBOL(funcName);
        KON_EnvDefine(kstate, env, varNameCstr, proc);
    }

    KonTrampoline* bounce = AllocBounceWithType(kstate, KON_TRAMPOLINE_RUN);
    bounce->Run.Value = proc;
    bounce->Cont = cont;
    return bounce;
}
