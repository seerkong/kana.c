#include "kon_cell.h"
#include "../container/kx_vector.h"
#include "../container/kx_hashtable.h"
extern KN UnBoxAccessorValue(KN konValue);

KN KonAccessor_InitProperty(KonState* kstate, KN val, KN rest)
{
    KonProcedure* setter = NULL;
    char* modCstr = NULL;
    if (rest != KN_NIL) {
        KN mod = KN_CAR(rest);
        if (KN_IS_IDENTIFIER(mod)) {
            modCstr = KN_UNBOX_SYMBOL(mod);
        }
        else if (KN_IS_STRING(mod)) {
            modCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(mod));
        }
        
        if (KN_CDR(rest) != KN_NIL) {
            setter = KN_CADR(rest);
        }
    }

    KonAccessor* self = KN_MakePropertyAccessor(kstate, val, modCstr, setter);

    return self;
}

KN KonAccessor_InitDir(KonState* kstate, KN args)
{
    char* mod = NULL;
    if (args != KN_NIL && KN_IS_IDENTIFIER(KN_CAR(args))) {
        mod = KN_UNBOX_SYMBOL(KN_CAR(args));
    }
    KonProcedure* setter = NULL;    // TODO
    KonAccessor* self = KN_MakeDirAccessor(kstate, mod, setter);

    return self;
}

KN KonAccessor_Unbox(KonState* kstate, KN self)
{
    return UnBoxAccessorValue(self);
}

KN KonAccessor_SetVal(KonState* kstate, KN self, KN assignTo)
{
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    accessor->isDir = false;
    accessor->value = assignTo;

    return self;
}

KN KonAccessor_HasKey(KonState* kstate, KN self, KN key)
{
    char* keyCstr = NULL;
    if (KN_IS_SYMBOL(key)) {
        keyCstr = KN_UNBOX_SYMBOL(key);
    }
    else if (KN_IS_STRING(key)) {
        keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    }
    KonAccessor* value = CAST_Kon(Accessor, self);
    KxHashTable* dir = value->dir;
    if (dir == NULL) {
        return KN_FALSE;
    }
    return KxHashTable_HasKey(dir, keyCstr) ? KN_TRUE : KN_FALSE;
}

KN KonAccessor_AtKey(KonState* kstate, KN self, KN key)
{
    char* keyCstr = NULL;
    if (KN_IS_SYMBOL(key)) {
        keyCstr = KN_UNBOX_SYMBOL(key);
    }
    else if (KN_IS_STRING(key)) {
        keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    }
    KonAccessor* value = CAST_Kon(Accessor, self);
    KxHashTable* dir = value->dir;
    if (dir == NULL) {
        return KN_UNDEF;
    }

    KN findRes = KxHashTable_AtKey(dir, keyCstr);
    return findRes;
}

KN KonAccessor_PutKeyVal(KonState* kstate, KN self, KN key, KN val, KN rest)
{
    const char* mod = NULL;
    KonProcedure* setter = NULL;
    char* keyCstr = NULL;
    if (KN_IS_SYMBOL(key)) {
        keyCstr = KN_UNBOX_SYMBOL(key);
    }
    else if (KN_IS_STRING(key)) {
        keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    }

    if (rest != KN_NIL) {
        setter = KN_CAR(rest);
    }

    return KN_DirAccessorPutKeyValue(kstate, self, keyCstr, val, mod, setter) ? KN_TRUE : KN_FALSE;
}

KN KonAccessor_PutKeyProp(KonState* kstate, KN self, KN key, KN property)
{
    char* keyCstr = NULL;
    if (KN_IS_SYMBOL(key)) {
        keyCstr = KN_UNBOX_SYMBOL(key);
    }
    else if (KN_IS_STRING(key)) {
        keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    }

    return KN_DirAccessorPutKeyProperty(kstate, self, keyCstr, property) ? KN_TRUE : KN_FALSE;
}

KN KonAccessor_IterHead(KonState* kstate, KN self)
{
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    KxHashTable* dir = accessor->dir;
    
    KxHashTableIter iter = KxHashTable_IterHead(dir);
    return KN_MAKE_EXT_POINTER(iter);
}

KN KonAccessor_IterTail(KonState* kstate, KN self)
{
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    KxHashTable* dir = accessor->dir;
    
    KxHashTableIter iter = KxHashTable_IterTail(dir);
    return KN_MAKE_EXT_POINTER(iter);
}

KN KonAccessor_IterPrev(KonState* kstate, KN self, KN iter)
{
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    KxHashTable* dir = accessor->dir;
    
    KxHashTableIter prev = KxHashTable_IterPrev(dir, KN_UNBOX_EXT_POINTER(iter));
    if ((KN)prev != KN_NIL) {
        return KN_MAKE_EXT_POINTER(prev);
    }
    else {
        return KN_NIL;
    }
}

KN KonAccessor_IterNext(KonState* kstate, KN self, KN iter)
{   
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    KxHashTable* dir = accessor->dir;
    
    KxHashTableIter next = KxHashTable_IterNext(dir, KN_UNBOX_EXT_POINTER(iter));
    if ((KN)next != KN_NIL) {
        return KN_MAKE_EXT_POINTER(next);
    }
    else {
        return KN_NIL;
    }
}

KN KonAccessor_IterGetKey(KonState* kstate, KN self, KN iter)
{
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    KxHashTable* dir = accessor->dir;
    
    const char* key = KxHashTable_IterGetKey(dir, KN_UNBOX_EXT_POINTER(iter));
    KonString* value = KN_ALLOC_TYPE_TAG(kstate, KonString, KN_T_STRING);
    value->string = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->string, key);
    return (KN)value;
}

KN KonAccessor_IterGetVal(KonState* kstate, KN self, KN iter)
{
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    KxHashTable* dir = accessor->dir;
    
    return KxHashTable_IterGetVal(dir, KN_UNBOX_EXT_POINTER(iter));
}

KonAccessor* KonAccessor_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* slot = (KonAccessor*)KN_MakeDirAccessor(kstate, "dr", NULL);
    KxHashTable_PutKv(slot->dir,
        "init-prop",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_InitProperty, 1, 1, 0)
    );

    KxHashTable_PutKv(slot->dir,
        "init-dir",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_InitDir, 0, 1, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "unbox",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_Unbox, 1, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "set-val",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_SetVal, 2, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "has-key",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_HasKey, 2, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "at-key",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_AtKey, 2, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "put-key-val",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_PutKeyVal, 3, 1, 0)
    );

    KxHashTable_PutKv(slot->dir,
        "put-key-prop",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_PutKeyProp, 3, 0, 0)
    );

    KxHashTable_PutKv(slot->dir,
        "iter-head",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_IterHead, 1, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "iter-tail",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_IterTail, 1, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "iter-prev",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_IterPrev, 2, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "iter-next",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_IterNext, 2, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "iter-key",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_IterGetKey, 2, 0, 0)
    );
    KxHashTable_PutKv(slot->dir,
        "iter-val",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonAccessor_IterGetVal, 2, 0, 0)
    );

    return slot;
}
