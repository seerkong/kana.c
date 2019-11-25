#include "kmod_cell.h"
#include "../container/kn_vector.h"
#include "../container/kn_hashtable.h"

KN KonCell_Init(Kana* kana)
{
    KonCell* value = KN_NEW_DYNAMIC_OBJ(kana, KonCell, KN_T_CELL);
    value->core = KN_UNDEF;
    value->map = KN_2_KON(KN_UNDEF, Map);
    value->table = KN_2_KON(KN_UNDEF, Table);
    value->list = KN_2_KON(KN_UNDEF, Pair);
    return KON_2_KN(value);
}

KN KonCell_SetCore(Kana* kana, KN cell, KN core)
{
    KonCell* self = KN_2_KON(cell, Cell);
    self->core = core;
    return cell;
}

KN KonCell_GetCore(Kana* kana, KN cell)
{
    KonCell* self = KN_2_KON(cell, Cell);
    return self->core;
}

KN KonCell_DelCore(Kana* kana, KN cell)
{
    KonCell* self = KN_2_KON(cell, Cell);
    self->core = KN_UNDEF;
    return cell;
}

KN KonCell_SetMap(Kana* kana, KN cell, KN map)
{
    KonCell* self = KN_2_KON(cell, Cell);
    self->map = KN_2_KON(map, Map);
    return cell;
}

KN KonCell_GetMap(Kana* kana, KN cell)
{
    KonCell* self = KN_2_KON(cell, Cell);
    return KON_2_KN(self->map);
}

KN KonCell_DelMap(Kana* kana, KN cell)
{
    KonCell* self = KN_2_KON(cell, Cell);
    self->map = KN_2_KON(KN_UNDEF, Map);
    return cell;
}

KN KonCell_ClearMap(Kana* kana, KN cell)
{
    KonCell* self = KN_2_KON(cell, Cell);
    KnHashTable* map = self->map;
    KnHashTable_Clear(map);
    return cell;
}

KN KonCell_SetTable(Kana* kana, KN cell, KN table)
{
    KonCell* self = KN_2_KON(cell, Cell);
    self->table = KN_2_KON(table, Table);
    return cell;
}

KN KonCell_GetTable(Kana* kana, KN cell)
{
    KonCell* self = KN_2_KON(cell, Cell);
    return KON_2_KN(self->table);
}

KN KonCell_DelTable(Kana* kana, KN cell)
{
    KonCell* self = KN_2_KON(cell, Cell);
    self->table = KNBOX_UNDEF;
    return cell;
}

KN KonCell_ClearTable(Kana* kana, KN cell)
{
    KonCell* self = KN_2_KON(cell, Cell);
    KnHashTable* table = self->table;
    KnHashTable_Clear(table);
    return cell;
}

KN KonCell_SetList(Kana* kana, KN cell, KN list)
{
    KonCell* self = KN_2_KON(cell, Cell);
    self->list = KN_2_KON(list, Pair);
    return cell;
}

KN KonCell_GetList(Kana* kana, KN cell)
{
    KonCell* self = KN_2_KON(cell, Cell);
    return KON_2_KN(self->list);
}

KN KonCell_DelList(Kana* kana, KN cell)
{
    KonCell* self = KN_2_KON(cell, Cell);
    self->list = KNBOX_UNDEF;
    return cell;
}

KN KonCell_ClearList(Kana* kana, KN cell)
{
    KonCell* self = KN_2_KON(cell, Cell);
    self->list = KNBOX_NIL;
    return cell;
}


NativeExportConf Kmod_Cell_Export(Kana* kana)
{
    NativeExportItem exportArr[] = {
        { .type = KN_NATIVE_EXPORT_PROC, .name = "init", .proc = { KN_NATIVE_FUNC, KonCell_Init, 0, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "set-core", .proc = { KN_NATIVE_FUNC, KonCell_SetCore, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "get-core", .proc = { KN_NATIVE_FUNC, KonCell_GetCore, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "del-core", .proc = { KN_NATIVE_FUNC, KonCell_DelCore, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "set-map", .proc = { KN_NATIVE_FUNC, KonCell_SetMap, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "get-map", .proc = { KN_NATIVE_FUNC, KonCell_GetMap, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "del-map", .proc = { KN_NATIVE_FUNC, KonCell_DelMap, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "clear-map", .proc = { KN_NATIVE_FUNC, KonCell_ClearMap, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "set-table", .proc = { KN_NATIVE_FUNC, KonCell_SetTable, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "get-table", .proc = { KN_NATIVE_FUNC, KonCell_GetTable, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "del-table", .proc = { KN_NATIVE_FUNC, KonCell_DelTable, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "clear-table", .proc = { KN_NATIVE_FUNC, KonCell_ClearTable, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "set-list", .proc = { KN_NATIVE_FUNC, KonCell_SetList, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "get-list", .proc = { KN_NATIVE_FUNC, KonCell_GetList, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "del-list", .proc = { KN_NATIVE_FUNC, KonCell_DelList, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "clear-list", .proc = { KN_NATIVE_FUNC, KonCell_ClearList, 1, 0, 0, 0 } },
    };

    int len = 16;
    NativeExportItem* items = (NativeExportItem*)calloc(len, sizeof(NativeExportItem));
    memcpy(items, exportArr, len * sizeof(NativeExportItem));
    NativeExportConf res = {
        .len = len,
        .items = items
    };
    
    return res;
}