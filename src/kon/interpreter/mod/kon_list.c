#include "kon_list.h"

KN KonList_Init(KonState* kstate, KN args)
{
    return args;
}

KN KonList_Length(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    // unbox QUOTE_LIST
    if (KON_IS_QUOTE(self) && KON_QUOTE_TYPE(self) == KON_QUOTE_LIST) {
        self = KON_UNBOX_QUOTE(self);
    }
    return KON_PairListLength(kstate, self);
}

KN KonList_Append(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    // unbox QUOTE_LIST
    if (KON_IS_QUOTE(self) && KON_QUOTE_TYPE(self) == KON_QUOTE_LIST) {
        self = KON_UNBOX_QUOTE(self);
    }
    KN other = KON_CADR(args);

    if (other == KON_NIL) {
        return self;
    }
    KonPair* iter = self;
    while (iter != KON_NIL) {
        KN next = KON_CDR(iter);
        if (next == KON_NIL) {
            // append
            CAST_Kon(Pair, iter)->Next = other;
            CAST_Kon(Pair, other)->Prev = iter;
            break;
        }
        iter = next;
    }

    return self;
}

KN KonList_Prepend(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    // unbox QUOTE_LIST
    if (KON_IS_QUOTE(self) && KON_QUOTE_TYPE(self) == KON_QUOTE_LIST) {
        self = KON_UNBOX_QUOTE(self);
    }
    KN other = KON_CADR(args);

    if (self == KON_NIL) {
        return other;
    }
    KonPair* iter = other;
    while (iter != KON_NIL) {
        KN next = KON_CDR(iter);
        if (next == KON_NIL) {
            // append
            CAST_Kon(Pair, iter)->Next = self;
            CAST_Kon(Pair, self)->Prev = iter;
            break;
        }
        iter = next;
    }

    return other;
}

KN KonList_Cons(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    KN other = KON_CADR(args);
    return KON_CONS(kstate, self, other);
}

KN KonList_Car(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    // unbox QUOTE_LIST
    if (KON_IS_QUOTE(self) && KON_QUOTE_TYPE(self) == KON_QUOTE_LIST) {
        self = KON_UNBOX_QUOTE(self);
    }
    return KON_CAR(self);
}

KN KonList_Cdr(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    // unbox QUOTE_LIST
    if (KON_IS_QUOTE(self) && KON_QUOTE_TYPE(self) == KON_QUOTE_LIST) {
        self = KON_UNBOX_QUOTE(self);
    }
    return KON_CDR(self);
}

KN KonList_Caar(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CAAR(self);
}

KN KonList_Cadr(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CADR(self);
}

KN KonList_Cdar(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CDAR(self);
}

KN KonList_Cddr(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CDDR(self);
}

KN KonList_Caaar(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CAAAR(self);
}

KN KonList_Caadr(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CAADR(self);
}

KN KonList_Cadar(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CADAR(self);
}

KN KonList_Caddr(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CADDR(self);
}

KN KonList_Cdaar(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CDAAR(self);
}

KN KonList_Cdadr(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CDADR(self);
}

KN KonList_Cddar(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CDDAR(self);
}

KN KonList_Cdddr(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CDDDR(self);
}

KN KonList_Cadddr(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CADDDR(self);
}

KN KonList_Cddddr(KonState* kstate, KN args)
{
    KN self = KON_CAR(args);
    return KON_CDDDDR(self);
}



KonAttrSlot* KonList_Export(KonState* kstate, KonEnv* env)
{
    KON_EnvDefine(kstate, env, "list-init",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Init)
    );

    KON_EnvDefine(kstate, env, "list-append",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Append)
    );

    KON_EnvDefine(kstate, env, "list-prepend",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Prepend)
    );

    KON_EnvDefine(kstate, env, "cons",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Cons)
    );
    KON_EnvDefine(kstate, env, "car",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Car)
    );
    KON_EnvDefine(kstate, env, "cdr",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Cdr)
    );
    KON_EnvDefine(kstate, env, "caar",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Caar)
    );
    KON_EnvDefine(kstate, env, "cadr",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Cadr)
    );
    KON_EnvDefine(kstate, env, "cdar",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Cdar)
    );
    KON_EnvDefine(kstate, env, "cddr",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Cddr)
    );
    KON_EnvDefine(kstate, env, "caaar",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Caaar)
    );
    KON_EnvDefine(kstate, env, "caadr",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Caadr)
    );
    KON_EnvDefine(kstate, env, "cadar",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Cadar)
    );
    KON_EnvDefine(kstate, env, "caddr",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Caddr)
    );
    KON_EnvDefine(kstate, env, "cdaar",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Cdaar)
    );
    KON_EnvDefine(kstate, env, "cdadr",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Cdadr)
    );
    KON_EnvDefine(kstate, env, "cddar",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Cddar)
    );
    KON_EnvDefine(kstate, env, "cdddr",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Cdddr)
    );
    KON_EnvDefine(kstate, env, "cadddr",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Cadddr)
    );
    KON_EnvDefine(kstate, env, "cddddr",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Cddddr)
    );

    KonAttrSlot* slot = (KonAttrSlot*)MakeAttrSlotFolder(kstate, "");

    KxHashTable_PutKv(slot->Folder,
        "new",
        MakeAttrSlotLeaf(kstate,
            MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KonList_Init),
            "p"
        )
    );


    KonAttrSlot* methods = (KonAttrSlot*)MakeAttrSlotFolder(kstate, "");
    
    KxHashTable_PutKv(slot->Folder,
        "methods",
        methods
    );

    KxHashTable_PutKv(methods->Folder,
        "length",
        MakeAttrSlotLeaf(kstate,
            MakeNativeProcedure(kstate, KON_NATIVE_OBJ_METHOD, KonList_Length),
            "pm"
        )
    );
    return slot;
}