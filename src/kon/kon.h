#ifndef KON_H
#define KON_H

#ifdef __cplusplus
extern "C"{
#endif


#include "prefix.h"

////
// API start

// script lifetime start

KON_API int KON_Init(KonState* kstate);
KON_API int KON_Finish(KonState* kstate);

KON_API KN KON_EvalFile(KonState* kstate, char* filePath);

// script lifetime end

// API end
////


#include "kson/kson.h"


#ifdef __cplusplus
}
#endif

#endif