#include "kon_table.h"
#include "../../container/kx_hashtable.h"

KN KonTable_Init(KonState* kstate, KN args)
{
    KonTable* value = KON_ALLOC_TYPE_TAG(kstate, KonTable, KON_T_TABLE);
    value->Table = KxHashTable_Init(10);
    return value;
}

KN KonTable_Length(KonState* kstate, KN args)
{
    KxHashTable* value = KON_UNBOX_TABLE(KON_CAR(args));
    return KON_MAKE_FIXNUM(KxHashTable_Length(value));
}

KN KonTable_Clear(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KxHashTable* value = KON_UNBOX_TABLE(self);
    KxHashTable_Clear(value);
    return self;
}

KN KonTable_HasKey(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN key = KON_CADR(args);
    const char* keyCstr = KxStringBuffer_Cstr(KON_UNBOX_STRING(key));

    KxHashTable* value = KON_UNBOX_TABLE(self);
    return KxHashTable_HasKey(value, keyCstr) ? KON_TRUE : KON_FALSE;
}

KN KonTable_AtKey(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN key = KON_CADR(args);
    const char* keyCstr = KxStringBuffer_Cstr(KON_UNBOX_STRING(key));
    KxHashTable* value = KON_UNBOX_TABLE(self);
    return KxHashTable_AtKey(value, keyCstr);
}

KN KonTable_ValAtIndex(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN index = KON_CADR(args);
    int indexNum = KON_UNBOX_FIXNUM(index);
    KxHashTable* value = KON_UNBOX_TABLE(self);
    return KxHashTable_ValAtIndex(value, indexNum);
}

KN KonTable_KeyAtIndex(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN index = KON_CADR(args);
    int indexNum = KON_UNBOX_FIXNUM(index);

    KxHashTable* value = KON_UNBOX_TABLE(self);
    
    const char* key = KxHashTable_KeyAtIndex(value, indexNum);
    KonString* keyStr = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    keyStr->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(keyStr, key);

    return keyStr;
}

KN KonTable_FirstVal(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KxHashTable* value = KON_UNBOX_TABLE(self);
    return KxHashTable_FirstVal(value);
}

KN KonTable_LastVal(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KxHashTable* value = KON_UNBOX_TABLE(self);
    return KxHashTable_LastVal(value);
}

KN KonTable_PushVal(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN val = KON_CADR(args);
    KxHashTable* value = KON_UNBOX_TABLE(self);
    KxHashTable_PushVal(value, val);
    return self;
}

KN KonTable_PushKv(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN key = KON_CADR(args);
    KN val = KON_CADDR(args);

    const char* keyCstr = KxStringBuffer_Cstr(KON_UNBOX_STRING(key));
    KxHashTable* value = KON_UNBOX_TABLE(self);
    KxHashTable_PushKv(value, keyCstr, val);
    return self;
}

KN KonTable_UnshiftVal(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN val = KON_CADR(args);
    KxHashTable* table = KON_UNBOX_TABLE(self);
    KxHashTable_UnshiftVal(table, val);
    return self;
}

KN KonTable_UnshiftKv(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN key = KON_CADR(args);
    KN val = KON_CADDR(args);

    const char* keyCstr = KxStringBuffer_Cstr(KON_UNBOX_STRING(key));
    
    KxHashTable* value = KON_UNBOX_TABLE(self);
    KxHashTable_UnshiftKv(value, keyCstr, val);
    return self;
}

KN KonTable_PutKv(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN key = KON_CADR(args);
    KN val = KON_CADDR(args);

    const char* keyCstr = KxStringBuffer_Cstr(KON_UNBOX_STRING(key));
    
    KxHashTable* value = KON_UNBOX_TABLE(self);
    KxHashTable_PutKv(value, keyCstr, val);
    return self;
}

// self, index, key
KN KonTable_SetIndexKey(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN index = KON_CADR(args);
    KN key = KON_CADDR(args);
    
    int indexNum = KON_UNBOX_FIXNUM(index);
    const char* keyCstr = KxStringBuffer_Cstr(KON_UNBOX_STRING(key));
    

    KxHashTable* value = KON_UNBOX_TABLE(self);
    KxHashTable_SetKeyAtIndex(value, indexNum, keyCstr);
    return self;
}

// self,  index, val,
KN KonTable_SetIndexVal(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN index = KON_CADR(args);
    KN val = KON_CADDR(args);
    
    int indexNum = KON_UNBOX_FIXNUM(index);
    
    KxHashTable* value = KON_UNBOX_TABLE(self);
    KxHashTable_SetValAtIndex(value, indexNum, val);
    return self;
}

// self, index, key, val
KN KonTable_SetIndexKv(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN index = KON_CADR(args);
    KN key = KON_CADDR(args);
    KN val = KON_CADDDR(args);
    
    int indexNum = KON_UNBOX_FIXNUM(index);
    const char* keyCstr = KxStringBuffer_Cstr(KON_UNBOX_STRING(key));
 

    KxHashTable* value = KON_UNBOX_TABLE(self);
    KxHashTable_SetKvAtIndex(value, indexNum, keyCstr, val);
    return self;
}

