#ifndef KN_GC_TINA_H
#define KN_GC_TINA_H 1


// #define KN_DISABLE_GC 0
#define KN_DISABLE_GC 1

#include "prefix.h"
#include "../kon/value.h"


typedef struct _GcState
{

} GcState;

void KN_InitGc(KonState* knState);
void KN_DestroyGc(KonState* knState);

KN_API KN KN_AllocTagged(KonState* kstate, size_t size, kon_uint_t tag);


void KN_ShowGcStatics(KonState* knState);
long long KN_CurrentObjCount(KonState* knState);
void KN_SwitchContinuation(KonState* knState, struct _KonContinuation* cont);
void KN_RecordNewKonNode(KonState* knState, KN newVal);

void KN_EnterGcSafepoint(KonState* knState);
void KN_Gc(KonState* knState);



#endif