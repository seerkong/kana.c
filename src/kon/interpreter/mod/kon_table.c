#include "kon_table.h"
#include "../../container/hashtable/kx_hashtable.h"

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

KonAttrSlot* KonTable_Export(KonState* kstate, KonEnv* env)
{
    KON_EnvDefine(kstate, env, "table-init",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_Init)
    );

    KON_EnvDefine(kstate, env, "table-length",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_Length)
    );
    KON_EnvDefine(kstate, env, "table-clear",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_Clear)
    );
    KON_EnvDefine(kstate, env, "table-has-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_HasKey)
    );
    KON_EnvDefine(kstate, env, "table-at-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_AtKey)
    );
    KON_EnvDefine(kstate, env, "table-at-index",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_ValAtIndex)
    );
    KON_EnvDefine(kstate, env, "table-get-index-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_KeyAtIndex)
    );
    KON_EnvDefine(kstate, env, "table-first-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_FirstVal)
    );
    KON_EnvDefine(kstate, env, "table-last-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_LastVal)
    );
    KON_EnvDefine(kstate, env, "table-push-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_PushVal)
    );
    KON_EnvDefine(kstate, env, "table-push-kv",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_PushKv)
    );
    KON_EnvDefine(kstate, env, "table-unshift-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_UnshiftVal)
    );
    KON_EnvDefine(kstate, env, "table-unshift-kv",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_UnshiftKv)
    );
    KON_EnvDefine(kstate, env, "table-put-kv",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_PutKv)
    );
    KON_EnvDefine(kstate, env, "table-set-index-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_SetIndexKey)
    );
    KON_EnvDefine(kstate, env, "table-set-index-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_SetIndexVal)
    );
    KON_EnvDefine(kstate, env, "table-set-index-kv",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_SetIndexKv)
    );
    KON_EnvDefine(kstate, env, "table-del-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_DelByKey)
    );
    KON_EnvDefine(kstate, env, "table-del-index",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonTable_DelByIndex)
    );

    KonAttrSlot* slot = (KonAttrSlot*)MakeAttrSlotFolder(kstate, "");
    return slot;
}