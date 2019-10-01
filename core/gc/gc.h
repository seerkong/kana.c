#ifndef KN_GC_H
#define KN_GC_H 1

// #define KN_DISABLE_GC 0
#define KN_DISABLE_GC 1

#include "prefix.h"
#include "../kon/value.h"


typedef struct _GcState
{

} GcState;

void KN_InitGc(KonState* knState);
void KN_DestroyGc(KonState* knState);
void KN_ShowGcStatics(KonState* knState);
long long KN_CurrentObjCount(KonState* knState);
void KN_SwitchContinuation(KonState* knState, struct _KonContinuation* cont);
void KN_RecordNewKonNode(KonState* knState, KN newVal);

void KN_EnterGcSafepoint(KonState* knState);
void KN_Gc(KonState* knState);

void KN_MarkPhase(KonState* knState);
void KN_Mark(KonState* knState, KxList* taskQueue, char color);
void KN_SweepPhase();
void KN_ResetAndCopyPtrSegList(KonState* knState);
void KN_MarkNode(struct _KonBase* item, KxList* markTaskQueue, char color);
void KN_DestroyNode(KonState* knState, struct _KonBase* item);



#endif