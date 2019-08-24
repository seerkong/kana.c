#include "kon_cell.h"
#include "../../container/kx_vector.h"
#include "../../container/kx_hashtable.h"

KN KonCell_Init(KonState* kstate, KN args)
{
    KonCell* value = KN_ALLOC_TYPE_TAG(kstate, KonCell, KN_T_CELL);
    value->Core = KN_UNDEF;
    value->Vector = KN_UNDEF;
    value->Table = KN_UNDEF;
    value->List = KN_UNDEF;
    return (KN)value;
}

KN KonCell_SetCore(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    self->Core = KN_CADR(args);
    return (KN)self;
}

KN KonCell_GetCore(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    return (KN)self->Core;
}

KN KonCell_DelCore(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    self->Core = KN_UNDEF;
    return (KN)self;
}

KN KonCell_SetVector(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    self->Vector = KN_CADR(args);
    return (KN)self;
}

KN KonCell_GetVector(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    return (KN)self->Vector;
}

KN KonCell_DelVector(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    self->Vector = KN_UNDEF;
    return (KN)self;
}

KN KonCell_ClearVector(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    KxVector* vector = KN_UNBOX_VECTOR(self->Vector);
    KxVector_Clear(vector);
    return (KN)self;
}

KN KonCell_SetTable(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    self->Table = KN_CADR(args);
    return self;
}

KN KonCell_GetTable(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    return (KN)self->Table;
}

KN KonCell_DelTable(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    self->Table = KN_UNDEF;
    return (KN)self;
}

KN KonCell_ClearTable(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    KxHashTable* table = KN_UNBOX_VECTOR(self->Table);
    KxHashTable_Clear(table);
    return (KN)self;
}

KN KonCell_SetList(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    self->List = KN_CADR(args);
    return (KN)self;
}

KN KonCell_GetList(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    return (KN)self->List;
}

KN KonCell_DelList(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
    self->List = KN_UNDEF;
    return (KN)self;
}

KN KonCell_ClearList(KonState* kstate, KN args)
{
    KonCell* self = CAST_Kon(Cell, KN_CAR(args));
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
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_Init),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "set-core",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_SetCore),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "get-core",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_GetCore),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "del-core",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_DelCore),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "set-vector",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_SetVector),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "get-vector",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_GetVector),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "del-vector",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_DelVector),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "clear-vector",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_ClearVector),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "set-table",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_SetTable),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "get-table",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_GetTable),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "del-table",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_DelTable),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "clear-table",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_ClearTable),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "set-list",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_SetList),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "get-list",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_GetList),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "del-list",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_DelList),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "clear-list",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonCell_ClearList),
        "r",
        NULL
    );


    return slot;
}
