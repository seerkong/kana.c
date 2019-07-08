#ifndef KON_H
#define KON_H

#ifdef __cplusplus
extern "C"{
#endif


#include "prefix.h"

////
// API start

// script lifetime start

KON_API int KON_Init(Kon* kstate);
KON_API int KON_Finish(Kon* kstate);

KON_API Kon* KON_EvalFile(Kon* kstate, char* filePath);

// script lifetime end

// API end
////


#include "kson/kson.h"


#ifdef __cplusplus
}
#endif

#endif