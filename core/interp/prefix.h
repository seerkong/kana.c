#ifndef KN_INTERP_PREFIX_H
#define KN_INTERP_PREFIX_H 1

#include "../prefix.h"
#include "../utils/utils.h"
#include "../container/container.h"
#include "../kon/kon.h"


typedef struct _NativeProcedureConf NativeProcedureConf;

struct _NativeProcedureConf {
    KonProcedureType type;
    char* name;
    KonNativeFuncRef funcRef;
    int paramNum;   // arg num before ...
    int hasVAList;   // if have ... in arg list
    int hasVAMap;   // variable argument map
    int hasBlock;
};


#endif