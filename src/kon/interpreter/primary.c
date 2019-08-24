#include "primary.h"

// not(kon_false) => true, not(other) => false
KN KN_PrimaryNot(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    if (KN_IS_FALSE(item)) {
        return KN_TRUE;
    }
    else {
        return KN_FALSE;
    }
}

// pointer check
KN KN_PrimaryEqual(KonState* kstate, KN args)
{
    KN left = KN_CAR(args);
    KN right = KN_CADR(args);
    if (left == right) {
        return KN_TRUE;
    }
    else {
        return KN_FALSE;
    }
}

// basic types, boolean, number, char symbol, string value check
KN KN_PrimaryEqv(KonState* kstate, KN args)
{
    KN left = KN_CAR(args);
    KN right = KN_CADR(args);
    if (left == KN_UNDEF || left == KN_UKN) {
        return (left == right) ? KN_TRUE: KN_FALSE;
    }
    else if (KN_IS_NIL(left) || KN_IS_NIL(right)) {
        return (KN_IS_NIL(left) && KN_IS_NIL(right)) ? KN_TRUE: KN_FALSE;
    }
    else if (KN_IS_BOOLEAN(left) && KN_IS_BOOLEAN(right)) {
        return (left == right) ? KN_TRUE: KN_FALSE;
    }
    else if (KN_IS_FIXNUM(left) && KN_IS_FIXNUM(right)) {
        return (left == right) ? KN_TRUE: KN_FALSE;
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

KN KN_PrimaryEq(KonState* kstate, KN args)
{
    KN left = KN_CAR(args);
    if (left == KN_UNDEF || left == KN_UKN
        || KN_IS_NIL(left)
        || KN_IS_BOOLEAN(left)
        || KN_IS_FIXNUM(left)
        || KN_IS_FLONUM(left)
        || KN_IS_CHAR(left)
        || KN_IS_SYMBOL(left)
        || KN_IS_STRING(left)
    ) {
        return KN_PrimaryEqv(kstate, args);
    }
    else {
        return KN_PrimaryEqual(kstate, args);
    }
}

KN KN_PrimaryNeq(KonState* kstate, KN args)
{
    KN result = KN_PrimaryEq(kstate, args);
    if (result == KN_FALSE) {
        return KN_TRUE;
    }
    else {
        return KN_FALSE;
    }
}

KN KN_PrimaryNewline(KonState* kstate, KN args)
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
    } while (iter != KN_NIL);
    printf("%s", KxStringBuffer_Cstr(merged));
    int size = KxStringBuffer_Length(merged);
    return KN_MAKE_FIXNUM(size);
}

KN KN_PrimaryDisplayln(KonState* kstate, KN args)
{
    KN size = KN_PrimaryDisplay(kstate, args);
    KN_PrimaryNewline(kstate, args);
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
    } while (iter != KN_NIL);
    printf("%s", KxStringBuffer_Cstr(merged));
    int size = KxStringBuffer_Length(merged);
    return KN_MAKE_FIXNUM(size);
}

// TODO select output
KN KN_PrimaryWriteln(KonState* kstate, KN args)
{
    KN size = KN_PrimaryWrite(kstate, args);
    KN_PrimaryNewline(kstate, args);
    return KN_MAKE_FIXNUM(KN_UNBOX_FIXNUM(size) + 1);
}

KN KN_PrimaryStringify(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    KN formated = KN_ToFormatString(kstate, item, false, 0, "  ");

    return formated;
}

// TODO
KN KN_PrimaryParse(KonState* kstate, KN args)
{
    // KN item = KN_CAR(args);
    
    // return item;
    return KN_UNDEF;
}

