#ifndef kn_interp_primitives_h
#define kn_interp_primitives_h 1

#include "prefix.h"

KN KN_PrimaryNewline(KonState* kstate);
KN KN_PrimaryDisplay(KonState* kstate, KN args);
KN KN_PrimaryWrite(KonState* kstate, KN args);
KN KN_PrimaryStringify(KonState* kstate, KN args);

KN KN_PrimaryOpExport(KonState* kstate, KonEnv* env);

#endif