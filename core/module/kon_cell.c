#include "kon_cell.h"
#include "../container/kx_vector.h"
#include "../container/kx_hashtable.h"

KN KonCell_Init(KonState* kstate)
{
    KonCell* value = KN_ALLOC_TYPE_TAG(kstate, KonCell, KN_T_CELL);
    value->Core = KN_UNDEF;
    value->Vector = KN_UNDEF;
    value->Table = KN_UNDEF;
    value->List = KN_UNDEF;
    return (KN)value;
}

KN KonCell_SetCore(KonState* kstate, KN cell, KN core)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->Core = core;
    return (KN)self;
}

KN KonCell_GetCore(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    return (KN)self->Core;
}

KN KonCell_DelCore(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->Core = KN_UNDEF;
    return (KN)self;
}

KN KonCell_SetVector(KonState* kstate, KN cell, KN vector)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->Vector = vector;
    return (KN)self;
}

KN KonCell_GetVector(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    return (KN)self->Vector;
}

KN KonCell_DelVector(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->Vector = KN_UNDEF;
    return (KN)self;
}

KN KonCell_ClearVector(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    KxVector* vector = KN_UNBOX_VECTOR(self->Vector);
    KxVector_Clear(vector);
    return (KN)self;
}

KN KonCell_SetTable(KonState* kstate, KN cell, KN table)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->Table = table;
    return self;
}

KN KonCell_GetTable(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    return (KN)self->Table;
}

KN KonCell_DelTable(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->Table = KN_UNDEF;
    return (KN)self;
}

KN KonCell_ClearTable(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    KxHashTable* table = KN_UNBOX_VECTOR(self->Table);
    KxHashTable_Clear(table);
    return (KN)self;
}

KN KonCell_SetList(KonState* kstate, KN cell, KN list)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->List = list;
    return (KN)self;
}

KN KonCell_GetList(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    return (KN)self->List;
}

KN KonCell_DelList(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->List = KN_UNDEF;
    return (KN)self;
}

KN KonCell_ClearList(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->List = KN_NIL;
    return (KN)self;
}

KonAccessor* KonCell_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* slot = (KonAccessor*)KN_MakeDirAccessor(kstate, "dr", NULL);
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "init",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_Init, 0, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "set-core",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_SetCore, 2, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "get-core",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_GetCore, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "del-core",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_DelCore, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "set-vector",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_SetVector, 2, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "get-vector",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_GetVector, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "del-vector",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_DelVector, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "clear-vector",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_ClearVector, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "set-table",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_SetTable, 2, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "get-table",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_GetTable, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "del-table",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_DelTable, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "clear-table",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_ClearTable, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "set-list",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_SetList, 2, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "get-list",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_GetList, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "del-list",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_DelList, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "clear-list",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_ClearList, 1, 0, 0),
        "r",
        NULL
    );


    return slot;
}
