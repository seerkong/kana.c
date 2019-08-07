


//         // vector
//         || strcmp(funcName, "make-vector")
//         || strcmp(funcName, "is-vector")
//         || strcmp(funcName, "vector-push")   // add tail
//         || strcmp(funcName, "vector-pop")    // get tail and remove
//         || strcmp(funcName, "vector-shift")  // get head and remove
//         || strcmp(funcName, "vector-unshift")    // add head
//         || strcmp(funcName, "vector-head")
//         || strcmp(funcName, "vector-tail")
//         || strcmp(funcName, "vector-at")
//         || strcmp(funcName, "vector-del")
#ifndef KON_MOD_VECTOR_H
#define KON_MOD_VECTOR_H 1

#include "../../kson/node.h"

KonAttrSlot* KonVector_Export(KonState* kstate, KonEnv* env);

#endif
