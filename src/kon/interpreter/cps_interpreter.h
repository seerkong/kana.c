#ifndef KON_CPS_INTERPRETER_H
#define KON_CPS_INTERPRETER_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../prefix.h"
#include "types.h"
#include "env.h"

Kon* KON_ProcessSentences(Kon* kstate, Kon* sentences, Kon* env);

KonContinuation* AllocContinuationWithType(KonContinuationType type);
KonTrampoline* AllocBounceWithType(KonBounceType type);
KonTrampoline* KON_EvalSentences(Kon* kstate, Kon* sentences, Kon* env, KonContinuation* cont);
KonTrampoline* ApplySubjVerbAndObjects(Kon* kstate, Kon* subj, Kon* argList, Kon* env, KonContinuation* cont);

#ifdef __cplusplus
}
#endif

#endif