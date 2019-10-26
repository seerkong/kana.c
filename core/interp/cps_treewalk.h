#ifndef KN_CPS_TREEWALK_H
#define KN_CPS_TREEWALK_H 1

#include "prefix.h"


KN KN_EvalFile(KonState* knstate, char* filePath);
KN KN_ExecAst(KonState* knstate, KN sentences, KonEnv* env);

#endif