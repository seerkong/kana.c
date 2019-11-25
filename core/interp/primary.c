#include "primary.h"

// not(kon_false) => true, not(other) => false
KN KN_PrimaryNot(Kana* kana, KN item)
{
    if (KN_IS_FALSE(item)) {
        return KN_TRUE;
    }
    else {
        return KN_FALSE;
    }
}

// pointer check
KN KN_PrimaryEqual(Kana* kana, KN left, KN right)
{
    if (left.asU64 == right.asU64) {
        return KN_TRUE;
    }
    else {
        return KN_FALSE;
    }
}

// basic types, boolean, number, char symbol, string value check
KN KN_PrimaryEqv(Kana* kana, KN left, KN right)
{
    if (left.asU64 == KNBOX_UNDEF || left.asU64 == KNBOX_UKN) {
        return (left.asU64 == right.asU64) ? KN_TRUE: KN_FALSE;
    }
    else if (KN_IS_NIL(left) || KN_IS_NIL(right)) {
        return (KN_IS_NIL(left) && KN_IS_NIL(right)) ? KN_TRUE: KN_FALSE;
    }
    else if (KN_IS_BOOLEAN(left) && KN_IS_BOOLEAN(right)) {
        return (left.asU64 == right.asU64) ? KN_TRUE: KN_FALSE;
    }
    else if (KN_IS_FIXNUM(left) && KN_IS_FIXNUM(right)) {
        return (left.asU64 == right.asU64) ? KN_TRUE: KN_FALSE;
    }
    else if (KN_IS_FLONUM(left) && KN_IS_FLONUM(right)) {
        return (KN_UNBOX_FLONUM(left) == KN_UNBOX_FLONUM(right)) ? KN_TRUE: KN_FALSE;
    }
    else if (KN_IS_CHAR(left) && KN_IS_CHAR(right)) {
        return (KN_UNBOX_CHAR(left) == KN_UNBOX_CHAR(right)) ? KN_TRUE: KN_FALSE;
    }
    // TODO KN_IS_BYTE
    else if (KN_IS_SYMBOL(left) && KN_IS_SYMBOL(right)) {
        const char* leftStr = KN_UNBOX_SYMBOL(left);
        const char* rightStr = KN_UNBOX_SYMBOL(right);
        return (strcmp(leftStr, rightStr) == 0) ? KN_TRUE: KN_FALSE;;
    }
    
    else if (KN_IS_STRING(left) && KN_IS_STRING(right)) {
        const char* leftStr = KxStringBuffer_Cstr(KN_UNBOX_STRING(left));
        const char* rightStr = KxStringBuffer_Cstr(KN_UNBOX_STRING(right));
        return (strcmp(leftStr, rightStr) == 0) ? KN_TRUE: KN_FALSE;;
    }
    else {
        return KN_FALSE;
    }
}

KN KN_PrimaryEq(Kana* kana, KN left, KN right)
{
    if (left.asU64 == KNBOX_UNDEF || left.asU64 == KNBOX_UKN
        || KN_IS_NIL(left)
        || KN_IS_BOOLEAN(left)
        || KN_IS_FIXNUM(left)
        || KN_IS_FLONUM(left)
        || KN_IS_CHAR(left)
        || KN_IS_SYMBOL(left)
        || KN_IS_STRING(left)
    ) {
        return KN_PrimaryEqv(kana, left, right);
    }
    else {
        return KN_PrimaryEqual(kana, left, right);
    }
}

KN KN_PrimaryNeq(Kana* kana, KN left, KN right)
{
    KN result = KN_PrimaryEq(kana, left, right);
    if (result.asU64 == KNBOX_FALSE) {
        return KN_TRUE;
    }
    else {
        return KN_FALSE;
    }
}

KN KN_PrimaryNewline(Kana* kana)
{
    printf("\n");
    return KN_MAKE_FIXNUM(1);
}

