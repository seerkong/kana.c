#include "kmod_table.h"
#include "../container/kn_hashtable.h"

KN KonTable_Init(Kana* kana)
{
    KonTable* value = KN_NEW_DYNAMIC_OBJ(kana, KonTable, KN_T_TABLE);
    value->table = KnHashTable_Init(10);
    return KON_2_KN(value);
}

KN KonTable_Length(Kana* kana, KN self)
{
    KnHashTable* value = KN_UNBOX_TABLE(self);
    return KN_MAKE_FIXNUM(KnHashTable_Length(value));
}

KN KonTable_Clear(Kana* kana, KN self)
{
    KnHashTable* value = KN_UNBOX_TABLE(self);
    KnHashTable_Clear(value);
    return self;
}

KN KonTable_HasKey(Kana* kana, KN self, KN key)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));

    KnHashTable* value = KN_UNBOX_TABLE(self);
    return KnHashTable_HasKey(value, keyCstr) ? KN_TRUE : KN_FALSE;
}

KN KonTable_AtKey(Kana* kana, KN self, KN key)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    KnHashTable* value = KN_UNBOX_TABLE(self);
    return (KN)KnHashTable_AtKey((ktable_val_t)value, keyCstr);
}

KN KonTable_ValAtIndex(Kana* kana, KN self, KN index)
{
    int indexNum = KN_UNBOX_FIXNUM(index);
    KnHashTable* value = KN_UNBOX_TABLE(self);
    return (KN)KnHashTable_ValAtIndex(value, indexNum);
}

KN KonTable_KeyAtIndex(Kana* kana, KN self, KN index)
{
    int indexNum = KN_UNBOX_FIXNUM(index);

    KnHashTable* value = KN_UNBOX_TABLE(self);
    
    const char* key = KnHashTable_KeyAtIndex(value, indexNum);
    KonString* keyStr = KN_NEW_DYNAMIC_OBJ(kana, KonString, KN_T_STRING);
    keyStr->string = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(keyStr, key);

    return KON_2_KN(keyStr);
}

KN KonTable_FirstVal(Kana* kana, KN self)
{
    KnHashTable* value = KN_UNBOX_TABLE(self);
    return (KN)KnHashTable_FirstVal(value);
}

KN KonTable_LastVal(Kana* kana, KN self)
{
    KnHashTable* value = KN_UNBOX_TABLE(self);
    return (KN)KnHashTable_LastVal(value);
}

KN KonTable_PushVal(Kana* kana, KN self, KN val)
{
    KnHashTable* table = KN_UNBOX_TABLE(self);
    KnHashTable_PushVal(table, val.asU64);
    return self;
}

KN KonTable_PushKv(Kana* kana, KN self, KN key, KN val)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    KnHashTable* table = KN_UNBOX_TABLE(self);
    KnHashTable_PushKv(table, keyCstr, val.asU64);
    return self;
}

KN KonTable_UnshiftVal(Kana* kana, KN self, KN val)
{
    KnHashTable* table = KN_UNBOX_TABLE(self);
    KnHashTable_UnshiftVal(table, val.asU64);
    return self;
}

KN KonTable_UnshiftKv(Kana* kana, KN self, KN key, KN val)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    
    KnHashTable* value = KN_UNBOX_TABLE(self);
    KnHashTable_UnshiftKv(value, keyCstr, val.asU64);
    return self;
}

KN KonTable_PutKv(Kana* kana, KN self, KN key, KN val)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    
    KnHashTable* value = KN_UNBOX_TABLE(self);
    KnHashTable_PutKv(value, keyCstr, val.asU64);
    return self;
}

// self, index, key
KN KonTable_SetIndexKey(Kana* kana, KN self, KN index, KN key)
{   
    int indexNum = KN_UNBOX_FIXNUM(index);
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    

    KnHashTable* value = KN_UNBOX_TABLE(self);
    KnHashTable_SetKeyAtIndex(value, indexNum, keyCstr);
    return self;
}

// self,  index, val,
KN KonTable_SetIndexVal(Kana* kana, KN self, KN index, KN val)
{    
    int indexNum = KN_UNBOX_FIXNUM(index);
    
    KnHashTable* value = KN_UNBOX_TABLE(self);
    KnHashTable_SetValAtIndex(value, indexNum, val.asU64);
    return self;
}

// self, index, key, val
KN KonTable_SetIndexKv(Kana* kana, KN self, KN index, KN key, KN val)
{
    int indexNum = KN_UNBOX_FIXNUM(index);
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
 

    KnHashTable* value = KN_UNBOX_TABLE(self);
    KnHashTable_SetKvAtIndex(value, indexNum, keyCstr, val.asU64);
    return self;
}

KN KonTable_DelByKey(Kana* kana, KN self, KN key)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));

    KnHashTable* value = KN_UNBOX_TABLE(self);
    KnHashTable_DelByKey(value, keyCstr);
    return self;
}

