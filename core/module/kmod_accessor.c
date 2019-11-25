#include "kmod_accessor.h"
#include "../container/kn_vector.h"
#include "../container/kn_hashtable.h"

extern KN UnBoxAccessorValue(KN konValue);

KN KonAccessor_InitProperty(Kana* kana, KN val, KN rest)
{
    KonProcedure* setter = NULL;
    char* modCstr = NULL;
    if (rest.asU64 != KNBOX_NIL) {
        KN mod = KN_CAR(rest);
        if (KN_IS_IDENTIFIER(mod)) {
            modCstr = KN_UNBOX_SYMBOL(mod);
        }
        else if (KN_IS_STRING(mod)) {
            modCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(mod));
        }
        
        if (KN_CDR(rest).asU64 != KNBOX_NIL) {
            setter = KN_2_KON(KN_CADR(rest), Procedure);
        }
    }

    KonAccessor* self = KN_MakePropertyAccessor(kana, val, modCstr, setter);

    return KON_2_KN(self);
}

KN KonAccessor_InitDir(Kana* kana, KN args)
{
    char* mod = NULL;
    if (args.asU64 != KNBOX_NIL && KN_IS_IDENTIFIER(KN_CAR(args))) {
        mod = KN_UNBOX_SYMBOL(KN_CAR(args));
    }
    KonProcedure* setter = NULL;    // TODO
    KonAccessor* self = KN_MakeDirAccessor(kana, mod, setter);

    return KON_2_KN(self);
}

KN KonAccessor_Unbox(Kana* kana, KN self)
{
    return UnBoxAccessorValue(self);
}

KN KonAccessor_SetVal(Kana* kana, KN self, KN assignTo)
{
    KonAccessor* accessor = KN_2_KON(self, Accessor);
    accessor->isDir = false;
    accessor->value = assignTo;

    return self;
}

KN KonAccessor_HasKey(Kana* kana, KN self, KN key)
{
    char* keyCstr = NULL;
    if (KN_IS_SYMBOL(key)) {
        keyCstr = KN_UNBOX_SYMBOL(key);
    }
    else if (KN_IS_STRING(key)) {
        keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    }
    KonAccessor* value = KN_2_KON(self, Accessor);
    KnHashTable* dir = value->dir;
    if (dir == NULL) {
        return KN_FALSE;
    }
    return KnHashTable_HasKey(dir, keyCstr) ? KN_TRUE : KN_FALSE;
}

KN KonAccessor_AtKey(Kana* kana, KN self, KN key)
{
    char* keyCstr = NULL;
    if (KN_IS_SYMBOL(key)) {
        keyCstr = KN_UNBOX_SYMBOL(key);
    }
    else if (KN_IS_STRING(key)) {
        keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    }
    KonAccessor* value = KN_2_KON(self, Accessor);
    KnHashTable* dir = value->dir;
    if (dir == NULL) {
        return KN_UNDEF;
    }

    KN findRes = (KN)KnHashTable_AtKey(dir, keyCstr);
    return findRes;
}

KN KonAccessor_PutKeyVal(Kana* kana, KN self, KN key, KN val, KN rest)
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

    if (rest.asU64 != KNBOX_NIL) {
        setter = KN_2_KON(KN_CAR(rest), Procedure);
    }

    return KN_DirAccessorPutKeyValue(kana, self, keyCstr, val, mod, setter) ? KN_TRUE : KN_FALSE;
}

KN KonAccessor_PutKeyProp(Kana* kana, KN self, KN key, KN property)
{
    char* keyCstr = NULL;
    if (KN_IS_SYMBOL(key)) {
        keyCstr = KN_UNBOX_SYMBOL(key);
    }
    else if (KN_IS_STRING(key)) {
        keyCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(key));
    }

    return KN_DirAccessorPutKeyProperty(kana, self, keyCstr, property) ? KN_TRUE : KN_FALSE;
}

KN KonAccessor_IterHead(Kana* kana, KN self)
{
    KonAccessor* accessor = KN_2_KON(self, Accessor);
    KnHashTable* dir = accessor->dir;
    
    KnHashTableIter iter = KnHashTable_IterHead(dir);
    return KN_BOX_EXT_PTR(iter);
}

