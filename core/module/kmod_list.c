#include "kmod_list.h"

KN KonList_Init(Kana* kana, KN args)
{
    return args;
}

KN KonList_Length(Kana* kana, KN self)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }
    return KN_PairListLength(kana, self);
}

KN KonList_Push(Kana* kana, KN self, KN other)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }

    if (other.asU64 == KNBOX_NIL) {
        return self;
    }
    KN iter = self;
    while (iter.asU64 != KNBOX_NIL) {
        KN next = KN_CDR(iter);
        if (next.asU64 == KNBOX_NIL) {
            // append
            KonPair* node = KN_NEW_DYNAMIC_OBJ(kana, KonPair, KN_T_PAIR);
            node->body = other;
            node->next = KN_NIL;
            node->prev = iter;
            
            KN_CDR(iter) = KON_2_KN(node);

            break;
        }
        iter = next;
    }

    return self;
}

KN KonList_Unshift(Kana* kana, KN self, KN other)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }

    KonPair* node = KN_NEW_DYNAMIC_OBJ(kana, KonPair, KN_T_PAIR);
    node->body = other;
    node->next = self;
    node->prev = KN_NIL;

    
    return KON_2_KN(node);
}

NativeExportConf Kmod_List_Export(Kana* kana)
{
    NativeExportItem exportArr[] = {
        { .type = KN_NATIVE_EXPORT_PROC, .name = "init", .proc = { KN_NATIVE_FUNC, KonList_Init, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "length", .proc = { KN_NATIVE_FUNC, KonList_Length, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "push", .proc = { KN_NATIVE_FUNC, KonList_Push, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "unshift", .proc = { KN_NATIVE_FUNC, KonList_Unshift, 2, 0, 0, 0 } },
    };

    int len = 4;
    NativeExportItem* items = (NativeExportItem*)calloc(len, sizeof(NativeExportItem));
    memcpy(items, exportArr, len * sizeof(NativeExportItem));
    NativeExportConf res = {
        .len = len,
        .items = items
    };
    
    return res;
}