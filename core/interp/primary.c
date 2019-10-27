#include "primary.h"

// not(kon_false) => true, not(other) => false
KN KN_PrimaryNot(KonState* kstate, KN item)
{
    if (KN_IS_FALSE(item)) {
        return KN_TRUE;
    }
    else {
        return KN_FALSE;
    }
}

// pointer check
KN KN_PrimaryEqual(KonState* kstate, KN left, KN right)
{
    if (left.asU64 == right.asU64) {
        return KN_TRUE;
    }
    else {
        return KN_FALSE;
    }
}

// basic types, boolean, number, char symbol, string value check
KN KN_PrimaryEqv(KonState* kstate, KN left, KN right)
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

KN KN_PrimaryEq(KonState* kstate, KN left, KN right)
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
        return KN_PrimaryEqv(kstate, left, right);
    }
    else {
        return KN_PrimaryEqual(kstate, left, right);
    }
}

KN KN_PrimaryNeq(KonState* kstate, KN left, KN right)
{
    KN result = KN_PrimaryEq(kstate, left, right);
    if (result.asU64 == KNBOX_FALSE) {
        return KN_TRUE;
    }
    else {
        return KN_FALSE;
    }
}

KN KN_PrimaryNewline(KonState* kstate)
{
    printf("\n");
    return KN_MAKE_FIXNUM(1);
}

KN KN_PrimaryDisplay(KonState* kstate, KN args)
{
    KN iter = args;
    KxStringBuffer* merged = KxStringBuffer_New();

    int state = 1; // 1 need verb, 2 need objects
    do {
        KN item = KN_CAR(iter);

        KN formated = KN_ToFormatString(kstate, item, false, 0, "  ");
        KxStringBuffer_AppendStringBuffer(merged, KN_UNBOX_STRING(formated));

        iter = KN_CDR(iter);
    } while (iter.asU64 != KNBOX_NIL);
    printf("%s", KxStringBuffer_Cstr(merged));
    int size = KxStringBuffer_Length(merged);
    return KN_MAKE_FIXNUM(size);
}

KN KN_PrimaryDisplayln(KonState* kstate, KN args)
{
    KN size = KN_PrimaryDisplay(kstate, args);
    KN_PrimaryNewline(kstate);
    return KN_MAKE_FIXNUM(KN_UNBOX_FIXNUM(size) + 1);
}

// TODO select output
KN KN_PrimaryWrite(KonState* kstate, KN args)
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
            KN formated = KN_ToFormatString(kstate, item, false, 0, "  ");
            KxStringBuffer_AppendStringBuffer(merged, KN_UNBOX_STRING(formated));
        }
        iter = KN_CDR(iter);
    } while (iter.asU64 != KNBOX_NIL);
    printf("%s", KxStringBuffer_Cstr(merged));
    int size = KxStringBuffer_Length(merged);
    return KN_MAKE_FIXNUM(size);
}

// TODO select output
KN KN_PrimaryWriteln(KonState* kstate, KN args)
{
    KN size = KN_PrimaryWrite(kstate, args);
    KN_PrimaryNewline(kstate);
    return KN_MAKE_FIXNUM(KN_UNBOX_FIXNUM(size) + 1);
}

KN KN_PrimaryStringify(KonState* kstate, KN item)
{
    KN formated = KN_ToFormatString(kstate, item, false, 0, "  ");

    return formated;
}

// TODO
KN KN_PrimaryParse(KonState* kstate, KN item)
{
    // return item;
    return KN_UNDEF;
}