KN KN_PrimaryDisplay(Kana* kana, KN args)
{
    KN iter = args;
    KxStringBuffer* merged = KxStringBuffer_New();

    int state = 1; // 1 need verb, 2 need objects
    do {
        KN item = KN_CAR(iter);

        KN formated = KN_ToFormatString(kana, item, false, 0, "  ");
        KxStringBuffer_AppendStringBuffer(merged, KN_UNBOX_STRING(formated));

        iter = KN_CDR(iter);
    } while (iter.asU64 != KNBOX_NIL);
    printf("%s", KxStringBuffer_Cstr(merged));
    int size = KxStringBuffer_Length(merged);
    return KN_MAKE_FIXNUM(size);
}

KN KN_PrimaryDisplayln(Kana* kana, KN args)
{
    KN size = KN_PrimaryDisplay(kana, args);
    KN_PrimaryNewline(kana);
    return KN_MAKE_FIXNUM(KN_UNBOX_FIXNUM(size) + 1);
}

// TODO select output
KN KN_PrimaryWrite(Kana* kana, KN args)
{
    KN iter = args;
    KxStringBuffer* merged = KxStringBuffer_New();

    int state = 1; // 1 need verb, 2 need objects
    do {
        KN item = KN_CAR(iter);

        if (KN_IS_STRING(item)) {
            KxStringBuffer_AppendStringBuffer(merged, KN_UNBOX_STRING(item));
        }
        else {
            KN formated = KN_ToFormatString(kana, item, false, 0, "  ");
            KxStringBuffer_AppendStringBuffer(merged, KN_UNBOX_STRING(formated));
        }
        iter = KN_CDR(iter);
    } while (iter.asU64 != KNBOX_NIL);
    printf("%s", KxStringBuffer_Cstr(merged));
    int size = KxStringBuffer_Length(merged);
    return KN_MAKE_FIXNUM(size);
}

// TODO select output
KN KN_PrimaryWriteln(Kana* kana, KN args)
{
    KN size = KN_PrimaryWrite(kana, args);
    KN_PrimaryNewline(kana);
    return KN_MAKE_FIXNUM(KN_UNBOX_FIXNUM(size) + 1);
}

KN KN_PrimaryStringify(Kana* kana, KN item)
{
    KN formated = KN_ToFormatString(kana, item, false, 0, "  ");

    return formated;
}

// TODO
KN KN_PrimaryParse(Kana* kana, KN item)
{
    // return item;
    return KN_UNDEF;
}

