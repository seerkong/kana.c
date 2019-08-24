#include "kon_cell.h"
#include "../../container/kx_vector.h"
#include "../../container/kx_hashtable.h"
extern KN UnBoxAccessorValue(KN konValue);

KN KonAccessor_InitProperty(KonState* kstate, KN args)
{
    KN val = KON_CAR(args);

    KonProcedure* setter = NULL;
    char* modCstr = NULL;
    if (KON_CDR(args) != KON_NIL) {
        KN mod = KON_CADR(args);
        if (KON_IS_IDENTIFIER(mod)) {
            modCstr = KON_UNBOX_SYMBOL(mod);
        }
        else if (KON_IS_STRING(mod)) {
            modCstr = KxStringBuffer_Cstr(KON_UNBOX_STRING(mod));
        }
        
        if (KON_CDDR(args) != KON_NIL) {
            setter = KON_CADDR(args);
        }
    }

    KonAccessor* self = KON_MakePropertyAccessor(kstate, val, modCstr, setter);

    return self;
}

KN KonAccessor_InitDir(KonState* kstate, KN args)
{
    char* mod = NULL;
    if (args != KON_NIL && KON_IS_IDENTIFIER(KON_CAR(args))) {
        mod = KON_UNBOX_SYMBOL(KON_CAR(args));
    }
    KonProcedure* setter = NULL;    // TODO
    KonAccessor* self = KON_MakeDirAccessor(kstate, mod, setter);

    return self;
}

KN KonAccessor_Unbox(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    
    return UnBoxAccessorValue(self);
}

KN KonAccessor_SetVal(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN assignTo = KON_CADR(args);
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    accessor->IsDir = false;
    accessor->Value = assignTo;

    return self;
}

KN KonAccessor_HasKey(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    // key should be a symbol
    KN key = KON_CADR(args);
    char* keyCstr = NULL;
    if (KON_IS_SYMBOL(key)) {
        keyCstr = KON_UNBOX_SYMBOL(key);
    }
    else if (KON_IS_STRING(key)) {
        keyCstr = KxStringBuffer_Cstr(KON_UNBOX_STRING(key));
    }
    KonAccessor* value = CAST_Kon(Accessor, self);
    KxHashTable* dir = value->Dir;
    if (dir == NULL) {
        return KON_FALSE;
    }
    return KxHashTable_HasKey(dir, keyCstr) ? KON_TRUE : KON_FALSE;
}

KN KonAccessor_AtKey(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN key = KON_CADR(args);
    char* keyCstr = NULL;
    if (KON_IS_SYMBOL(key)) {
        keyCstr = KON_UNBOX_SYMBOL(key);
    }
    else if (KON_IS_STRING(key)) {
        keyCstr = KxStringBuffer_Cstr(KON_UNBOX_STRING(key));
    }
    KonAccessor* value = CAST_Kon(Accessor, self);
    KxHashTable* dir = value->Dir;
    if (dir == NULL) {
        return KON_UNDEF;
    }

    KN findRes = KxHashTable_AtKey(dir, keyCstr);
    return findRes;
}

KN KonAccessor_PutKeyVal(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN key = KON_CADR(args);
    KN val = KON_CADDR(args);
    const char* mod = NULL;
    KonProcedure* setter = NULL;
    char* keyCstr = NULL;
    if (KON_IS_SYMBOL(key)) {
        keyCstr = KON_UNBOX_SYMBOL(key);
    }
    else if (KON_IS_STRING(key)) {
        keyCstr = KxStringBuffer_Cstr(KON_UNBOX_STRING(key));
    }

    if (KON_CDDDR(args) != KON_NIL) {
        setter = KON_CADDDR(args);
    }

    return KON_DirAccessorPutKeyValue(kstate, self, keyCstr, val, mod, setter);
}

