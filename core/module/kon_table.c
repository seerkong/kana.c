#include "kon_table.h"
#include "../container/kx_hashtable.h"

KN KonTable_Init(KonState* kstate, KN args)
{
    KonTable* value = KN_ALLOC_TYPE_TAG(kstate, KonTable, KN_T_TABLE);
    value->Table = KxHashTable_Init(10);
    return (KN)value;
}

KN KonTable_Length(KonState* kstate, KN args)
{
    KxHashTable* value = KN_UNBOX_TABLE(KN_CAR(args));
    return KN_MAKE_FIXNUM(KxHashTable_Length(value));
}

KN KonTable_Clear(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KxHashTable* value = KN_UNBOX_TABLE(self);
    KxHashTable_Clear(value);
    return self;
}

KN KonTable_HasKey(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN key = KN_CADR(args);
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));

    KxHashTable* value = KN_UNBOX_TABLE(self);
    return KxHashTable_HasKey(value, keyCstr) ? KN_TRUE : KN_FALSE;
}

KN KonTable_AtKey(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN key = KN_CADR(args);
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    KxHashTable* value = KN_UNBOX_TABLE(self);
    return KxHashTable_AtKey(value, keyCstr);
}

KN KonTable_ValAtIndex(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN index = KN_CADR(args);
    int indexNum = KN_UNBOX_FIXNUM(index);
    KxHashTable* value = KN_UNBOX_TABLE(self);
    return KxHashTable_ValAtIndex(value, indexNum);
}

KN KonTable_KeyAtIndex(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN index = KN_CADR(args);
    int indexNum = KN_UNBOX_FIXNUM(index);

    KxHashTable* value = KN_UNBOX_TABLE(self);
    
    const char* key = KxHashTable_KeyAtIndex(value, indexNum);
    KonString* keyStr = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    keyStr->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(keyStr, key);

    return keyStr;
}

KN KonTable_FirstVal(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KxHashTable* value = KN_UNBOX_TABLE(self);
    return KxHashTable_FirstVal(value);
}

KN KonTable_LastVal(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KxHashTable* value = KN_UNBOX_TABLE(self);
    return KxHashTable_LastVal(value);
}

KN KonTable_PushVal(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN val = KN_CADR(args);
    KxHashTable* value = KN_UNBOX_TABLE(self);
    KxHashTable_PushVal(value, val);
    return self;
}

KN KonTable_PushKv(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN key = KN_CADR(args);
    KN val = KN_CADDR(args);

    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    KxHashTable* value = KN_UNBOX_TABLE(self);
    KxHashTable_PushKv(value, keyCstr, val);
    return self;
}

KN KonTable_UnshiftVal(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN val = KN_CADR(args);
    KxHashTable* table = KN_UNBOX_TABLE(self);
    KxHashTable_UnshiftVal(table, val);
    return self;
}

KN KonTable_UnshiftKv(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN key = KN_CADR(args);
    KN val = KN_CADDR(args);

    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    
    KxHashTable* value = KN_UNBOX_TABLE(self);
    KxHashTable_UnshiftKv(value, keyCstr, val);
    return self;
}

KN KonTable_PutKv(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN key = KN_CADR(args);
    KN val = KN_CADDR(args);

    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    
    KxHashTable* value = KN_UNBOX_TABLE(self);
    KxHashTable_PutKv(value, keyCstr, val);
    return self;
}

// self, index, key
KN KonTable_SetIndexKey(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN index = KN_CADR(args);
    KN key = KN_CADDR(args);
    
    int indexNum = KN_UNBOX_FIXNUM(index);
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    

    KxHashTable* value = KN_UNBOX_TABLE(self);
    KxHashTable_SetKeyAtIndex(value, indexNum, keyCstr);
    return self;
}

// self,  index, val,
KN KonTable_SetIndexVal(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN index = KN_CADR(args);
    KN val = KN_CADDR(args);
    
    int indexNum = KN_UNBOX_FIXNUM(index);
    
    KxHashTable* value = KN_UNBOX_TABLE(self);
    KxHashTable_SetValAtIndex(value, indexNum, val);
    return self;
}

// self, index, key, val
KN KonTable_SetIndexKv(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN index = KN_CADR(args);
    KN key = KN_CADDR(args);
    KN val = KN_CADDDR(args);
    
    int indexNum = KN_UNBOX_FIXNUM(index);
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
 

    KxHashTable* value = KN_UNBOX_TABLE(self);
    KxHashTable_SetKvAtIndex(value, indexNum, keyCstr, val);
    return self;
}

