#ifndef KN_GC_KIKUMARU_H
#define KN_GC_KIKUMARU_H 1


// #define KN_DISABLE_GC 0
#define KN_DISABLE_GC 1

#include "prefix.h"
#include "../kon/value.h"


typedef struct _GcState
{

} GcState;

void KN_InitGc(KonState* knState);
void KN_DestroyGc(KonState* knState);

// alloced at parsing time, readonly, free them when destroy KonState
KN_API KN KN_NewConstMemObj(KonState* kstate, size_t size, kon_uint_t tag);

// alloced at exec time, can be garbage collected
KN_API KN KN_NewDynamicMemObj(KonState* kstate, size_t size, kon_uint_t tag);

void KN_Gc(KonState* knState);



#endif