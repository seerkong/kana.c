#include "kon_vector.h"
#include "../../container/vector/kx_vector.h"

KN KonVector_Init(KonState* kstate, KN args)
{
    KonVector* value = KON_ALLOC_TYPE_TAG(kstate, KonVector, KON_T_VECTOR);
    value->Vector = KxVector_Init();
    return value;
}

// set capacity, but item num is 0
KN KonVector_InitWithCapacity(KonState* kstate, KN args)
{
    KN capacity = KON_CAR(args);
    int capacityNum = KON_UNBOX_FIXNUM(capacity);
    KonVector* value = KON_ALLOC_TYPE_TAG(kstate, KonVector, KON_T_VECTOR);
    value->Vector = KxVector_InitWithCapacity(capacityNum);
    return value;
}

// set capacity, and item num is `size`, default value set to KON_UKN
KN KonVector_InitWithSize(KonState* kstate, KN args)
{
    KN size = KON_CAR(args);
    int sizeNum = KON_UNBOX_FIXNUM(size);
    KonVector* value = KON_ALLOC_TYPE_TAG(kstate, KonVector, KON_T_VECTOR);
    value->Vector = KxVector_InitWithSize(sizeNum);
    return value;
}

KN KonVector_Length(KonState* kstate, KN args)
{
    KxVector* value = KON_UNBOX_VECTOR(KON_CAR(args));
    return KON_MAKE_FIXNUM(KxVector_Length(value));
}

KN KonVector_Clear(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KxVector* value = KON_UNBOX_VECTOR(self);
    KxVector_Clear(value);
    return self;
}

KN KonVector_Push(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN val = KON_CADR(args);
    KxVector* value = KON_UNBOX_VECTOR(self);
    KxVector_Push(value, val);
    return self;
}

KN KonVector_Pop(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KxVector* value = KON_UNBOX_VECTOR(self);
    return KxVector_Pop(value);
}

// add val to head
KN KonVector_Unshift(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN val = KON_CADR(args);
    KxVector* value = KON_UNBOX_VECTOR(self);
    KxVector_Unshift(value, val);
    return self;
}

KN KonVector_Shift(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KxVector* value = KON_UNBOX_VECTOR(self);
    return KxVector_Shift(value);
}

KN KonVector_AtIndex(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN index = KON_CADR(args);
    int indexNum = KON_UNBOX_FIXNUM(index);

    KxVector* value = KON_UNBOX_VECTOR(self);
    return KxVector_AtIndex(value, indexNum);
}

KN KonVector_SetIndex(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN index = KON_CADR(args);
    KN val = KON_CADDR(args);
    
    int indexNum = KON_UNBOX_FIXNUM(index);

    KxVector* value = KON_UNBOX_VECTOR(self);
    KxVector_SetIndex(value, indexNum, val);
    return self;
}

KonAttrSlot* KonVector_Export(KonState* kstate, KonEnv* env)
{
    KON_EnvDefine(kstate, env, "vector-init",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonVector_Init)
    );
    KON_EnvDefine(kstate, env, "vector-init-capacity",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonVector_InitWithCapacity)
    );
    KON_EnvDefine(kstate, env, "vector-init-size",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonVector_InitWithSize)
    );
    KON_EnvDefine(kstate, env, "vector-length",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonVector_Length)
    );
    KON_EnvDefine(kstate, env, "vector-clear",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonVector_Clear)
    );
    KON_EnvDefine(kstate, env, "vector-push",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonVector_Push)
    );
    KON_EnvDefine(kstate, env, "vector-pop",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonVector_Pop)
    );
    KON_EnvDefine(kstate, env, "vector-unshift",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonVector_Unshift)
    );
    KON_EnvDefine(kstate, env, "vector-shift",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonVector_Shift)
    );
    KON_EnvDefine(kstate, env, "vector-at-index",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonVector_AtIndex)
    );
    KON_EnvDefine(kstate, env, "vector-set-index",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonVector_SetIndex)
    );

    KonAttrSlot* slot = (KonAttrSlot*)MakeAttrSlotFolder(kstate, "");
    return slot;
}