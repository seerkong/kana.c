#include "kon_cell.h"
#include "../container/kx_vector.h"
#include "../container/kx_hashtable.h"

KN KonCell_Init(KonState* kstate)
{
    KonCell* value = KN_ALLOC_TYPE_TAG(kstate, KonCell, KN_T_CELL);
    value->core = KN_UNDEF;
    value->map = KN_UNDEF;
    value->table = KN_UNDEF;
    value->list = KN_UNDEF;
    return (KN)value;
}

KN KonCell_SetCore(KonState* kstate, KN cell, KN core)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->core = core;
    return (KN)self;
}

KN KonCell_GetCore(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    return (KN)self->core;
}

KN KonCell_DelCore(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->core = KN_UNDEF;
    return (KN)self;
}

KN KonCell_SetMap(KonState* kstate, KN cell, KN map)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->map = map;
    return (KN)self;
}

KN KonCell_GetMap(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    return (KN)self->map;
}

KN KonCell_DelMap(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->map = KN_UNDEF;
    return (KN)self;
}

KN KonCell_ClearMap(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    KxHashTable* map = KN_UNBOX_TABLE(self->map);
    KxHashTable_Clear(map);
    return (KN)self;
}

KN KonCell_SetTable(KonState* kstate, KN cell, KN table)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->table = table;
    return self;
}

KN KonCell_GetTable(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    return (KN)self->table;
}

KN KonCell_DelTable(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->table = KN_UNDEF;
    return (KN)self;
}

KN KonCell_ClearTable(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    KxHashTable* table = KN_UNBOX_TABLE(self->table);
    KxHashTable_Clear(table);
    return (KN)self;
}

KN KonCell_SetList(KonState* kstate, KN cell, KN list)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->list = list;
    return (KN)self;
}

KN KonCell_GetList(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    return (KN)self->list;
}

KN KonCell_DelList(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->list = KN_UNDEF;
    return (KN)self;
}

KN KonCell_ClearList(KonState* kstate, KN cell)
{
    KonCell* self = CAST_Kon(Cell, cell);
    self->list = KN_NIL;
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
        "set-map",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_SetMap, 2, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "get-map",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_GetMap, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "del-map",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_DelMap, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "clear-map",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_ClearMap, 1, 0, 0),
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