KN KonAccessor_IterTail(Kana* kana, KN self)
{
    KonAccessor* accessor = KN_2_KON(self, Accessor);
    KnHashTable* dir = accessor->dir;
    
    KnHashTableIter iter = KnHashTable_IterTail(dir);
    return KN_BOX_EXT_PTR(iter);
}

KN KonAccessor_IterPrev(Kana* kana, KN self, KN iter)
{
    KonAccessor* accessor = KN_2_KON(self, Accessor);
    KnHashTable* dir = accessor->dir;
    
    KnHashTableIter prev = KnHashTable_IterPrev(dir, KN_UNBOX_EXT_PTR(iter));
    if (prev != KNBOX_NIL) {
        return KN_BOX_EXT_PTR(prev);
    }
    else {
        return KN_NIL;
    }
}

KN KonAccessor_IterNext(Kana* kana, KN self, KN iter)
{   
    KonAccessor* accessor = KN_2_KON(self, Accessor);
    KnHashTable* dir = accessor->dir;
    
    KnHashTableIter next = KnHashTable_IterNext(dir, KN_UNBOX_EXT_PTR(iter));
    if (next != KNBOX_NIL) {
        return KN_BOX_EXT_PTR(next);
    }
    else {
        return KN_NIL;
    }
}

KN KonAccessor_IterGetKey(Kana* kana, KN self, KN iter)
{
    KonAccessor* accessor = KN_2_KON(self, Accessor);
    KnHashTable* dir = accessor->dir;
    
    const char* key = KnHashTable_IterGetKey(dir, KN_UNBOX_EXT_PTR(iter));
    KonString* value = KN_NEW_DYNAMIC_OBJ(kana, KonString, KN_T_STRING);
    value->string = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(value->string, key);
    return KON_2_KN(value);
}

KN KonAccessor_IterGetVal(Kana* kana, KN self, KN iter)
{
    KonAccessor* accessor = KN_2_KON(self, Accessor);
    KnHashTable* dir = accessor->dir;
    
    return (KN)KnHashTable_IterGetVal(dir, KN_UNBOX_EXT_PTR(iter));
}

NativeExportConf Kmod_Accessor_Export(Kana* kana)
{
    NativeExportItem exportArr[] = {
        { .type = KN_NATIVE_EXPORT_PROC, .name = "init-prop", .proc = { KN_NATIVE_FUNC, KonAccessor_InitProperty, 1, 1, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "init-dir", .proc = { KN_NATIVE_FUNC, KonAccessor_InitDir, 0, 1, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "unbox", .proc = { KN_NATIVE_FUNC, KonAccessor_Unbox, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "set-val", .proc = { KN_NATIVE_FUNC, KonAccessor_SetVal, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "has-key", .proc = { KN_NATIVE_FUNC, KonAccessor_HasKey, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "at-key", .proc = { KN_NATIVE_FUNC, KonAccessor_AtKey, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "put-key-val", .proc = { KN_NATIVE_FUNC, KonAccessor_PutKeyVal, 3, 1, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "put-key-prop", .proc = { KN_NATIVE_FUNC, KonAccessor_PutKeyProp, 3, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "iter-head", .proc = { KN_NATIVE_FUNC, KonAccessor_IterHead, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "iter-tail", .proc = { KN_NATIVE_FUNC, KonAccessor_IterTail, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "iter-prev", .proc = { KN_NATIVE_FUNC, KonAccessor_IterPrev, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "iter-next", .proc = { KN_NATIVE_FUNC, KonAccessor_IterNext, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "iter-key", .proc = { KN_NATIVE_FUNC, KonAccessor_IterGetKey, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "iter-val", .proc = { KN_NATIVE_FUNC, KonAccessor_IterGetVal, 2, 0, 0, 0 } },
    };

    int len = 14;
    NativeExportItem* items = (NativeExportItem*)calloc(len, sizeof(NativeExportItem));
    memcpy(items, exportArr, len * sizeof(NativeExportItem));
    NativeExportConf res = {
        .len = len,
        .items = items
    };
    
    return res;
}
