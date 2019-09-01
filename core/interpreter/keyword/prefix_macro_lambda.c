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

    if (param == KN_UNDEF) {
        funcName = KN_DCNR(expression);
        param = KN_DTNR(expression);
        body = KN_DLNR(expression);
    }
    else {
        body = KN_DLR(expression);
    }

    param = KN_ParamTableToList(kstate, param);

    KN_DEBUG("funcName %s", KN_StringToCstr(KN_ToFormatString(kstate, funcName, true, 0, "  ")));
    KN_DEBUG("param %s", KN_StringToCstr(KN_ToFormatString(kstate, param, true, 0, "  ")));
    KN_DEBUG("body %s", KN_StringToCstr(KN_ToFormatString(kstate, body, true, 0, "  ")));

    KonProcedure* proc = KN_ALLOC_TYPE_TAG(kstate, KonProcedure, KN_T_PROCEDURE);
    proc->Type = KN_COMPOSITE_MACRO_LAMBDA;
    proc->Composite.LexicalEnv = env;
    proc->Composite.ArgList = param;
    proc->Composite.Body = body;

    if (KN_IS_WORD(funcName)) {
        const char* varNameCstr = KN_UNBOX_SYMBOL(funcName);
        KN_EnvDefine(kstate, env, varNameCstr, proc);
    }

    KonTrampoline* bounce = AllocBounceWithType(kstate, KN_TRAMPOLINE_RUN);
    bounce->Run.Value = proc;
    bounce->Cont = cont;
    return bounce;
}