KN KonTable_DelByKey(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN key = KON_CADR(args);
    const char* keyCstr = KxStringBuffer_Cstr(KON_UNBOX_STRING(key));
 

    KxHashTable* value = KON_UNBOX_TABLE(self);
    KxHashTable_DelByKey(value, keyCstr);
    return self;
}

KN KonTable_DelByIndex(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN index = KON_CADR(args);
    int indexNum = KON_UNBOX_FIXNUM(index);

    KxHashTable* value = KON_UNBOX_TABLE(self);
    KxHashTable_DelByIndex(value, indexNum);
    return self;
}


KN KonTable_IterHead(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KxHashTable* table = KON_UNBOX_TABLE(self);
    
    KxHashTableIter iter = KxHashTable_IterHead(table);
    return KON_MakeCpointer(kstate, iter);
}

KN KonTable_IterTail(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KxHashTable* table = KON_UNBOX_TABLE(self);
    
    KxHashTableIter iter = KxHashTable_IterTail(table);
    return KON_MakeCpointer(kstate, iter);
}

KN KonTable_IterPrev(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN iter = KON_CADR(args);
    KxHashTable* table = KON_UNBOX_TABLE(self);
    
    KxHashTableIter prev = KxHashTable_IterPrev(table, KON_UNBOX_CPOINTER(iter));
    if ((KN)prev != KON_NIL) {
        return (KN)KON_MakeCpointer(kstate, prev);
    }
    else {
        return KON_NIL;
    }
}

KN KonTable_IterNext(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN iter = KON_CADR(args);
    
    KxHashTable* table = KON_UNBOX_TABLE(self);
    
    KxHashTableIter next = KxHashTable_IterNext(table, KON_UNBOX_CPOINTER(iter));
    if ((KN)next != KON_NIL) {
        return (KN)KON_MakeCpointer(kstate, next);
    }
    else {
        return KON_NIL;
    }
}

KN KonTable_IterGetKey(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN iter = KON_CADR(args);
    KxHashTable* table = KON_UNBOX_TABLE(self);
    
    const char* key = KxHashTable_IterGetKey(table, KON_UNBOX_CPOINTER(iter));
    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->String, key);
    return value;
}

KN KonTable_IterGetVal(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN iter = KON_CADR(args);
    KxHashTable* table = KON_UNBOX_TABLE(self);
    
    return KxHashTable_IterGetVal(table, KON_UNBOX_CPOINTER(iter));
}

KonAccessor* KonTable_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* slot = (KonAccessor*)KON_MakeDirAccessor(kstate, "dr", NULL);
    KxHashTable_PutKv(slot->Dir,
        "init",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_Init)
    );
    KxHashTable_PutKv(slot->Dir,
        "length",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_Length)
    );
    KxHashTable_PutKv(slot->Dir,
        "clear",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_Clear)
    );
    KxHashTable_PutKv(slot->Dir,
        "has-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_HasKey)
    );
    KxHashTable_PutKv(slot->Dir,
        "at-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_AtKey)
    );
    KxHashTable_PutKv(slot->Dir,
        "at-index",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_ValAtIndex)
    );
    KxHashTable_PutKv(slot->Dir,
        "get-index-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_KeyAtIndex)
    );
    KxHashTable_PutKv(slot->Dir,
        "first-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_FirstVal)
    );
    KxHashTable_PutKv(slot->Dir,
        "last-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_LastVal)
    );
    KxHashTable_PutKv(slot->Dir,
        "push-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_PushVal)
    );
    KxHashTable_PutKv(slot->Dir,
        "push-kv",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_PushKv)
    );
    KxHashTable_PutKv(slot->Dir,
        "unshift-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_UnshiftVal)
    );
    KxHashTable_PutKv(slot->Dir,
        "unshift-kv",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_UnshiftKv)
    );
    KxHashTable_PutKv(slot->Dir,
        "put-kv",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_PutKv)
    );
    KxHashTable_PutKv(slot->Dir,
        "set-index-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_SetIndexKey)
    );
    KxHashTable_PutKv(slot->Dir,
        "set-index-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_SetIndexVal)
    );
    KxHashTable_PutKv(slot->Dir,
        "set-index-kv",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_SetIndexKv)
    );
    KxHashTable_PutKv(slot->Dir,
        "del-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_DelByKey)
    );
    KxHashTable_PutKv(slot->Dir,
        "del-index",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_DelByIndex)
    );
    
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "iter-head",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_IterHead),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "iter-tail",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_IterTail),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "iter-prev",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_IterPrev),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "iter-next",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_IterNext),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "iter-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_IterGetKey),
        "r",
        NULL
    );
    KON_DirAccessorPutKeyValue(
        kstate,
        slot,
        "iter-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_IterGetVal),
        "r",
        NULL
    );
    
    return slot;
}