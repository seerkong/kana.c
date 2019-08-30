#include "kon_vector.h"
#include "../container/kx_vector.h"

KN KonVector_Init(KonState* kstate, KN args)
{
    KonVector* value = KN_ALLOC_TYPE_TAG(kstate, KonVector, KN_T_VECTOR);
    value->Vector = KxVector_Init();
    return value;
}

// set capacity, but item num is 0
KN KonVector_InitWithCapacity(KonState* kstate, KN args)
{
    KN capacity = KN_CAR(args);
    int capacityNum = KN_UNBOX_FIXNUM(capacity);
    KonVector* value = KN_ALLOC_TYPE_TAG(kstate, KonVector, KN_T_VECTOR);
    value->Vector = KxVector_InitWithCapacity(capacityNum);
    return value;
}

// set capacity, and item num is `size`, default value set to KN_UKN
KN KonVector_InitWithSize(KonState* kstate, KN args)
{
    KN size = KN_CAR(args);
    int sizeNum = KN_UNBOX_FIXNUM(size);
    KonVector* value = KN_ALLOC_TYPE_TAG(kstate, KonVector, KN_T_VECTOR);
    value->Vector = KxVector_InitWithSize(sizeNum);
    return value;
}

KN KonVector_Length(KonState* kstate, KN args)
{
    KxVector* value = KN_UNBOX_VECTOR(KN_CAR(args));
    return KN_MAKE_FIXNUM(KxVector_Length(value));
}

KN KonVector_Clear(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KxVector* value = KN_UNBOX_VECTOR(self);
    KxVector_Clear(value);
    return self;
}

KN KonVector_Push(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN val = KN_CADR(args);
    KxVector* value = KN_UNBOX_VECTOR(self);
    KxVector_Push(value, val);
    return self;
}

KN KonVector_Pop(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KxVector* value = KN_UNBOX_VECTOR(self);
    return KxVector_Pop(value);
}

// add val to head
KN KonVector_Unshift(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN val = KN_CADR(args);
    KxVector* value = KN_UNBOX_VECTOR(self);
    KxVector_Unshift(value, val);
    return self;
}

KN KonVector_Shift(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KxVector* value = KN_UNBOX_VECTOR(self);
    return KxVector_Shift(value);
}

KN KonVector_AtIndex(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN index = KN_CADR(args);
    int indexNum = KN_UNBOX_FIXNUM(index);

    KxVector* value = KN_UNBOX_VECTOR(self);
    return KxVector_AtIndex(value, indexNum);
}

KN KonVector_SetIndex(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN index = KN_CADR(args);
    KN val = KN_CADDR(args);
    
    int indexNum = KN_UNBOX_FIXNUM(index);

    KxVector* value = KN_UNBOX_VECTOR(self);
    KxVector_SetIndex(value, indexNum, val);
    return self;
}

KonAccessor* KonVector_Export(KonState* kstate, KonEnv* env)
{

    KonAccessor* slot = (KonAccessor*)KN_MakeDirAccessor(kstate, "dr", NULL);
    
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "init",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonVector_Init),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "init-capacity",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonVector_InitWithCapacity),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "init-size",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonVector_InitWithSize),
        "r",
        NULL
    );
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "length",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonVector_Length),
        "r",
        NULL
    );
    
    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "clear",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonVector_Length),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "push",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonVector_Push),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "pop",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonVector_Pop),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "unshift",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonVector_Unshift),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "shift",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonVector_Shift),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "at-i",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonVector_AtIndex),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "set-i",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonVector_SetIndex),
        "r",
        NULL
    );
    return slot;
}