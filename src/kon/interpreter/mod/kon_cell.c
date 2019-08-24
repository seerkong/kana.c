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
    return (KN)value;
}

KN KonCell_SetCore(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->Core = KON_CADR(args);
    return (KN)self;
}

KN KonCell_GetCore(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    return (KN)self->Core;
}

KN KonCell_DelCore(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->Core = KON_UNDEF;
    return (KN)self;
}

KN KonCell_SetVector(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->Vector = KON_CADR(args);
    return (KN)self;
}

KN KonCell_GetVector(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    return (KN)self->Vector;
}

KN KonCell_DelVector(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->Vector = KON_UNDEF;
    return (KN)self;
}

KN KonCell_ClearVector(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    KxVector* vector = KON_UNBOX_VECTOR(self->Vector);
    KxVector_Clear(vector);
    return (KN)self;
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
    return (KN)self->Table;
}

KN KonCell_DelTable(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->Table = KON_UNDEF;
    return (KN)self;
}

KN KonCell_ClearTable(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    KxHashTable* table = KON_UNBOX_VECTOR(self->Table);
    KxHashTable_Clear(table);
    return (KN)self;
}

KN KonCell_SetList(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->List = KON_CADR(args);
    return (KN)self;
}

KN KonCell_GetList(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    return (KN)self->List;
}

KN KonCell_DelList(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->List = KON_UNDEF;
    return (KN)self;
}

KN KonCell_ClearList(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KON_CAR(args));
    self->List = KON_NIL;
    return (KN)self;
}

KonAccessor* KonCell_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* slot = (KonAccessor*)KON_MakeDirAccessor(kstate, "dr", NULL);
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "init",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_Init),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "set-core",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_SetCore),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "get-core",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_GetCore),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "del-core",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_DelCore),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "set-vector",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_SetVector),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "get-vector",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_GetVector),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "del-vector",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_DelVector),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "clear-vector",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_ClearVector),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "set-table",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_SetTable),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "get-table",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_GetTable),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "del-table",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_DelTable),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "clear-table",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_ClearTable),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "set-list",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_SetList),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "get-list",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_GetList),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "del-list",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_DelList),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "clear-list",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonCell_ClearList),
        "r",
        NULL
    );


    return slot;
}
