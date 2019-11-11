#ifndef KN_GC_KIKUMARU_H
#define KN_GC_KIKUMARU_H 1


// #define KN_DISABLE_GC 0
#define KN_DISABLE_GC 1

#include "prefix.h"
#include "../kon/value.h"


typedef struct _GcState
{

} GcState;

void KN_InitGc(Kana* kana);
void KN_DestroyGc(Kana* kana);

// alloced at parsing time, readonly, free them when destroy Kana
KN_API KN KN_NewConstMemObj(Kana* kana, size_t size, kon_uint_t tag);

// alloced at exec time, can be garbage collected
KN_API KN KN_NewDynamicMemObj(Kana* kana, size_t size, kon_uint_t tag);

void KN_Gc(Kana* kana);



#endif