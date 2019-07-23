#include "kon_list.h"

KN KonList_New(KonState* kstate, KN args)
{
    return KON_NIL;
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
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_New)
    );


    KonAttrSlot* methods = (KonAttrSlot*)MakeAttrSlotFolder(kstate, "");
    
    KxHashTable_PutKv(slot->Folder,
        "methods",
        methods
    );

    KxHashTable_PutKv(methods->Folder,
        "length",
        MakeNativeProcedure(kstate, KON_NATIVE_OBJ_METHOD, KonList_Length)
    );

}