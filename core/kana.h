#ifndef kana_h
#define kana_h

#include "prefix.h"


#include "box/box.h"
#include "utils/utils.h"
#include "container/container.h"
#include "string/string.h"
#include "kon/kon.h"
#include "gc/gc.h"
#include "interp/interp.h"

KN_API Kana* KN_Init();
KN_API int KN_Finish(Kana* kana);

#endif