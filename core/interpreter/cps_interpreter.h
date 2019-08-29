#ifndef KN_CPS_INTERPRETER_H
#define KN_CPS_INTERPRETER_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../prefix.h"
#include "types.h"
#include "env.h"

bool IsSelfEvaluated(KN source);
KonTrampoline* ApplyProcedureArguments(KonState* kstate, KonProcedure* proc, KN argList, KonEnv* env, KonContinuation* cont);
KonTrampoline* AssignValueToAccessor(KonState* kstate, KN accessorKon, KN assignTo, KonEnv* env, KonContinuation* cont);
KN UnBoxAccessorValue(KN konValue);
KonTrampoline* ApplySubjVerbAndObjects(KonState* kstate, KN subj, KN argList, KonEnv* env, KonContinuation* cont);

KN KN_ProcessSentences(KonState* kstate, KN sentences, KonEnv* env);

KonContinuation* AllocContinuationWithType(KonState* kstate, KonContinuationType type);
KonTrampoline* AllocBounceWithType(KonState* kstate, KonBounceType type);
KonTrampoline* KN_EvalSentences(KonState* kstate, KN sentences, KonEnv* env, KonContinuation* cont);
KonTrampoline* KN_EvalExpression(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont);
KonTrampoline* ApplySubjVerbAndObjects(KonState* kstate, KN subj, KN argList, KonEnv* env, KonContinuation* cont);

#ifdef __cplusplus
}
#endif

#endif