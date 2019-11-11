#ifndef KN_GC_TINA_H
#define KN_GC_TINA_H 1


// #define KN_DISABLE_GC 0
#define KN_DISABLE_GC 1

#include "prefix.h"
#include "../kon/value.h"


typedef struct _GcState
{

} GcState;

void KN_InitGc(Kana* kana);
void KN_DestroyGc(Kana* kana);

KN_API KN KN_NewDynamicMemObj(Kana* kana, size_t size, kon_uint_t tag);


void KN_ShowGcStatics(Kana* kana);
long long KN_CurrentObjCount(Kana* kana);
void KN_SwitchContinuation(Kana* kana, struct _KonContinuation* cont);
void KN_RecordNewKonNode(Kana* kana, KN newVal);

void KN_EnterGcSafepoint(Kana* kana);
void KN_Gc(Kana* kana);



#endif