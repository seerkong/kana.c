#include "kon_list.h"

KN KonList_New(KonState* kstate, KN args)
{
    return args;
}

KN KonList_Length(KonState* kstate, KN args)
{
    KN self = kon_car(args);
    return KON_PairListLength(kstate, self);
}

KonAttrSlot* KonList_Init(KonState* kstate)
{
    KonAttrSlot* slot = (KonAttrSlot*)MakeAttrSlotFolder(kstate, "");

    KxHashTable_PutKv(slot->Folder,
        "new",
        MakeAttrSlotLeaf(kstate,
            MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_New),
            "p"
        )
    );


    KonAttrSlot* methods = (KonAttrSlot*)MakeAttrSlotFolder(kstate, "");
    
    KxHashTable_PutKv(slot->Folder,
        "methods",
        methods
    );

    KxHashTable_PutKv(methods->Folder,
        "length",
        MakeAttrSlotLeaf(kstate,
            MakeNativeProcedure(kstate, KON_NATIVE_OBJ_METHOD, KonList_Length),
            "pm"
        )
    );
    return slot;
}