#include "kmod_vector.h"
#include "../container/kn_vector.h"

KN KonVector_Init(Kana* kana)
{
    KonVector* value = KN_NEW_DYNAMIC_OBJ(kana, KonVector, KN_T_VECTOR);
    value->vector = KnVector_Init();
    return KON_2_KN(value);
}

// set capacity, but item num is 0
KN KonVector_InitWithCapacity(Kana* kana, KN capacity)
{
    int capacityNum = KN_UNBOX_FIXNUM(capacity);
    KonVector* value = KN_NEW_DYNAMIC_OBJ(kana, KonVector, KN_T_VECTOR);
    value->vector = KnVector_InitWithCapacity(capacityNum);
    return KON_2_KN(value);
}

// set capacity, and item num is `size`, default value set to KN_UKN
KN KonVector_InitWithSize(Kana* kana, KN size)
{
    int sizeNum = KN_UNBOX_FIXNUM(size);
    KonVector* value = KN_NEW_DYNAMIC_OBJ(kana, KonVector, KN_T_VECTOR);
    value->vector = KnVector_InitWithSize(sizeNum);
    return KON_2_KN(value);
}

KN KonVector_Length(Kana* kana, KN self)
{
    KnVector* value = KN_UNBOX_VECTOR(self);
    return KN_MAKE_FIXNUM(KnVector_Length(value));
}

KN KonVector_Clear(Kana* kana, KN self)
{
    KnVector* value = KN_UNBOX_VECTOR(self);
    KnVector_Clear(value);
    return self;
}

KN KonVector_Push(Kana* kana, KN self, KN val)
{
    KnVector* value = KN_UNBOX_VECTOR(self);
    KnVector_Push(value, val.asU64);
    return self;
}

KN KonVector_Pop(Kana* kana, KN self)
{
    KnVector* value = KN_UNBOX_VECTOR(self);
    return (KN)KnVector_Pop(value);
}

// add val to head
KN KonVector_Unshift(Kana* kana, KN self, KN val)
{
    KnVector* value = KN_UNBOX_VECTOR(self);
    KnVector_Unshift(value, val.asU64);
    return self;
}

KN KonVector_Shift(Kana* kana, KN self)
{
    KnVector* value = KN_UNBOX_VECTOR(self);
    return (KN)KnVector_Shift(value);
}

KN KonVector_AtIndex(Kana* kana, KN self, KN index)
{
    int indexNum = KN_UNBOX_FIXNUM(index);

    KnVector* value = KN_UNBOX_VECTOR(self);
    return (KN)KnVector_AtIndex(value, indexNum);
}

KN KonVector_SetIndex(Kana* kana, KN self, KN index, KN val)
{    
    int indexNum = KN_UNBOX_FIXNUM(index);

    KnVector* value = KN_UNBOX_VECTOR(self);
    KnVector_SetIndex(value, indexNum, val.asU64);
    return self;
}

NativeExportConf Kmod_Vector_Export(Kana* kana)
{
    NativeExportItem exportArr[] = {
        { .type = KN_NATIVE_EXPORT_PROC, .name = "init", .proc = { KN_NATIVE_FUNC, KonVector_Init, 0, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "init-capacity", .proc = { KN_NATIVE_FUNC, KonVector_InitWithCapacity, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "init-size", .proc = { KN_NATIVE_FUNC, KonVector_InitWithSize, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "length", .proc = { KN_NATIVE_FUNC, KonVector_Length, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "clear", .proc = { KN_NATIVE_FUNC, KonVector_Clear, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "push", .proc = { KN_NATIVE_FUNC, KonVector_Push, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "pop", .proc = { KN_NATIVE_FUNC, KonVector_Pop, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "unshift", .proc = { KN_NATIVE_FUNC, KonVector_Unshift, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "shift", .proc = { KN_NATIVE_FUNC, KonVector_Shift, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "at-idx", .proc = { KN_NATIVE_FUNC, KonVector_AtIndex, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "set-idx", .proc = { KN_NATIVE_FUNC, KonVector_SetIndex, 3, 0, 0, 0 } },
    };

    int len = 11;
    NativeExportItem* items = (NativeExportItem*)calloc(len, sizeof(NativeExportItem));
    memcpy(items, exportArr, len * sizeof(NativeExportItem));
    NativeExportConf res = {
        .len = len,
        .items = items
    };
    
    return res;
}
