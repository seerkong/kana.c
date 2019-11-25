#include "kmod_map.h"
#include "../container/kn_hashtable.h"

KN KonMap_Init(Kana* kana)
{
    KonMap* value = KN_NEW_DYNAMIC_OBJ(kana, KonMap, KN_T_MAP);
    value->map = KnHashTable_Init(10);
    return KON_2_KN(value);
}

KN KonMap_Length(Kana* kana, KN self)
{
    KnHashTable* value = KN_UNBOX_MAP(self);
    return KN_MAKE_FIXNUM(KnHashTable_Length(value));
}

KN KonMap_Clear(Kana* kana, KN self)
{
    KnHashTable* value = KN_UNBOX_MAP(self);
    KnHashTable_Clear(value);
    return self;
}

KN KonMap_HasKey(Kana* kana, KN self, KN key)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));

    KnHashTable* value = KN_UNBOX_MAP(self);
    return KnHashTable_HasKey(value, keyCstr) ? KN_TRUE : KN_FALSE;
}

KN KonMap_AtKey(Kana* kana, KN self, KN key)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    KnHashTable* value = KN_UNBOX_MAP(self);
    return (KN)KnHashTable_AtKey(value, keyCstr);
}

KN KonMap_PutKv(Kana* kana, KN self, KN key, KN val)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    
    KnHashTable* table = KN_UNBOX_MAP(self);
    KnHashTable_PutKv(table, keyCstr, val.asU64);
    return self;
}

KN KonMap_DelByKey(Kana* kana, KN self, KN key)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));

    KnHashTable* value = KN_UNBOX_MAP(self);
    KnHashTable_DelByKey(value, keyCstr);
    return self;
}

KN KonMap_IterHead(Kana* kana, KN self)
{
    KnHashTable* table = KN_UNBOX_MAP(self);
    
    KnHashTableIter iter = KnHashTable_IterHead(table);
    return KN_BOX_EXT_PTR(iter);
}

KN KonMap_IterTail(Kana* kana, KN self)
{
    KnHashTable* table = KN_UNBOX_MAP(self);
    
    KnHashTableIter iter = KnHashTable_IterTail(table);
    return KN_BOX_EXT_PTR(iter);
}

KN KonMap_IterPrev(Kana* kana, KN self, KN iter)
{
    KnHashTable* table = KN_UNBOX_MAP(self);
    
    KnHashTableIter prev = KnHashTable_IterPrev(table, KN_UNBOX_EXT_PTR(iter));
    if (prev != KNBOX_NIL) {
        return KN_BOX_EXT_PTR(prev);
    }
    else {
        return KN_NIL;
    }
}

KN KonMap_IterNext(Kana* kana, KN self, KN iter)
{
    KnHashTable* table = KN_UNBOX_MAP(self);
    
    KnHashTableIter next = KnHashTable_IterNext(table, KN_UNBOX_EXT_PTR(iter));
    if (next != KNBOX_NIL) {
        return KN_BOX_EXT_PTR(next);
    }
    else {
        return KN_NIL;
    }
}

KN KonMap_IterGetKey(Kana* kana, KN self, KN iter)
{
    KnHashTable* table = KN_UNBOX_MAP(self);
    
    const char* key = KnHashTable_IterGetKey(table, KN_UNBOX_EXT_PTR(iter));
    KonString* value = KN_NEW_DYNAMIC_OBJ(kana, KonString, KN_T_STRING);
    value->string = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->string, key);
    return KON_2_KN(value);
}

KN KonMap_IterGetVal(Kana* kana, KN self, KN iter)
{
    KnHashTable* table = KN_UNBOX_MAP(self);
    
    return (KN)KnHashTable_IterGetVal(table, KN_UNBOX_EXT_PTR(iter));
}

NativeExportConf Kmod_Map_Export(Kana* kana)
{
    NativeExportItem exportArr[] = {
        { .type = KN_NATIVE_EXPORT_PROC, .name = "init", .proc = { KN_NATIVE_FUNC, KonMap_Init, 0, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "length", .proc = { KN_NATIVE_FUNC, KonMap_Length, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "clear", .proc = { KN_NATIVE_FUNC, KonMap_Clear, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "has-key", .proc = { KN_NATIVE_FUNC, KonMap_HasKey, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "at-key", .proc = { KN_NATIVE_FUNC, KonMap_AtKey, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "put-kv", .proc = { KN_NATIVE_FUNC, KonMap_PutKv, 3, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "del-key", .proc = { KN_NATIVE_FUNC, KonMap_DelByKey, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "iter-head", .proc = { KN_NATIVE_FUNC, KonMap_IterHead, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "iter-tail", .proc = { KN_NATIVE_FUNC, KonMap_IterTail, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "iter-prev", .proc = { KN_NATIVE_FUNC, KonMap_IterPrev, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "iter-next", .proc = { KN_NATIVE_FUNC, KonMap_IterNext, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "iter-key", .proc = { KN_NATIVE_FUNC, KonMap_IterGetKey, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "iter-val", .proc = { KN_NATIVE_FUNC, KonMap_IterGetVal, 1, 0, 0, 0 } },
    };

    int len = 13;
    NativeExportItem* items = (NativeExportItem*)calloc(len, sizeof(NativeExportItem));
    memcpy(items, exportArr, len * sizeof(NativeExportItem));
    NativeExportConf res = {
        .len = len,
        .items = items
    };
    
    return res;
}
