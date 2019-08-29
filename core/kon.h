#ifndef KN_H
#define KN_H

#ifdef __cplusplus
extern "C"{
#endif


#include "prefix.h"

////
// API start

// script lifetime start

KN_API KonState* KN_Init();
KN_API int KN_Finish(KonState* kstate);

KN_API KN KN_EvalFile(KonState* kstate, char* filePath);

// script lifetime end

// API end
////


#include "kson/kson.h"


#ifdef __cplusplus
}
#endif

#endif