#ifndef KON_INTERPRETER_ENV_H
#define KON_INTERPRETER_ENV_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../prefix.h"
#include "types.h"
#include "primary.h"

// typedef struct {
//     Kon* Parent;
//     // pair list. pair car key, pair cdr value
//     Kon* Bindings;
// } KonEnv;

Kon* KON_MakeRootEnv(Kon* kstate);
Kon* KON_MakeChildEnv(Kon* kstate, Kon* parentEnv);
Kon* KON_EnvDefine(Kon* kstate, Kon* env, char* key, Kon* value);
Kon* KON_EnvLookup(Kon* kstate, Kon* env, char* key);

#ifdef __cplusplus
}
#endif

#endif