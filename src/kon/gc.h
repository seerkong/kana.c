#ifndef KON_GC_H
#define KON_GC_H 1

#define KON_DISABLE_GC 0

#ifdef __cplusplus
extern "C" {
#endif

void KON_InitGc(KonState* kstate);
void KON_DestroyGc(KonState* kstate);
void KON_ShowGcStatics(KonState* kstate);
long long KON_CurrentObjCount(KonState* kstate);
void KON_SwitchContinuation(KonState* kstate, KonContinuation* cont);
void KON_RecordNewKonNode(KonState* kstate, KN newVal);

void KON_Gc(KonState* kstate);

void KON_MarkPhase(KonState* kstate);
void KON_Mark(KonState* kstate, KxList* taskQueue, char color);
void KON_SweepPhase();
void KON_ResetAndCopyPtrSegList(KonState* kstate);
void KON_MarkNode(KonBase* item, KxList* markTaskQueue, char color);
void KON_DestroyNode(KonState* kstate, KonBase* item);

#ifdef __cplusplus
}
#endif

#endif