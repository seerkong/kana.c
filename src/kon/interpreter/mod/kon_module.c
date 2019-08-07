#include "kon_number.h"
#include "kon_string.h"
#include "kon_list.h"
#include "kon_vector.h"
#include "kon_table.h"
#include "kon_cell.h"

#include "kon_module.h"

KN KonModule_Export(KonState* kstate, KonEnv* env)
{
    KonAttrSlot* module = (KonAttrSlot*)MakeAttrSlotFolder(kstate, "");
    
    KxHashTable_PutKv(module->Folder,
        "number",
        KonNumber_Export(kstate, env)
    );

    KxHashTable_PutKv(module->Folder,
        "string",
        KonString_Export(kstate, env)
    );

    KxHashTable_PutKv(module->Folder,
        "list",
        KonList_Export(kstate, env)
    );

    KxHashTable_PutKv(module->Folder,
        "vector",
        KonVector_Export(kstate, env)
    );

    KxHashTable_PutKv(module->Folder,
        "table",
        KonTable_Export(kstate, env)
    );

    KxHashTable_PutKv(module->Folder,
        "cell",
        KonCell_Export(kstate, env)
    );

    return (KN)module;
}
