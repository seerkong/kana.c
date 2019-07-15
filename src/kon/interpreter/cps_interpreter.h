#ifndef KON_CPS_INTERPRETER_H
#define KON_CPS_INTERPRETER_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../prefix.h"
#include "types.h"
#include "env.h"

KN KON_ProcessSentences(KonState* kstate, KN sentences, KN env);

KonContinuation* AllocContinuationWithType(KonContinuationType type);
KonTrampoline* AllocBounceWithType(KonBounceType type);
KonTrampoline* KON_EvalSentences(KonState* kstate, KN sentences, KN env, KonContinuation* cont);
KonTrampoline* KON_EvalExpression(KonState* kstate, KN expression, KN env, KonContinuation* cont);
KonTrampoline* ApplySubjVerbAndObjects(KonState* kstate, KN subj, KN argList, KonEnv* env, KonContinuation* cont);

#ifdef __cplusplus
}
#endif

#endif