KN KN_PrimaryIsTrue(KonState* kstate, KN item)
{
    return (KN_IS_TRUE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsFalse(KonState* kstate, KN item)
{
    return (KN_IS_FALSE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsNil(KonState* kstate, KN item)
{
    return (KN_IS_NIL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsNull(KonState* kstate, KN item)
{
    return (KN_IS_UNDEF(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsUkn(KonState* kstate, KN item)
{
    return (KN_IS_UKN(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsPointer(KonState* kstate, KN item)
{
    return (KN_IS_POINTER(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsFixnum(KonState* kstate, KN item)
{
    return (KN_IS_FIXNUM(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsFlonum(KonState* kstate, KN item)
{
    return (KN_IS_FLONUM(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsChar(KonState* kstate, KN item)
{
    return (KN_IS_CHAR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsBoolean(KonState* kstate, KN item)
{
    return (KN_IS_BOOLEAN(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsBytes(KonState* kstate, KN item)
{
    return (KN_IS_BYTES(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsString(KonState* kstate, KN item)
{
    return (KN_IS_STRING(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsSymbol(KonState* kstate, KN item)
{
    return (KN_IS_SYMBOL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsVariable(KonState* kstate, KN item)
{
    return (KN_IS_VARIABLE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsIdentifier(KonState* kstate, KN item)
{
    return (KN_IS_IDENTIFIER(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsWord(KonState* kstate, KN item)
{
    return (KN_IS_WORD(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsAttrSlot(KonState* kstate, KN item)
{
    return (KN_IS_ACCESSOR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsPair(KonState* kstate, KN item)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(item)) {
        item = KN_UNBOX_QUOTE(item);
    }
    return (KN_IS_PAIR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsPairList(KonState* kstate, KN item)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(item)) {
        item = KN_UNBOX_QUOTE(item);
    }
    return (KN_IsPairList(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsVector(KonState* kstate, KN item)
{
    return (KN_IS_VECTOR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsTable(KonState* kstate, KN item)
{
    return (KN_IS_TABLE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsCell(KonState* kstate, KN item)
{
    return (KN_IS_CELL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsEnv(KonState* kstate, KN item)
{
    return (KN_IS_ENV(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsProcedure(KonState* kstate, KN item)
{
    return (KN_IS_PROCEDURE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsContinuation(KonState* kstate, KN item)
{
    return (KN_IS_CONTINUATION(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsExtPointer(KonState* kstate, KN item)
{
    return (KN_IS_EXT_POINTER(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsException(KonState* kstate, KN item)
{
    return (KN_IS_EXCEPTION(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryVarFromSym(KonState* kstate, KN item)
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
    KonSymbol* value = KN_NEW_CONST_OBJ(kstate, KonSymbol, KN_T_SYMBOL);
    value->type = KN_SYM_VARIABLE;
    value->data = utf8dup(symCstr);
    return KON_2_KN(value);
}

KN KN_PrimaryToIdentifier(KonState* kstate, KN item)
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
    KonSymbol* value = KN_NEW_CONST_OBJ(kstate, KonSymbol, KN_T_SYMBOL);
    value->type = KN_SYM_IDENTIFIER;
    value->data = utf8dup(symCstr);
    return KON_2_KN(value);
}

KN KN_PrimaryToSymString(KonState* kstate, KN item)
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
    KonSymbol* value = KN_NEW_CONST_OBJ(kstate, KonSymbol, KN_T_SYMBOL);
    value->type = KN_SYM_STRING;
    value->data = utf8dup(symCstr);
    return KON_2_KN(value);
}

KN KN_PrimaryUnboxQuote(KonState* kstate, KN obj)
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

KN KN_PrimaryGetDispatcherId(KonState* kstate, KN obj)
{
    return KN_MAKE_FIXNUM(KN_NodeDispacherId(kstate, obj));
}

KN KN_PrimarySetDispatcherId(KonState* kstate, KN obj, KN boxedId)
{
    unsigned int dispatcherId = KN_UNBOX_FIXNUM(boxedId);
    if (KN_IS_POINTER(obj)) {
        KN_FIELD(obj, Base, msgDispatcherId) = dispatcherId;
    }
    return KN_TRUE;
}



KN KonList_CONS(KonState* kstate, KN self, KN other)
{
    return KN_CONS(kstate, self, other);
}

KN KonList_CAR(KonState* kstate, KN self)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }
    return KN_CAR(self);
}

KN KonList_CDR(KonState* kstate, KN self)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }
    return KN_CDR(self);
}

KN KonList_CAAR(KonState* kstate, KN self)
{
    return KN_CAAR(self);
}

KN KonList_CADR(KonState* kstate, KN self)
{
    return KN_CADR(self);
}

KN KonList_CDAR(KonState* kstate, KN self)
{
    return KN_CDAR(self);
}

KN KonList_CDDR(KonState* kstate, KN self)
{
    return KN_CDDR(self);
}

KN KonList_CAAAR(KonState* kstate, KN self)
{
    return KN_CAAAR(self);
}

KN KonList_CAADR(KonState* kstate, KN self)
{
    return KN_CAADR(self);
}

KN KonList_CADAR(KonState* kstate, KN self)
{
    return KN_CADAR(self);
}

KN KonList_CADDR(KonState* kstate, KN self)
{
    return KN_CADDR(self);
}

KN KonList_CDAAR(KonState* kstate, KN self)
{
    return KN_CDAAR(self);
}

KN KonList_CDADR(KonState* kstate, KN self)
{
    return KN_CDADR(self);
}

KN KonList_CDDAR(KonState* kstate, KN self)
{
    return KN_CDDAR(self);
}

KN KonList_CDDDR(KonState* kstate, KN self)
{
    return KN_CDDDR(self);
}

KN KonList_CADDDR(KonState* kstate, KN self)
{
    return KN_CADDDR(self);
}

KN KonList_CDDDDR(KonState* kstate, KN self)
{
    return KN_CDDDDR(self);
}

KN KonCell_DCR(KonState* kstate, KN self)
{
    return KN_DCR(self);
}

KN KonCell_DNR(KonState* kstate, KN self)
{
    return KN_DNR(self);
}

KN KonCell_DPR(KonState* kstate, KN self)
{
    return KN_DPR(self);
}

KN KonCell_DLR(KonState* kstate, KN self)
{
    return KN_DLR(self);
}

KN KonCell_DTR(KonState* kstate, KN self)
{
    return KN_DTR(self);
}

KN KonCell_DCNR(KonState* kstate, KN self)
{
    return KN_DCNR(self);
}

KN KonCell_DTNR(KonState* kstate, KN self)
{
    return KN_DTNR(self);
}

KN KonCell_DLNR(KonState* kstate, KN self)
{
    return KN_DLNR(self);
}

KN KonCell_DCCR(KonState* kstate, KN self)
{
    return KN_DCCR(self);
}

KN KonCell_DTCR(KonState* kstate, KN self)
{
    return KN_DTCR(self);
}

KN KonCell_DLCR(KonState* kstate, KN self)
{
    return KN_DLCR(self);
}

KN KonCell_DCNNR(KonState* kstate, KN self)
{
    return KN_DCNNR(self);
}

KN KonCell_DTNNR(KonState* kstate, KN self)
{
    return KN_DTNNR(self);
}

KN KonCell_DLNNR(KonState* kstate, KN self)
{
    return KN_DLNNR(self);
}

KN KonCell_DCCNR(KonState* kstate, KN self)
{
    return KN_DCCNR(self);
}

KN KonCell_DTCNR(KonState* kstate, KN self)
{
    return KN_DTCNR(self);
}

KN KonCell_DLCNR(KonState* kstate, KN self)
{
    return KN_DLCNR(self);
}


// init internal types dispatcher id
KN KN_PrimaryExportDispacherId(KonState* kstate, KonEnv* env)
{
    KN_EnvDefine(kstate, env, "BooleanDispacher", KN_MAKE_FIXNUM(KN_T_BOOLEAN));
    KN_EnvDefine(kstate, env, "UnknownDispacher", KN_MAKE_FIXNUM(KN_T_UKN));
    KN_EnvDefine(kstate, env, "UndefinedDispacher", KN_MAKE_FIXNUM(KN_T_UNDEF));
    KN_EnvDefine(kstate, env, "SymbolDispacher", KN_MAKE_FIXNUM(KN_T_SYMBOL));
    KN_EnvDefine(kstate, env, "CharDispacher", KN_MAKE_FIXNUM(KN_T_CHAR));
    KN_EnvDefine(kstate, env, "NumberDispacher", KN_MAKE_FIXNUM(KN_T_NUMBER));
    KN_EnvDefine(kstate, env, "StringDispacher", KN_MAKE_FIXNUM(KN_T_STRING));
    KN_EnvDefine(kstate, env, "ListDispacher", KN_MAKE_FIXNUM(KN_T_PAIRLIST));
    KN_EnvDefine(kstate, env, "VectorDispacher", KN_MAKE_FIXNUM(KN_T_VECTOR));
    KN_EnvDefine(kstate, env, "TableDispacher", KN_MAKE_FIXNUM(KN_T_TABLE));
    KN_EnvDefine(kstate, env, "CellDispacher", KN_MAKE_FIXNUM(KN_T_CELL));
    KN_EnvDefine(kstate, env, "AttrSlotDispacher", KN_MAKE_FIXNUM(KN_T_ACCESSOR));

}


KN KN_PrimaryOpExport(KonState* kstate, KonEnv* env)
{
    KN_EnvDefine(kstate, env, "get-env", env);

    KN_PrimaryExportDispacherId(kstate, env);

    NativeProcedureConf defaultFnArr[] = {
        { KN_NATIVE_FUNC, "not", KN_PrimaryNot, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "equal", KN_PrimaryEqual, 2, 0, 0, 0 },
        { KN_NATIVE_FUNC, "eqv", KN_PrimaryEqv, 2, 0, 0, 0 },
        { KN_NATIVE_FUNC, "eq", KN_PrimaryEq, 2, 0, 0 },
        { KN_NATIVE_FUNC, "neq", KN_PrimaryNeq, 2, 0, 0, 0 },
        { KN_NATIVE_FUNC, "newline", KN_PrimaryNewline, 0, 0, 0, 0 },
        { KN_NATIVE_FUNC, "display", KN_PrimaryDisplay, 0, 1, 0, 0 },
        { KN_NATIVE_FUNC, "displayln", KN_PrimaryDisplayln, 0, 1, 0, 0 },
        { KN_NATIVE_FUNC, "write", KN_PrimaryWrite, 0, 1, 0, 0 },
        { KN_NATIVE_FUNC, "writeln", KN_PrimaryWriteln, 0, 1, 0, 0 },
        { KN_NATIVE_FUNC, "stringify", KN_PrimaryStringify, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "kon-from-str", KN_PrimaryParse, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-true", KN_PrimaryIsTrue, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-false", KN_PrimaryIsFalse, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-nil", KN_PrimaryIsNil, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-null", KN_PrimaryIsNull, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-ukn", KN_PrimaryIsUkn, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-pointer", KN_PrimaryIsPointer, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-fixnum", KN_PrimaryIsFixnum, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-flonum", KN_PrimaryIsFlonum, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-char", KN_PrimaryIsChar, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-boolean", KN_PrimaryIsBoolean, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-bytes", KN_PrimaryIsBytes, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-string", KN_PrimaryIsString, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-symbol", KN_PrimaryIsSymbol, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-variable", KN_PrimaryIsVariable, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-identifier", KN_PrimaryIsIdentifier, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-word", KN_PrimaryIsWord, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-attr-slot", KN_PrimaryIsAttrSlot, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-pair", KN_PrimaryIsPair, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-list", KN_PrimaryIsPairList, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-vector", KN_PrimaryIsVector, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-table", KN_PrimaryIsTable, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-cell", KN_PrimaryIsCell, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-env", KN_PrimaryIsEnv, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-procedure", KN_PrimaryIsProcedure, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-continuation", KN_PrimaryIsContinuation, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-ext-pointer", KN_PrimaryIsExtPointer, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "is-exception", KN_PrimaryIsException, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "unbox-quote", KN_PrimaryUnboxQuote, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "get-dispatcher-id", KN_PrimaryGetDispatcherId, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "set-dispatcher-id", KN_PrimarySetDispatcherId, 2, 0, 0, 0 },
        { KN_NATIVE_FUNC, "var-from-sym", KN_PrimaryVarFromSym, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "to-identifier", KN_PrimaryToIdentifier, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "to-sym-string", KN_PrimaryToSymString, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "cons", KonList_CONS, 2, 0, 0, 0 },
        { KN_NATIVE_FUNC, "car", KonList_CAR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "cdr", KonList_CDR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "caar", KonList_CAAR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "cadr", KonList_CADR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "cdar", KonList_CDAR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "cddr", KonList_CDDR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "caaar", KonList_CAAAR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "caadr", KonList_CAADR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "cadar", KonList_CADAR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "caddr", KonList_CADDR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "cdaar", KonList_CDAAR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "cdadr", KonList_CDADR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "cddar", KonList_CDDAR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "cdddr", KonList_CDDDR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "cadddr", KonList_CADDDR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "cddddr", KonList_CDDDDR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dcr", KonCell_DCR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dnr", KonCell_DNR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dpr", KonCell_DPR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dlr", KonCell_DLR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dtr", KonCell_DTR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dcnr", KonCell_DCNR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dtnr", KonCell_DTNR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dlnr", KonCell_DLNR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dccr", KonCell_DCCR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dtcr", KonCell_DTCR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dlcr", KonCell_DLCR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dcnnr", KonCell_DCNNR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dtnnr", KonCell_DTNNR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dlnnr", KonCell_DLNNR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dccnr", KonCell_DCCNR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dtcnr", KonCell_DTCNR, 1, 0, 0, 0 },
        { KN_NATIVE_FUNC, "dlcnr", KonCell_DLCNR, 1, 0, 0, 0 },
    };

    int fnCnt = 79;
    for (int i = 0; i < fnCnt; i++) {
        KN_EnvDefine(kstate, env, defaultFnArr[i].name,
            MakeNativeProcedure(
                kstate,
                defaultFnArr[i].type,
                defaultFnArr[i].funcRef,
                defaultFnArr[i].paramNum,
                defaultFnArr[i].hasVAList,
                defaultFnArr[i].hasVAMap
            )
        );
    }
}