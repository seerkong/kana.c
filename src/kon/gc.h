#ifndef KN_GC_H
#define KN_GC_H 1

// #define KN_DISABLE_GC 0
#define KN_DISABLE_GC 1

#ifdef __cplusplus
extern "C" {
#endif

void KN_InitGc(KonState* kstate);
void KN_DestroyGc(KonState* kstate);
void KN_ShowGcStatics(KonState* kstate);
long long KN_CurrentObjCount(KonState* kstate);
void KN_SwitchContinuation(KonState* kstate, KonContinuation* cont);
void KN_RecordNewKonNode(KonState* kstate, KN newVal);

void KN_Gc(KonState* kstate);

void KN_MarkPhase(KonState* kstate);
void KN_Mark(KonState* kstate, KxList* taskQueue, char color);
void KN_SweepPhase();
void KN_ResetAndCopyPtrSegList(KonState* kstate);
void KN_MarkNode(KonBase* item, KxList* markTaskQueue, char color);
void KN_DestroyNode(KonState* kstate, KonBase* item);

#ifdef __cplusplus
}
#endif

#endif