KN KN_PrimaryIsTrue(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_TRUE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsFalse(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_FALSE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsNil(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_NIL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsNull(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_UNDEF(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsUkn(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_UKN(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsPointer(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_POINTER(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsFixnum(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_FIXNUM(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsFlonum(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_FLONUM(item)) ? KN_TRUE : KN_FALSE;
}


KN KN_PrimaryIsImdtSymbol(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_IMDT_SYMBOL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsChar(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_CHAR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsBoolean(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_BOOLEAN(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsBytes(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_BYTES(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsString(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_STRING(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsSymbol(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_SYMBOL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsVariable(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_VARIABLE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsIdentifier(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_IDENTIFIER(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsWord(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_WORD(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsAttrSlot(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_ACCESSOR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsPair(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(item)) {
        item = KN_UNBOX_QUOTE(item);
    }
    return (KN_IS_PAIR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsPairList(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(item)) {
        item = KN_UNBOX_QUOTE(item);
    }
    return (KN_IsPairList(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsVector(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_VECTOR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsTable(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_TABLE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsCell(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_CELL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsEnv(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_ENV(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsProcedure(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_PROCEDURE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsContinuation(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_CONTINUATION(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsCpointer(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_CPOINTER(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsException(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
    return (KN_IS_EXCEPTION(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryToVariable(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
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
    KonSymbol* value = KN_ALLOC_TYPE_TAG(kstate, KonSymbol, KN_T_SYMBOL);
    value->Type = KN_SYM_VARIABLE;
    value->Data = utf8dup(symCstr);
    return value;
}

KN KN_PrimaryToIdentifier(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
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
    KonSymbol* value = KN_ALLOC_TYPE_TAG(kstate, KonSymbol, KN_T_SYMBOL);
    value->Type = KN_SYM_IDENTIFIER;
    value->Data = utf8dup(symCstr);
    return value;
}

KN KN_PrimaryToSymString(KonState* kstate, KN args)
{
    KN item = KN_CAR(args);
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
    KonSymbol* value = KN_ALLOC_TYPE_TAG(kstate, KonSymbol, KN_T_SYMBOL);
    value->Type = KN_SYM_STRING;
    value->Data = utf8dup(symCstr);
    return value;
}

KN KN_PrimaryUnboxQuote(KonState* kstate, KN args)
{
    KN obj = KN_CAR(args);
    if (KN_IS_QUOTE(obj)) {
        return ((KonQuote*)obj)->Inner;
    }
    else if (KN_IS_QUASIQUOTE(obj)) {
        return ((KonQuasiquote*)obj)->Inner;
    }
    else if (KN_IS_EXPAND(obj)) {
        return ((KonExpand*)obj)->Inner;
    }
    else if (KN_IS_UNQUOTE(obj)) {
        return ((KonUnquote*)obj)->Inner;
    }
    return obj;
}

KN KN_PrimaryGetDispatcherId(KonState* kstate, KN args)
{
    KN obj = KN_CAR(args);
    return KN_MAKE_FIXNUM(KN_NodeDispacherId(kstate, obj));
}

KN KN_PrimarySetDispatcherId(KonState* kstate, KN args)
{
    KN obj = KN_CAR(args);
    KN boxedId = KN_CADR(args);
    unsigned int dispatcherId = KN_UNBOX_FIXNUM(boxedId);
    if (KN_IS_POINTER(obj)) {
        ((KonBase*)obj)->MsgDispatcherId = dispatcherId;
    }
    return KN_TRUE;
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
    
    KN_EnvDefine(kstate, env, "not",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryNot)
    );
    KN_EnvDefine(kstate, env, "equal",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryEqual)
    );
    KN_EnvDefine(kstate, env, "eqv",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryEqv)
    );
    KN_EnvDefine(kstate, env, "eq",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryEq)
    );
    KN_EnvDefine(kstate, env, "neq",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryNeq)
    );

    // IO
    KN_EnvDefine(kstate, env, "newline",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryNewline)
    );
    KN_EnvDefine(kstate, env, "display",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryDisplay)
    );
    KN_EnvDefine(kstate, env, "displayln",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryDisplayln)
    );
    KN_EnvDefine(kstate, env, "write",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryWrite)
    );
    KN_EnvDefine(kstate, env, "writeln",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryWriteln)
    );

    KN_EnvDefine(kstate, env, "stringify",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryStringify)
    );

    KN_EnvDefine(kstate, env, "kon-from-str",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryParse)
    );

    // predict
    KN_EnvDefine(kstate, env, "is-true",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsTrue)
    );
    KN_EnvDefine(kstate, env, "is-false",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsFalse)
    );
    KN_EnvDefine(kstate, env, "is-nil",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsNil)
    );
    KN_EnvDefine(kstate, env, "is-null",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsNull)
    );
    KN_EnvDefine(kstate, env, "is-null",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsUkn)
    );
    KN_EnvDefine(kstate, env, "is-pointer",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsPointer)
    );
    KN_EnvDefine(kstate, env, "is-fixnum",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsFixnum)
    );
    KN_EnvDefine(kstate, env, "is-flonum",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsFlonum)
    );
    KN_EnvDefine(kstate, env, "is-imdt-symbol",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsImdtSymbol)
    );
    KN_EnvDefine(kstate, env, "is-char",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsChar)
    );
    KN_EnvDefine(kstate, env, "is-boolean",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsBoolean)
    );
    KN_EnvDefine(kstate, env, "is-bytes",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsBytes)
    );
    KN_EnvDefine(kstate, env, "is-string",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsString)
    );
    KN_EnvDefine(kstate, env, "is-symbol",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsSymbol)
    );
    KN_EnvDefine(kstate, env, "is-variable",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsVariable)
    );
    KN_EnvDefine(kstate, env, "is-identifier",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsIdentifier)
    );
    KN_EnvDefine(kstate, env, "is-word",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsWord)
    );
    KN_EnvDefine(kstate, env, "is-attr-slot",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsAttrSlot)
    );
    KN_EnvDefine(kstate, env, "is-pair",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsPair)
    );
    KN_EnvDefine(kstate, env, "is-list",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsPairList)
    );
    KN_EnvDefine(kstate, env, "is-vector",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsVector)
    );
    KN_EnvDefine(kstate, env, "is-table",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsTable)
    );
    KN_EnvDefine(kstate, env, "is-cell",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsCell)
    );
    KN_EnvDefine(kstate, env, "is-env",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsEnv)
    );
    KN_EnvDefine(kstate, env, "is-procedure",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsProcedure)
    );
    KN_EnvDefine(kstate, env, "is-continuation",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsContinuation)
    );
    KN_EnvDefine(kstate, env, "is-cpointer",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsCpointer)
    );
    KN_EnvDefine(kstate, env, "is-exception",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsException)
    );
    KN_EnvDefine(kstate, env, "unbox-quote",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryUnboxQuote)
    );

    KN_EnvDefine(kstate, env, "get-dispatcher-id",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryGetDispatcherId)
    );

    KN_EnvDefine(kstate, env, "set-dispatcher-id",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimarySetDispatcherId)
    );

    KN_EnvDefine(kstate, env, "to-variable",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryToVariable)
    );

    KN_EnvDefine(kstate, env, "to-identifier",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryToIdentifier)
    );

    KN_EnvDefine(kstate, env, "to-sym-string",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryToSymString)
    );

    KN_EnvDefine(kstate, env, "get-env", env);

    KN_PrimaryExportDispacherId(kstate, env);
}