KN KN_PrimaryIsTrue(Kana* kana, KN item)
{
    return (KN_IS_TRUE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsFalse(Kana* kana, KN item)
{
    return (KN_IS_FALSE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsNil(Kana* kana, KN item)
{
    return (KN_IS_NIL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsNull(Kana* kana, KN item)
{
    return (KN_IS_UNDEF(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsUkn(Kana* kana, KN item)
{
    return (KN_IS_UKN(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsPointer(Kana* kana, KN item)
{
    return (KN_IS_POINTER(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsFixnum(Kana* kana, KN item)
{
    return (KN_IS_FIXNUM(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsFlonum(Kana* kana, KN item)
{
    return (KN_IS_FLONUM(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsChar(Kana* kana, KN item)
{
    return (KN_IS_CHAR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsBoolean(Kana* kana, KN item)
{
    return (KN_IS_BOOLEAN(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsBytes(Kana* kana, KN item)
{
    return (KN_IS_BYTES(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsString(Kana* kana, KN item)
{
    return (KN_IS_STRING(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsSymbol(Kana* kana, KN item)
{
    return (KN_IS_SYMBOL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsVariable(Kana* kana, KN item)
{
    return (KN_IS_VARIABLE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsIdentifier(Kana* kana, KN item)
{
    return (KN_IS_IDENTIFIER(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsWord(Kana* kana, KN item)
{
    return (KN_IS_WORD(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsAttrSlot(Kana* kana, KN item)
{
    return (KN_IS_ACCESSOR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsPair(Kana* kana, KN item)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(item)) {
        item = KN_UNBOX_QUOTE(item);
    }
    return (KN_IS_PAIR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsPairList(Kana* kana, KN item)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(item)) {
        item = KN_UNBOX_QUOTE(item);
    }
    return (KN_IsPairList(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsVector(Kana* kana, KN item)
{
    return (KN_IS_VECTOR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsTable(Kana* kana, KN item)
{
    return (KN_IS_TABLE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsCell(Kana* kana, KN item)
{
    return (KN_IS_CELL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsEnv(Kana* kana, KN item)
{
    return (KN_IS_ENV(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsProcedure(Kana* kana, KN item)
{
    return (KN_IS_PROCEDURE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsContinuation(Kana* kana, KN item)
{
    return (KN_IS_CONTINUATION(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsExtPointer(Kana* kana, KN item)
{
    return (KN_IS_EXT_POINTER(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsException(Kana* kana, KN item)
{
    return (KN_IS_EXCEPTION(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryVarFromSym(Kana* kana, KN item)
{
    char* symCstr = NULL;
    if (KN_IS_SYMBOL(item)) {
        symCstr = KN_UNBOX_SYMBOL(item);
    }
    else if (KN_IS_STRING(item)) {
        symCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(item));
    }
    else {
        return KN_UKN;
    }
    KonSymbol* value = KN_NEW_CONST_OBJ(kana, KonSymbol, KN_T_SYMBOL);
    value->type = KN_SYM_VARIABLE;
    value->data = utf8dup(symCstr);
    return KON_2_KN(value);
}

KN KN_PrimaryToIdentifier(Kana* kana, KN item)
{
    char* symCstr = NULL;
    if (KN_IS_SYMBOL(item)) {
        symCstr = KN_UNBOX_SYMBOL(item);
    }
    else if (KN_IS_STRING(item)) {
        symCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(item));
    }
    else {
        return KN_UKN;
    }
    KonSymbol* value = KN_NEW_CONST_OBJ(kana, KonSymbol, KN_T_SYMBOL);
    value->type = KN_SYM_IDENTIFIER;
    value->data = utf8dup(symCstr);
    return KON_2_KN(value);
}

KN KN_PrimaryToSymString(Kana* kana, KN item)
{
    char* symCstr = NULL;
    if (KN_IS_SYMBOL(item)) {
        symCstr = KN_UNBOX_SYMBOL(item);
    }
    else if (KN_IS_STRING(item)) {
        symCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(item));
    }
    else {
        return KN_UKN;
    }
    KonSymbol* value = KN_NEW_CONST_OBJ(kana, KonSymbol, KN_T_SYMBOL);
    value->type = KN_SYM_STRING;
    value->data = utf8dup(symCstr);
    return KON_2_KN(value);
}

KN KN_PrimaryUnboxQuote(Kana* kana, KN obj)
{
    if (KN_IS_QUOTE(obj)) {
        return KN_FIELD(obj, Quote, inner);
    }
    else if (KN_IS_QUASIQUOTE(obj)) {
        return KN_FIELD(obj, Quasiquote, inner);
    }
    else if (KN_IS_UNQUOTE(obj)) {
        return KN_FIELD(obj, Unquote, inner);
    }
    else if (KN_IS_PREFIX(obj)) {
        return KN_FIELD(obj, Prefix, inner);
    }
    else if (KN_IS_SUFFIX(obj)) {
        return KN_FIELD(obj, Suffix, inner);
    }
    return obj;
}

KN KN_PrimaryGetDispatcherId(Kana* kana, KN obj)
{
    return KN_MAKE_FIXNUM(KN_NodeDispacherId(kana, obj));
}

KN KN_PrimarySetDispatcherId(Kana* kana, KN obj, KN boxedId)
{
    unsigned int dispatcherId = KN_UNBOX_FIXNUM(boxedId);
    if (KN_IS_POINTER(obj)) {
        KN_FIELD(obj, Base, msgDispatcherId) = dispatcherId;
    }
    return KN_TRUE;
}



KN KonList_CONS(Kana* kana, KN self, KN other)
{
    return KN_CONS(kana, self, other);
}

KN KonList_CAR(Kana* kana, KN self)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }
    return KN_CAR(self);
}

KN KonList_CDR(Kana* kana, KN self)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }
    return KN_CDR(self);
}

KN KonList_CAAR(Kana* kana, KN self)
{
    return KN_CAAR(self);
}

KN KonList_CADR(Kana* kana, KN self)
{
    return KN_CADR(self);
}

KN KonList_CDAR(Kana* kana, KN self)
{
    return KN_CDAR(self);
}

KN KonList_CDDR(Kana* kana, KN self)
{
    return KN_CDDR(self);
}

KN KonList_CAAAR(Kana* kana, KN self)
{
    return KN_CAAAR(self);
}

KN KonList_CAADR(Kana* kana, KN self)
{
    return KN_CAADR(self);
}

KN KonList_CADAR(Kana* kana, KN self)
{
    return KN_CADAR(self);
}

KN KonList_CADDR(Kana* kana, KN self)
{
    return KN_CADDR(self);
}

KN KonList_CDAAR(Kana* kana, KN self)
{
    return KN_CDAAR(self);
}

KN KonList_CDADR(Kana* kana, KN self)
{
    return KN_CDADR(self);
}

KN KonList_CDDAR(Kana* kana, KN self)
{
    return KN_CDDAR(self);
}

KN KonList_CDDDR(Kana* kana, KN self)
{
    return KN_CDDDR(self);
}

KN KonList_CADDDR(Kana* kana, KN self)
{
    return KN_CADDDR(self);
}

KN KonList_CDDDDR(Kana* kana, KN self)
{
    return KN_CDDDDR(self);
}

KN KonCell_DCR(Kana* kana, KN self)
{
    return KN_DCR(self);
}

KN KonCell_DNR(Kana* kana, KN self)
{
    return KN_DNR(self);
}

KN KonCell_DPR(Kana* kana, KN self)
{
    return KN_DPR(self);
}

KN KonCell_DLR(Kana* kana, KN self)
{
    return KN_DLR(self);
}

KN KonCell_DTR(Kana* kana, KN self)
{
    return KN_DTR(self);
}

KN KonCell_DCNR(Kana* kana, KN self)
{
    return KN_DCNR(self);
}

KN KonCell_DTNR(Kana* kana, KN self)
{
    return KN_DTNR(self);
}

KN KonCell_DLNR(Kana* kana, KN self)
{
    return KN_DLNR(self);
}

KN KonCell_DCCR(Kana* kana, KN self)
{
    return KN_DCCR(self);
}

KN KonCell_DTCR(Kana* kana, KN self)
{
    return KN_DTCR(self);
}

KN KonCell_DLCR(Kana* kana, KN self)
{
    return KN_DLCR(self);
}

KN KonCell_DCNNR(Kana* kana, KN self)
{
    return KN_DCNNR(self);
}

KN KonCell_DTNNR(Kana* kana, KN self)
{
    return KN_DTNNR(self);
}

KN KonCell_DLNNR(Kana* kana, KN self)
{
    return KN_DLNNR(self);
}

KN KonCell_DCCNR(Kana* kana, KN self)
{
    return KN_DCCNR(self);
}

KN KonCell_DTCNR(Kana* kana, KN self)
{
    return KN_DTCNR(self);
}

KN KonCell_DLCNR(Kana* kana, KN self)
{
    return KN_DLCNR(self);
}


// init internal types dispatcher id
KN KN_PrimaryExportDispacherId(Kana* kana, KonEnv* env)
{
    KN_EnvDefine(kana, env, "BooleanDispacher", KN_MAKE_FIXNUM(KN_T_BOOLEAN));
    KN_EnvDefine(kana, env, "UnknownDispacher", KN_MAKE_FIXNUM(KN_T_UKN));
    KN_EnvDefine(kana, env, "UndefinedDispacher", KN_MAKE_FIXNUM(KN_T_UNDEF));
    KN_EnvDefine(kana, env, "SymbolDispacher", KN_MAKE_FIXNUM(KN_T_SYMBOL));
    KN_EnvDefine(kana, env, "CharDispacher", KN_MAKE_FIXNUM(KN_T_CHAR));
    KN_EnvDefine(kana, env, "NumberDispacher", KN_MAKE_FIXNUM(KN_T_NUMBER));
    KN_EnvDefine(kana, env, "StringDispacher", KN_MAKE_FIXNUM(KN_T_STRING));
    KN_EnvDefine(kana, env, "ListDispacher", KN_MAKE_FIXNUM(KN_T_PAIRLIST));
    KN_EnvDefine(kana, env, "VectorDispacher", KN_MAKE_FIXNUM(KN_T_VECTOR));
    KN_EnvDefine(kana, env, "TableDispacher", KN_MAKE_FIXNUM(KN_T_TABLE));
    KN_EnvDefine(kana, env, "CellDispacher", KN_MAKE_FIXNUM(KN_T_CELL));
    KN_EnvDefine(kana, env, "AttrSlotDispacher", KN_MAKE_FIXNUM(KN_T_ACCESSOR));

}


NativeExportConf KN_PrimaryOpExport(Kana* kana, KonEnv* env)
{
    KN_EnvDefine(kana, env, "get-env", env);

    KN_PrimaryExportDispacherId(kana, env);

    NativeExportItem defaultFnArr[] = {
        { .type = KN_NATIVE_EXPORT_PROC, .name = "not", .proc = { KN_NATIVE_FUNC, KN_PrimaryNot, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "equal", .proc = { KN_NATIVE_FUNC, KN_PrimaryEqual, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "eqv", .proc = { KN_NATIVE_FUNC, KN_PrimaryEqv, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "eq", .proc = { KN_NATIVE_FUNC, KN_PrimaryEq, 2, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "neq", .proc = { KN_NATIVE_FUNC, KN_PrimaryNeq, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "newline", .proc = { KN_NATIVE_FUNC, KN_PrimaryNewline, 0, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "display", .proc = { KN_NATIVE_FUNC, KN_PrimaryDisplay, 0, 1, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "displayln", .proc = { KN_NATIVE_FUNC, KN_PrimaryDisplayln, 0, 1, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "write", .proc = { KN_NATIVE_FUNC, KN_PrimaryWrite, 0, 1, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "writeln", .proc = { KN_NATIVE_FUNC, KN_PrimaryWriteln, 0, 1, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "stringify", .proc = { KN_NATIVE_FUNC, KN_PrimaryStringify, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "kon-from-str", .proc = { KN_NATIVE_FUNC, KN_PrimaryParse, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-true", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsTrue, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-false", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsFalse, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-nil", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsNil, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-null", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsNull, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-ukn", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsUkn, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-pointer", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsPointer, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-fixnum", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsFixnum, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-flonum", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsFlonum, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-char", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsChar, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-boolean", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsBoolean, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-bytes", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsBytes, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-string", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsString, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-symbol", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsSymbol, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-variable", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsVariable, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-identifier", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsIdentifier, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-word", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsWord, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-attr-slot", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsAttrSlot, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-pair", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsPair, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-list", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsPairList, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-vector", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsVector, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-table", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsTable, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-cell", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsCell, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-env", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsEnv, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-procedure", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsProcedure, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-continuation", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsContinuation, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-ext-pointer", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsExtPointer, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "is-exception", .proc = { KN_NATIVE_FUNC, KN_PrimaryIsException, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "unbox-quote", .proc = { KN_NATIVE_FUNC, KN_PrimaryUnboxQuote, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "get-dispatcher-id", .proc = { KN_NATIVE_FUNC, KN_PrimaryGetDispatcherId, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "set-dispatcher-id", .proc = { KN_NATIVE_FUNC, KN_PrimarySetDispatcherId, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "var-from-sym", .proc = { KN_NATIVE_FUNC, KN_PrimaryVarFromSym, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "to-identifier", .proc = { KN_NATIVE_FUNC, KN_PrimaryToIdentifier, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "to-sym-string", .proc = { KN_NATIVE_FUNC, KN_PrimaryToSymString, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "cons", .proc = { KN_NATIVE_FUNC, KonList_CONS, 2, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "car", .proc = { KN_NATIVE_FUNC, KonList_CAR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "cdr", .proc = { KN_NATIVE_FUNC, KonList_CDR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "caar", .proc = { KN_NATIVE_FUNC, KonList_CAAR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "cadr", .proc = { KN_NATIVE_FUNC, KonList_CADR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "cdar", .proc = { KN_NATIVE_FUNC, KonList_CDAR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "cddr", .proc = { KN_NATIVE_FUNC, KonList_CDDR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "caaar", .proc = { KN_NATIVE_FUNC, KonList_CAAAR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "caadr", .proc = { KN_NATIVE_FUNC, KonList_CAADR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "cadar", .proc = { KN_NATIVE_FUNC, KonList_CADAR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "caddr", .proc = { KN_NATIVE_FUNC, KonList_CADDR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "cdaar", .proc = { KN_NATIVE_FUNC, KonList_CDAAR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "cdadr", .proc = { KN_NATIVE_FUNC, KonList_CDADR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "cddar", .proc = { KN_NATIVE_FUNC, KonList_CDDAR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "cdddr", .proc = { KN_NATIVE_FUNC, KonList_CDDDR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "cadddr", .proc = { KN_NATIVE_FUNC, KonList_CADDDR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "cddddr", .proc = { KN_NATIVE_FUNC, KonList_CDDDDR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dcr", .proc = { KN_NATIVE_FUNC, KonCell_DCR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dnr", .proc = { KN_NATIVE_FUNC, KonCell_DNR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dpr", .proc = { KN_NATIVE_FUNC, KonCell_DPR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dlr", .proc = { KN_NATIVE_FUNC, KonCell_DLR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dtr", .proc = { KN_NATIVE_FUNC, KonCell_DTR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dcnr", .proc = { KN_NATIVE_FUNC, KonCell_DCNR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dtnr", .proc = { KN_NATIVE_FUNC, KonCell_DTNR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dlnr", .proc = { KN_NATIVE_FUNC, KonCell_DLNR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dccr", .proc = { KN_NATIVE_FUNC, KonCell_DCCR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dtcr", .proc = { KN_NATIVE_FUNC, KonCell_DTCR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dlcr", .proc = { KN_NATIVE_FUNC, KonCell_DLCR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dcnnr", .proc = { KN_NATIVE_FUNC, KonCell_DCNNR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dtnnr", .proc = { KN_NATIVE_FUNC, KonCell_DTNNR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dlnnr", .proc = { KN_NATIVE_FUNC, KonCell_DLNNR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dccnr", .proc = { KN_NATIVE_FUNC, KonCell_DCCNR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dtcnr", .proc = { KN_NATIVE_FUNC, KonCell_DTCNR, 1, 0, 0, 0 } },
        { .type = KN_NATIVE_EXPORT_PROC, .name = "dlcnr", .proc = { KN_NATIVE_FUNC, KonCell_DLCNR, 1, 0, 0, 0 } },
    };

    int len = 79;
    NativeExportItem* items = (NativeExportItem*)calloc(len, sizeof(NativeExportItem));
    memcpy(items, defaultFnArr, len * sizeof(NativeExportItem));
    NativeExportConf res = {
        .len = len,
        .items = items
    };
    
    return res;
}