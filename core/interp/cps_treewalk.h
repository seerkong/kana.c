#ifndef KN_CPS_TREEWALK_H
#define KN_CPS_TREEWALK_H 1

#include "prefix.h"


KN KN_EvalFile(KonState* knstate, char* filePath);
KN KN_ExecAst(KonState* knstate, KN sentences, KonEnv* env);


// apply argument list to a native function
static inline KN KN_ApplyArgsToNativeFunc(KonState* kstate, KonProcedure* proc, KN argList) {
    int paramLen = proc->paramNum;
    int hasVAList = proc->hasVAList;
    int hasVAMap = proc->hasVAMap;
    KonNativeFuncRef funcRef = proc->nativeFuncRef;

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

#endif