#ifndef kn_interp_primitives_h
#define kn_interp_primitives_h 1

#include "prefix.h"

KN KN_PrimaryNewline(Kana* kana);
KN KN_PrimaryDisplay(Kana* kana, KN args);
KN KN_PrimaryWrite(Kana* kana, KN args);
KN KN_PrimaryStringify(Kana* kana, KN args);

KN KN_PrimaryOpExport(Kana* kana, KonEnv* env);

#endif