#include "kon_cell.h"
#include "../../container/vector/kx_vector.h"
#include "../../container/hashtable/kx_hashtable.h"

KN KonCell_Init(KonState* kstate, KN args)
{
    KonCell* value = KON_ALLOC_TYPE_TAG(kstate, KonCell, KON_T_CELL);
    value->Name = KON_NULL;
    value->Vector = KON_NULL;
    value->Table = KON_NULL;
    value->List = KON_NULL;
    return value;
}

KN KonCell_SetName(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->Name = KON_CADR(args);
    return self;
}

KN KonCell_GetName(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    return self->Name;
}

KN KonCell_DelName(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->Name = KON_NULL;
    return self;
}

KN KonCell_SetVector(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->Vector = KON_CADR(args);
    return self;
}

KN KonCell_GetVector(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    return self->Vector;
}

KN KonCell_DelVector(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->Vector = KON_NULL;
    return self;
}

KN KonCell_ClearVector(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    KxVector* vector = KON_UNBOX_VECTOR(self->Vector);
    KxVector_Clear(vector);
    return self;
}

KN KonCell_SetTable(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->Table = KON_CADR(args);
    return self;
}

KN KonCell_GetTable(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    return self->Table;
}

KN KonCell_DelTable(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->Table = KON_NULL;
    return self;
}

KN KonCell_ClearTable(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    KxHashTable* table = KON_UNBOX_VECTOR(self->Table);
    KxHashTable_Clear(table);
    return self;
}

KN KonCell_SetList(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->List = KON_CADR(args);
    return self;
}

KN KonCell_GetList(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    return self->List;
}

KN KonCell_DelList(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->List = KON_NULL;
    return self;
}

KN KonCell_ClearList(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->List = KON_NIL;
    return self;
}

KonAttrSlot* KonCell_Export(KonState* kstate, KonEnv* env)
{
    KON_EnvDefine(kstate, env, "cell-init",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_Init)
    );

    KON_EnvDefine(kstate, env, "cell-set-name",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_SetName)
    );
    KON_EnvDefine(kstate, env, "cell-get-name",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_GetName)
    );
    KON_EnvDefine(kstate, env, "cell-del-name",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_DelName)
    );
    KON_EnvDefine(kstate, env, "cell-set-vector",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_SetVector)
    );
    KON_EnvDefine(kstate, env, "cell-get-vector",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_GetVector)
    );
    KON_EnvDefine(kstate, env, "cell-del-vector",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_DelVector)
    );
    KON_EnvDefine(kstate, env, "cell-clear-vector",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_ClearVector)
    );
    KON_EnvDefine(kstate, env, "cell-set-table",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_SetTable)
    );
    KON_EnvDefine(kstate, env, "cell-get-table",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_GetTable)
    );
    KON_EnvDefine(kstate, env, "cell-del-table",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_DelTable)
    );
    KON_EnvDefine(kstate, env, "cell-clear-table",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_ClearTable)
    );
    KON_EnvDefine(kstate, env, "cell-set-list",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_SetList)
    );
    KON_EnvDefine(kstate, env, "cell-get-list",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_GetList)
    );
    KON_EnvDefine(kstate, env, "cell-del-list",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_DelList)
    );
    KON_EnvDefine(kstate, env, "cell-clear-list",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_ClearList)
    );

    KonAttrSlot* slot = (KonAttrSlot*)MakeAttrSlotFolder(kstate, "");
    return slot;
}