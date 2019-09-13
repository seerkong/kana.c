#include "kon_map.h"
#include "../container/kx_hashtable.h"

KN KonMap_Init(KonState* kstate)
{
    KonMap* value = KN_ALLOC_TYPE_TAG(kstate, KonMap, KN_T_MAP);
    value->map = KxHashTable_Init(10);
    return (KN)value;
}

KN KonMap_Length(KonState* kstate, KN self)
{
    KxHashTable* value = KN_UNBOX_MAP(self);
    return KN_MAKE_FIXNUM(KxHashTable_Length(value));
}

KN KonMap_Clear(KonState* kstate, KN self)
{
    KxHashTable* value = KN_UNBOX_MAP(self);
    KxHashTable_Clear(value);
    return self;
}

KN KonMap_HasKey(KonState* kstate, KN self, KN key)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));

    KxHashTable* value = KN_UNBOX_MAP(self);
    return KxHashTable_HasKey(value, keyCstr) ? KN_TRUE : KN_FALSE;
}

KN KonMap_AtKey(KonState* kstate, KN self, KN key)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    KxHashTable* value = KN_UNBOX_MAP(self);
    return KxHashTable_AtKey(value, keyCstr);
}

KN KonMap_PutKv(KonState* kstate, KN self, KN key, KN val)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    
    KxHashTable* value = KN_UNBOX_MAP(self);
    KxHashTable_PutKv(value, keyCstr, val);
    return self;
}

KN KonMap_DelByKey(KonState* kstate, KN self, KN key)
{
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));

    KxHashTable* value = KN_UNBOX_MAP(self);
    KxHashTable_DelByKey(value, keyCstr);
    return self;
}

KN KonMap_IterHead(KonState* kstate, KN self)
{
    KxHashTable* table = KN_UNBOX_MAP(self);
    
    KxHashTableIter iter = KxHashTable_IterHead(table);
    return KN_MAKE_EXT_POINTER(iter);
}

KN KonMap_IterTail(KonState* kstate, KN self)
{
    KxHashTable* table = KN_UNBOX_MAP(self);
    
    KxHashTableIter iter = KxHashTable_IterTail(table);
    return KN_MAKE_EXT_POINTER(iter);
}

KN KonMap_IterPrev(KonState* kstate, KN self, KN iter)
{
    KxHashTable* table = KN_UNBOX_MAP(self);
    
    KxHashTableIter prev = KxHashTable_IterPrev(table, KN_UNBOX_EXT_POINTER(iter));
    if ((KN)prev != KN_NIL) {
        return KN_MAKE_EXT_POINTER(prev);
    }
    else {
        return KN_NIL;
    }
}

KN KonMap_IterNext(KonState* kstate, KN self, KN iter)
{
    KxHashTable* table = KN_UNBOX_MAP(self);
    
    KxHashTableIter next = KxHashTable_IterNext(table, KN_UNBOX_EXT_POINTER(iter));
    if ((KN)next != KN_NIL) {
        return KN_MAKE_EXT_POINTER(next);
    }
    else {
        return KN_NIL;
    }
}

KN KonMap_IterGetKey(KonState* kstate, KN self, KN iter)
{
    KxHashTable* table = KN_UNBOX_MAP(self);
    
    const char* key = KxHashTable_IterGetKey(table, KN_UNBOX_EXT_POINTER(iter));
    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->string = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->string, key);
    return value;
}

KN KonMap_IterGetVal(KonState* kstate, KN self, KN iter)
{
    KxHashTable* table = KN_UNBOX_MAP(self);
    
    return KxHashTable_IterGetVal(table, KN_UNBOX_EXT_POINTER(iter));
}

KonAccessor* KonMap_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* slot = (KonAccessor*)KN_MakeDirAccessor(kstate, "dr", NULL);
    KxHashTable_PutKv(slot->dir,
        "init",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonMap_Init, 0, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "length",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonMap_Length, 1, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "clear",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonMap_Clear, 1, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "has-key",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonMap_HasKey, 2, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "at-key",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonMap_AtKey, 2, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "put-kv",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonMap_PutKv, 3, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "del-key",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonMap_DelByKey, 2, 0, 0)
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "iter-head",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonMap_IterHead, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "iter-tail",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonMap_IterTail, 1, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "iter-prev",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonMap_IterPrev, 2, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "iter-next",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonMap_IterNext, 2, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "iter-key",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonMap_IterGetKey, 2, 0, 0),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "iter-val",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonMap_IterGetVal, 2, 0, 0),
        "r",
        NULL
    );
    
    return slot;
}
