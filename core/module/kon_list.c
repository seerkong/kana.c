#include "kon_list.h"
#include "../interpreter/env.h"

KN KonList_Init(KonState* kstate, KN args)
{
    return args;
}

KN KonList_Length(KonState* kstate, KN self)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }
    return KN_PairListLength(kstate, self);
}

KN KonList_Push(KonState* kstate, KN self, KN other)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }

    if (other == KN_NIL) {
        return self;
    }
    KonPair* iter = self;
    while ((KN)iter != KN_NIL) {
        KN next = KN_CDR(iter);
        if ((KN)next == KN_NIL) {
            // append
            KonPair* node = KN_ALLOC_TYPE_TAG(kstate, KonPair, KN_T_PAIR);
            node->body = other;
            node->next = KN_NIL;
            node->prev = iter;
            
            iter->next = node;

            break;
        }
        iter = next;
    }

    return self;
}

KN KonList_Unshift(KonState* kstate, KN self, KN other)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }

    KonPair* node = KN_ALLOC_TYPE_TAG(kstate, KonPair, KN_T_PAIR);
    node->body = other;
    node->next = self;
    node->prev = KN_NIL;

    
    return (KN)node;
}

KonAccessor* KonList_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* slot = (KonAccessor*)KN_MakeDirAccessor(kstate, "", NULL);

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "init",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Init, 0, 1, 0),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "length",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonList_Length, 1, 0, 0),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "push",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonList_Push, 2, 0, 0),
        "r",
        NULL
    );

    KN_DirAccessorPutKeyValue(
        kstate,
        (KN)slot,
        "unshift",
        MakeNativeProcedure(kstate, KN_NATIVE_OBJ_METHOD, KonList_Unshift, 2, 0, 0),
        "r",
        NULL
    );

    return slot;
}
