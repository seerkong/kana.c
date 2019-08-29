#include "kon_list.h"
#include "../interpreter/env.h"

KN KonList_Init(KonState* kstate, KN args)
{
    return args;
}

KN KonList_Length(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }
    return KN_PairListLength(kstate, self);
}

KN KonList_Push(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }
    KN other = KN_CADR(args);

    if (other == KN_NIL) {
        return self;
    }
    KonPair* iter = self;
    while ((KN)iter != KN_NIL) {
        KN next = KN_CDR(iter);
        if ((KN)next == KN_NIL) {
            // append
            KonPair* node = KN_ALLOC_TYPE_TAG(kstate, KonPair, KN_T_PAIR);
            node->Body = other;
            node->Next = KN_NIL;
            node->Prev = iter;
            
            iter->Next = node;

            break;
        }
        iter = next;
    }

    return self;
}

KN KonList_Unshift(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }
    KN other = KN_CADR(args);
    
    KonPair* node = KN_ALLOC_TYPE_TAG(kstate, KonPair, KN_T_PAIR);
    node->Body = other;
    node->Next = self;
    node->Prev = KN_NIL;

    
    return (KN)node;
}

KN KonList_Cons(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    KN other = KN_CADR(args);
    return KN_CONS(kstate, self, other);
}

KN KonList_Car(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }
    return KN_CAR(self);
}

KN KonList_Cdr(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }
    return KN_CDR(self);
}

KN KonList_Caar(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CAAR(self);
}

KN KonList_Cadr(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CADR(self);
}

KN KonList_Cdar(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CDAR(self);
}

KN KonList_Cddr(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CDDR(self);
}

KN KonList_Caaar(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CAAAR(self);
}

KN KonList_Caadr(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CAADR(self);
}

KN KonList_Cadar(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CADAR(self);
}

KN KonList_Caddr(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CADDR(self);
}

KN KonList_Cdaar(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CDAAR(self);
}

KN KonList_Cdadr(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CDADR(self);
}

KN KonList_Cddar(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CDDAR(self);
}

KN KonList_Cdddr(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CDDDR(self);
}

KN KonList_Cadddr(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CADDDR(self);
}

KN KonList_Cddddr(KonState* kstate, KN args)
{
    KN self = KN_CAR(args);
    return KN_CDDDDR(self);
}



KonAccessor* KonList_Export(KonState* kstate, KonEnv* env)
{


    KN_EnvDefine(kstate, env, "cons",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cons)
    );
    KN_EnvDefine(kstate, env, "car",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Car)
    );
    KN_EnvDefine(kstate, env, "cdr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cdr)
    );
    KN_EnvDefine(kstate, env, "caar",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Caar)
    );
    KN_EnvDefine(kstate, env, "cadr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cadr)
    );
    KN_EnvDefine(kstate, env, "cdar",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cdar)
    );
    KN_EnvDefine(kstate, env, "cddr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cddr)
    );
    KN_EnvDefine(kstate, env, "caaar",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Caaar)
    );
    KN_EnvDefine(kstate, env, "caadr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Caadr)
    );
    KN_EnvDefine(kstate, env, "cadar",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cadar)
    );
    KN_EnvDefine(kstate, env, "caddr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Caddr)
    );
    KN_EnvDefine(kstate, env, "cdaar",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cdaar)
    );
    KN_EnvDefine(kstate, env, "cdadr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cdadr)
    );
    KN_EnvDefine(kstate, env, "cddar",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cddar)
    );
    KN_EnvDefine(kstate, env, "cdddr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cdddr)
    );
    KN_EnvDefine(kstate, env, "cadddr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cadddr)
    );
    KN_EnvDefine(kstate, env, "cddddr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cddddr)
    );

    KonAccessor* slot = (KonAccessor*)KN_MakeDirAccessor(kstate, "", NULL);

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "init",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Init),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "length",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonList_Length),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "push",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonList_Push),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "unshift",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonList_Unshift),
        "r",
        NULL
    );

    return slot;
}
