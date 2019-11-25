#ifndef KN_INTERP_PREFIX_H
#define KN_INTERP_PREFIX_H 1

#include "../prefix.h"
#include "../utils/utils.h"
#include "../container/container.h"
#include "../kon/kon.h"


typedef struct _NativeProcedureConf NativeProcedureConf;
typedef struct _NativeExportItem NativeExportItem;
typedef struct _NativeExportConf NativeExportConf;

struct _NativeProcedureConf {
    KonProcedureType type;
    KonNativeFuncRef funcRef;
    int paramNum;   // arg num before ...
    int hasVAList;   // if have ... in arg list
    int hasVAMap;   // variable argument map
    int hasBlock;
};

enum NativeExportItemType {
    KN_NATIVE_EXPORT_PROC,
};

struct _NativeExportItem {
    enum NativeExportItemType type;
    char* name;
    union {
        NativeProcedureConf proc;
    };
};

struct _NativeExportConf {
    int len;
    NativeExportItem* items;
};


#endif