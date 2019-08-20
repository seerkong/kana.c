#include "kon_cell.h"
#include "../../container/kx_vector.h"
#include "../../container/kx_hashtable.h"

KN KonCell_Init(KonState* kstate, KN args)
{
    KonCell* value = KON_ALLOC_TYPE_TAG(kstate, KonCell, KON_T_CELL);
    value->Core = KON_UNDEF;
    value->Vector = KON_UNDEF;
    value->Table = KON_UNDEF;
    value->List = KON_UNDEF;
    return value;
}

KN KonCell_SetCore(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->Core = KON_CADR(args);
    return self;
}

KN KonCell_GetCore(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    return self->Core;
}

KN KonCell_DelCore(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->Core = KON_UNDEF;
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
    self->Vector = KON_UNDEF;
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
    self->Table = KON_UNDEF;
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
    self->List = KON_UNDEF;
    return self;
}

KN KonCell_ClearList(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->List = KON_NIL;
    return self;
}

KonAccessor* KonCell_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* slot = (KonAccessor*)KON_MakeDirAccessor(kstate, "dr", NULL);
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "init",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_Init),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "set-core",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_SetCore),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "get-core",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_GetCore),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "del-core",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_DelCore),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "set-vector",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_SetVector),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "get-vector",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_GetVector),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "del-vector",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_DelVector),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "clear-vector",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_ClearVector),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "set-table",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_SetTable),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "get-table",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_GetTable),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "del-table",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_DelTable),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "clear-table",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_ClearTable),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "set-list",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_SetList),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "get-list",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_GetList),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "del-list",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_DelList),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "clear-list",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_ClearList),
        "r",
        NULL
    );


    return slot;
}
