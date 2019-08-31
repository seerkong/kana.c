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

// 1 need one next cell core
// {xx := 2}
// 2 need one next cell core. if is a word, convert to identifier
// {xxx / abc / efg}
// {xxx / @var-a / @var-b}
// {xxx . $abc . $efg}
// {xxx . abc . efg}
// 3 need semicolon to seperate next clause
// {xxx % arg1 arg2 arg3 ...; % arg1 arg2}
// {xxx | proc arg2 arg3 ...; | proc2}
// 4 need semicolon to seperate next clause
// convert word to identifier
// {xxx abc arg1 arg2 ...}
// {xxx @var-a arg1 arg2 ...}
static inline KN KN_CellToWordList(KonState* kstate, KN source)
{
    KonCell* head = CAST_Kon(Cell, source);
    KonCell* iter = head;
    KN result = KN_NIL;
    while ((KN)iter != KN_NIL) {
        KN core = iter->Core;
        result = KN_CONS(kstate, core, result);
        iter = iter->Next;
    }
    return KN_PairListRevert(kstate, result);
}

// apply argument list to a native function
static inline KN KN_ApplyArgsToNativeFunc(KonState* kstate, KonProcedure* proc, KN argList) {
    int paramLen = proc->ParamNum;
    int hasVAList = proc->HasVAList;
    int hasVAMap = proc->HasVAMap;
    KonNativeFuncRef funcRef = proc->NativeFuncRef;

    KN args[12] = { KN_UNDEF };

    KN iter = argList;
    int nextBindIndex = 0;
    for (int i = 0; i < paramLen; i++) {
        args[nextBindIndex] = KN_CAR(iter);
        iter = KN_CDR(iter);
        nextBindIndex += 1;
    }
    if (hasVAList) {
        // save the rest arguments to VAList
        args[nextBindIndex] = iter;
    }

    int actualParamLen = paramLen;
    if (hasVAMap) {
        actualParamLen += 2;
    }
    else if (hasVAList) {
        actualParamLen += 1;
    }

    KN result = KN_UKN;
    switch (actualParamLen) {
        case 0: {
            result = (*funcRef)(kstate);
            break;
        }
        case 1: {
            result = (*funcRef)(kstate, args[0]);
            break;
        }
        case 2: {
            result = (*funcRef)(kstate, args[0], args[1]);
            break;
        }
        case 3: {
            result = (*funcRef)(kstate, args[0], args[1], args[2]);
            break;
        }
        case 4: {
            result = (*funcRef)(kstate, args[0], args[1], args[2], args[3]);
            break;
        }
        case 5: {
            result = (*funcRef)(kstate, args[0], args[1], args[2], args[3], args[4]);
            break;
        }
    }
    return result;
}

#ifdef __cplusplus
}
#endif

#endif