KN KonTable_DelByKey(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN key = KN_CADR(args);
    const char* keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
 

    KxHashTable* value = KN_UNBOX_TABLE(self);
    KxHashTable_DelByKey(value, keyCstr);
    return self;
}

KN KonTable_DelByIndex(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN index = KN_CADR(args);
    int indexNum = KN_UNBOX_FIXNUM(index);

    KxHashTable* value = KN_UNBOX_TABLE(self);
    KxHashTable_DelByIndex(value, indexNum);
    return self;
}


KN KonTable_IterHead(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KxHashTable* table = KN_UNBOX_TABLE(self);
    
    KxHashTableIter iter = KxHashTable_IterHead(table);
    return KN_MakeCpointer(kstate, iter);
}

KN KonTable_IterTail(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KxHashTable* table = KN_UNBOX_TABLE(self);
    
    KxHashTableIter iter = KxHashTable_IterTail(table);
    return KN_MakeCpointer(kstate, iter);
}

KN KonTable_IterPrev(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN iter = KN_CADR(args);
    KxHashTable* table = KN_UNBOX_TABLE(self);
    
    KxHashTableIter prev = KxHashTable_IterPrev(table, KN_UNBOX_CPOINTER(iter));
    if ((KN)prev != KN_NIL) {
        return (KN)KN_MakeCpointer(kstate, prev);
    }
    else {
        return KN_NIL;
    }
}

KN KonTable_IterNext(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN iter = KN_CADR(args);
    
    KxHashTable* table = KN_UNBOX_TABLE(self);
    
    KxHashTableIter next = KxHashTable_IterNext(table, KN_UNBOX_CPOINTER(iter));
    if ((KN)next != KN_NIL) {
        return (KN)KN_MakeCpointer(kstate, next);
    }
    else {
        return KN_NIL;
    }
}

KN KonTable_IterGetKey(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN iter = KN_CADR(args);
    KxHashTable* table = KN_UNBOX_TABLE(self);
    
    const char* key = KxHashTable_IterGetKey(table, KN_UNBOX_CPOINTER(iter));
    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->String, key);
    return value;
}

KN KonTable_IterGetVal(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN iter = KN_CADR(args);
    KxHashTable* table = KN_UNBOX_TABLE(self);
    
    return KxHashTable_IterGetVal(table, KN_UNBOX_CPOINTER(iter));
}

KonAccessor* KonTable_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* slot = (KonAccessor*)KN_MakeDirAccessor(kstate, "dr", NULL);
    KxHashTable_PutKv(slot->Dir,
        "init",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_Init)
    );
    KxHashTable_PutKv(slot->Dir,
        "length",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_Length)
    );
    KxHashTable_PutKv(slot->Dir,
        "clear",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_Clear)
    );
    KxHashTable_PutKv(slot->Dir,
        "has-key",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_HasKey)
    );
    KxHashTable_PutKv(slot->Dir,
        "at-key",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_AtKey)
    );
    KxHashTable_PutKv(slot->Dir,
        "at-index",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_ValAtIndex)
    );
    KxHashTable_PutKv(slot->Dir,
        "get-index-key",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_KeyAtIndex)
    );
    KxHashTable_PutKv(slot->Dir,
        "first-val",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_FirstVal)
    );
    KxHashTable_PutKv(slot->Dir,
        "last-val",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_LastVal)
    );
    KxHashTable_PutKv(slot->Dir,
        "push-val",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_PushVal)
    );
    KxHashTable_PutKv(slot->Dir,
        "push-kv",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_PushKv)
    );
    KxHashTable_PutKv(slot->Dir,
        "unshift-val",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_UnshiftVal)
    );
    KxHashTable_PutKv(slot->Dir,
        "unshift-kv",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_UnshiftKv)
    );
    KxHashTable_PutKv(slot->Dir,
        "put-kv",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_PutKv)
    );
    KxHashTable_PutKv(slot->Dir,
        "set-index-key",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_SetIndexKey)
    );
    KxHashTable_PutKv(slot->Dir,
        "set-index-val",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_SetIndexVal)
    );
    KxHashTable_PutKv(slot->Dir,
        "set-index-kv",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_SetIndexKv)
    );
    KxHashTable_PutKv(slot->Dir,
        "del-key",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_DelByKey)
    );
    KxHashTable_PutKv(slot->Dir,
        "del-index",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_DelByIndex)
    );
    
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "iter-head",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_IterHead),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "iter-tail",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_IterTail),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "iter-prev",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_IterPrev),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "iter-next",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_IterNext),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "iter-key",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_IterGetKey),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "iter-val",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonTable_IterGetVal),
        "r",
        NULL
    );
    
    return slot;
}