KN KonAccessor_PutKeyProp(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN key = KON_CADR(args);
    KN property = KON_CADDR(args);

    char* keyCstr = NULL;
    if (KON_IS_SYMBOL(key)) {
        keyCstr = KON_UNBOX_SYMBOL(key);
    }
    else if (KON_IS_STRING(key)) {
        keyCstr = KxStringBuffer_Cstr(KON_UNBOX_STRING(key));
    }

    return KON_DirAccessorPutKeyProperty(kstate, self, keyCstr, property);
}

KN KonAccessor_IterHead(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    KxHashTable* dir = accessor->Dir;
    
    KxHashTableIter iter = KxHashTable_IterHead(dir);
    return (KN)KON_MakeCpointer(kstate, iter);
}

KN KonAccessor_IterTail(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    KxHashTable* dir = accessor->Dir;
    
    KxHashTableIter iter = KxHashTable_IterTail(dir);
    return (KN)KON_MakeCpointer(kstate, iter);
}

KN KonAccessor_IterPrev(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN iter = KON_CADR(args);
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    KxHashTable* dir = accessor->Dir;
    
    KxHashTableIter prev = KxHashTable_IterPrev(dir, KON_UNBOX_CPOINTER(iter));
    if ((KN)prev != KON_NIL) {
        return (KN)KON_MakeCpointer(kstate, prev);
    }
    else {
        return KON_NIL;
    }
}

KN KonAccessor_IterNext(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN iter = KON_CADR(args);
    
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    KxHashTable* dir = accessor->Dir;
    
    KxHashTableIter next = KxHashTable_IterNext(dir, KON_UNBOX_CPOINTER(iter));
    if ((KN)next != KON_NIL) {
        return (KN)KON_MakeCpointer(kstate, next);
    }
    else {
        return KON_NIL;
    }
}

KN KonAccessor_IterGetKey(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN iter = KON_CADR(args);
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    KxHashTable* dir = accessor->Dir;
    
    const char* key = KxHashTable_IterGetKey(dir, KON_UNBOX_CPOINTER(iter));
    KonString* value = KON_ALLOC_TYPE_TAG(kstate, KonString, KON_T_STRING);
    value->String = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->String, key);
    return (KN)value;
}

KN KonAccessor_IterGetVal(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN iter = KON_CADR(args);
    KonAccessor* accessor = CAST_Kon(Accessor, self);
    KxHashTable* dir = accessor->Dir;
    
    return KxHashTable_IterGetVal(dir, KON_UNBOX_CPOINTER(iter));
}

KonAccessor* KonAccessor_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* slot = (KonAccessor*)KON_MakeDirAccessor(kstate, "dr", NULL);
    KxHashTable_PutKv(slot->Dir,
        "init-prop",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_InitProperty)
    );

    KxHashTable_PutKv(slot->Dir,
        "init-dir",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_InitDir)
    );
    KxHashTable_PutKv(slot->Dir,
        "unbox",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_Unbox)
    );
    KxHashTable_PutKv(slot->Dir,
        "set-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_SetVal)
    );
    KxHashTable_PutKv(slot->Dir,
        "has-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_HasKey)
    );
    KxHashTable_PutKv(slot->Dir,
        "at-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_AtKey)
    );
    KxHashTable_PutKv(slot->Dir,
        "put-key-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_PutKeyVal)
    );

    KxHashTable_PutKv(slot->Dir,
        "put-key-prop",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_PutKeyProp)
    );

    KxHashTable_PutKv(slot->Dir,
        "iter-head",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_IterHead)
    );
    KxHashTable_PutKv(slot->Dir,
        "iter-tail",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_IterTail)
    );
    KxHashTable_PutKv(slot->Dir,
        "iter-prev",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_IterPrev)
    );
    KxHashTable_PutKv(slot->Dir,
        "iter-next",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_IterNext)
    );
    KxHashTable_PutKv(slot->Dir,
        "iter-key",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_IterGetKey)
    );
    KxHashTable_PutKv(slot->Dir,
        "iter-val",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonAccessor_IterGetVal)
    );

    return slot;
}
