#include "kon_list.h"
#include "kon_module.h"

KN KonModule_Init(KonState* kstate)
{
    KonAttrSlot* module = (KonAttrSlot*)MakeAttrSlotFolder(kstate, "");
    
    KxHashTable_PutKv(module->Folder,
        "list",
        KonList_Init(kstate)
    );
}