KN KonTable_DelByIndex(Kana* kana, KN self, KN index)
{
    int indexNum = KN_UNBOX_FIXNUM(index);

    KnHashTable* value = KN_UNBOX_TABLE(self);
    KnHashTable_DelByIndex(value, indexNum);
    return self;
}


KN KonTable_IterHead(Kana* kana, KN self)
{
    KnHashTable* table = KN_UNBOX_TABLE(self);
    
    KnHashTableIter iter = KnHashTable_IterHead(table);
    return KN_BOX_EXT_PTR(iter);
}

KN KonTable_IterTail(Kana* kana, KN self)
{
    KnHashTable* table = KN_UNBOX_TABLE(self);
    
    KnHashTableIter iter = KnHashTable_IterTail(table);
    return KN_BOX_EXT_PTR(iter);
}

KN KonTable_IterPrev(Kana* kana, KN self, KN iter)
{
    KnHashTable* table = KN_UNBOX_TABLE(self);
    
    KnHashTableIter prev = KnHashTable_IterPrev(table, KN_UNBOX_EXT_PTR(iter));
    if (prev != KNBOX_NIL) {
        return KN_BOX_EXT_PTR(prev);
    }
    else {
        return KN_NIL;
    }
}

KN KonTable_IterNext(Kana* kana, KN self, KN iter)
{
    KnHashTable* table = KN_UNBOX_TABLE(self);
    
    KnHashTableIter next = KnHashTable_IterNext(table, KN_UNBOX_EXT_PTR(iter));
    if (next != KNBOX_NIL) {
        return KN_BOX_EXT_PTR(next);
    }
    else {
        return KN_NIL;
    }
}

KN KonTable_IterGetKey(Kana* kana, KN self, KN iter)
{
    KnHashTable* table = KN_UNBOX_TABLE(self);
    
    const char* key = KnHashTable_IterGetKey(table, KN_UNBOX_EXT_PTR(iter));
    KonString* value = KN_NEW_DYNAMIC_OBJ(kana, KonString, KN_T_STRING);
    value->string = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->string, key);
    return KON_2_KN(value);
}

KN KonTable_IterGetVal(Kana* kana, KN self, KN iter)
{
    KnHashTable* table = KN_UNBOX_TABLE(self);
    
    return (KN)KnHashTable_IterGetVal(table, KN_UNBOX_EXT_PTR(iter));
}

NativeExportConf Kmod_Table_Export(Kana* kana)
{
    NativeExportItem exportArr[] = {
        { .type = KN_NATIVE_EXPORT_PROC, .name = "init", .proc = { KN_NATIVE_FUNC, KonTable_Init, 0, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "length", .proc = { KN_NATIVE_FUNC, KonTable_Length, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "clear", .proc = { KN_NATIVE_FUNC, KonTable_Clear, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "has-key", .proc = { KN_NATIVE_FUNC, KonTable_HasKey, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "at-key", .proc = { KN_NATIVE_FUNC, KonTable_AtKey, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "at-idx", .proc = { KN_NATIVE_FUNC, KonTable_ValAtIndex, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "get-idx-key", .proc = { KN_NATIVE_FUNC, KonTable_KeyAtIndex, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "first-val", .proc = { KN_NATIVE_FUNC, KonTable_FirstVal, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "last-val", .proc = { KN_NATIVE_FUNC, KonTable_LastVal, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "push-val", .proc = { KN_NATIVE_FUNC, KonTable_PushVal, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "push-kv", .proc = { KN_NATIVE_FUNC, KonTable_PushKv, 3, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "unshift-val", .proc = { KN_NATIVE_FUNC, KonTable_UnshiftVal, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "unshift-kv", .proc = { KN_NATIVE_FUNC, KonTable_UnshiftKv, 3, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "put-kv", .proc = { KN_NATIVE_FUNC, KonTable_PutKv, 3, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "set-idx-key", .proc = { KN_NATIVE_FUNC, KonTable_SetIndexKey, 3, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "set-idx-val", .proc = { KN_NATIVE_FUNC, KonTable_SetIndexVal, 3, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "set-idx-kv", .proc = { KN_NATIVE_FUNC, KonTable_SetIndexKv, 4, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "del-key", .proc = { KN_NATIVE_FUNC, KonTable_DelByKey, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "del-idx", .proc = { KN_NATIVE_FUNC, KonTable_DelByIndex, 2, 0, 0, 0 } },
    };

    int len = 19;
    NativeExportItem* items = (NativeExportItem*)calloc(len, sizeof(NativeExportItem));
    memcpy(items, exportArr, len * sizeof(NativeExportItem));
    NativeExportConf res = {
        .len = len,
        .items = items
    };
    
    return res;
}
