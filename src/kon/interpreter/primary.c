#include "primary.h"

// not(kon_false) => true, not(other) => false
KN KON_PrimaryNot(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    if (KON_IS_FALSE(item)) {
        return KON_TRUE;
    }
    else {
        return KON_FALSE;
    }
}

// pointer check
KN KON_PrimaryEqual(KonState* kstate, KN args)
{
    KN left = KON_CAR(args);
    KN right = KON_CADR(args);
    if (left == right) {
        return KON_TRUE;
    }
    else {
        return KON_FALSE;
    }
}

// basic types, boolean, number, char symbol, string value check
KN KON_PrimaryEqv(KonState* kstate, KN args)
{
    KN left = KON_CAR(args);
    KN right = KON_CADR(args);
    if (left == KON_NULL || left == KON_UKN) {
        return (left == right) ? KON_TRUE: KON_FALSE;
    }
    else if (KON_IS_NIL(left) || KON_IS_NIL(right)) {
        return (KON_IS_NIL(left) && KON_IS_NIL(right)) ? KON_TRUE: KON_FALSE;
    }
    else if (KON_IS_BOOLEAN(left) && KON_IS_BOOLEAN(right)) {
        return (left == right) ? KON_TRUE: KON_FALSE;
    }
    else if (KON_IS_FIXNUM(left) && KON_IS_FIXNUM(right)) {
        return (left == right) ? KON_TRUE: KON_FALSE;
    }
    else if (KON_IS_FLONUM(left) && KON_IS_FLONUM(right)) {
        return (KON_UNBOX_FLONUM(left) == KON_UNBOX_FLONUM(right)) ? KON_TRUE: KON_FALSE;
    }
    else if (KON_IS_CHAR(left) && KON_IS_CHAR(right)) {
        return (KON_UNBOX_CHAR(left) == KON_UNBOX_CHAR(right)) ? KON_TRUE: KON_FALSE;
    }
    // TODO KON_IS_BYTE
    else if (KON_IS_SYMBOL(left) && KON_IS_SYMBOL(right)) {
        const char* leftStr = KON_UNBOX_SYMBOL(left);
        const char* rightStr = KON_UNBOX_SYMBOL(right);
        return (strcmp(leftStr, rightStr) == 0) ? KON_TRUE: KON_FALSE;;
    }
    
    else if (KON_IS_STRING(left) && KON_IS_STRING(right)) {
        const char* leftStr = KxStringBuffer_Cstr(KON_UNBOX_STRING(left));
        const char* rightStr = KxStringBuffer_Cstr(KON_UNBOX_STRING(right));
        return (strcmp(leftStr, rightStr) == 0) ? KON_TRUE: KON_FALSE;;
    }
    // treat quote a empty list $[] equal to #nil;
    else if (KON_IS_QUOTE_NIL(left) || KON_IS_QUOTE_NIL(right)) {

    }
    else {
        return KON_FALSE;
    }
}

KN KON_PrimaryEq(KonState* kstate, KN args)
{
    KN left = KON_CAR(args);
    if (left == KON_NULL || left == KON_UKN
        || KON_IS_NIL(left)
        || KON_IS_BOOLEAN(left)
        || KON_IS_FIXNUM(left)
        || KON_IS_FLONUM(left)
        || KON_IS_CHAR(left)
        || KON_IS_SYMBOL(left)
        || KON_IS_STRING(left)
    ) {
        return KON_PrimaryEqv(kstate, args);
    }
    else {
        return KON_PrimaryEqual(kstate, args);
    }
}

KN KON_PrimaryNeq(KonState* kstate, KN args)
{
    KN result = KON_PrimaryEq(kstate, args);
    if (result == KON_FALSE) {
        return KON_TRUE;
    }
    else {
        return KON_FALSE;
    }
}

KN KON_PrimaryNewline(KonState* kstate, KN args)
{
    printf("\n");
    return KON_MAKE_FIXNUM(1);
}

KN KON_PrimaryDisplay(KonState* kstate, KN args)
{
    KN iter = args;
    KxStringBuffer* merged = KxStringBuffer_New();

    int state = 1; // 1 need verb, 2 need objects
    do {
        KN item = KON_CAR(iter);

        KN formated = KON_ToFormatString(kstate, item, false, 0, "  ");
        KxStringBuffer_AppendStringBuffer(merged, KON_UNBOX_STRING(formated));

        iter = KON_CDR(iter);
    } while (iter != KON_NIL);
    printf("%s", KxStringBuffer_Cstr(merged));
    int size = KxStringBuffer_Length(merged);
    return KON_MAKE_FIXNUM(size);
}

KN KON_PrimaryDisplayln(KonState* kstate, KN args)
{
    KN size = KON_PrimaryDisplay(kstate, args);
    KON_PrimaryNewline(kstate, args);
    return KON_MAKE_FIXNUM(KON_UNBOX_FIXNUM(size) + 1);
}

// TODO select output
KN KON_PrimaryWrite(KonState* kstate, KN args)
{
    KN iter = args;
    KxStringBuffer* merged = KxStringBuffer_New();

    int state = 1; // 1 need verb, 2 need objects
    do {
        KN item = KON_CAR(iter);

        if (KON_IS_STRING(item)) {
            KxStringBuffer_AppendStringBuffer(merged, KON_UNBOX_STRING(item));
        }
        else {
            KN formated = KON_ToFormatString(kstate, item, false, 0, "  ");
            KxStringBuffer_AppendStringBuffer(merged, KON_UNBOX_STRING(formated));
        }
        iter = KON_CDR(iter);
    } while (iter != KON_NIL);
    printf("%s", KxStringBuffer_Cstr(merged));
    int size = KxStringBuffer_Length(merged);
    return KON_MAKE_FIXNUM(size);
}

// TODO select output
KN KON_PrimaryWriteln(KonState* kstate, KN args)
{
    KN size = KON_PrimaryWrite(kstate, args);
    KON_PrimaryNewline(kstate, args);
    return KON_MAKE_FIXNUM(KON_UNBOX_FIXNUM(size) + 1);
}

KN KON_PrimaryStringify(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    KN formated = KON_ToFormatString(&kstate, item, false, 0, "  ");

    return formated;
}

// TODO
KN KON_PrimaryParse(KonState* kstate, KN args)
{
    // KN item = KON_CAR(args);
    
    // return item;
    return KON_NULL;
}

KN KON_PrimaryIsTrue(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_TRUE(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsFalse(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_FALSE(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsNil(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_NIL(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsNull(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_NULL(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsUkn(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_UKN(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsPointer(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_POINTER(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsFixnum(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_FIXNUM(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsFlonum(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_FLONUM(item)) ? KON_TRUE : KON_FALSE;
}


KN KON_PrimaryIsImmediateSymbol(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_IMMEDIATE_SYMBOL(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsChar(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_CHAR(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsBoolean(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_BOOLEAN(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsBytes(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_BYTES(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsString(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_STRING(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsSymbol(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_SYMBOL(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsVariable(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_VARIABLE(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsIdentifier(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_IDENTIFIER(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsWord(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_WORD(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsPathQuery(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_QUERY_PATH(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsAttrSlot(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_ATTR_SLOT(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsPair(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_PAIR(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsPairList(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IsPairList(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsVector(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_VECTOR(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsTable(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_TABLE(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsCell(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_CELL(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsEnv(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_ENV(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsProcedure(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_PROCEDURE(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsContinuation(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_CONTINUATION(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsCpointer(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_CPOINTER(item)) ? KON_TRUE : KON_FALSE;
}

KN KON_PrimaryIsException(KonState* kstate, KN args)
{
    KN item = KON_CAR(args);
    return (KON_IS_EXCEPTION(item)) ? KON_TRUE : KON_FALSE;
}


KN KON_PrimaryOpExport(KonState* kstate, KonEnv* env)
{
    
    KON_EnvDefine(kstate, env, "not",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryNot)
    );
    KON_EnvDefine(kstate, env, "equal",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryEqual)
    );
    KON_EnvDefine(kstate, env, "eqv",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryEqv)
    );
    KON_EnvDefine(kstate, env, "eq",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryEq)
    );
    KON_EnvDefine(kstate, env, "neq",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryNeq)
    );

    // IO
    KON_EnvDefine(kstate, env, "newline",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryNewline)
    );
    KON_EnvDefine(kstate, env, "display",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryDisplay)
    );
    KON_EnvDefine(kstate, env, "displayln",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryDisplayln)
    );
    KON_EnvDefine(kstate, env, "write",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryWrite)
    );
    KON_EnvDefine(kstate, env, "writeln",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryWriteln)
    );

    KON_EnvDefine(kstate, env, "stringify",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryStringify)
    );

    KON_EnvDefine(kstate, env, "kon-from-str",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryParse)
    );

    // predict
    KON_EnvDefine(kstate, env, "is-true",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsTrue)
    );
    KON_EnvDefine(kstate, env, "is-false",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsFalse)
    );
    KON_EnvDefine(kstate, env, "is-nil",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsNil)
    );
    KON_EnvDefine(kstate, env, "is-null",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsNull)
    );
    KON_EnvDefine(kstate, env, "is-null",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsUkn)
    );
    KON_EnvDefine(kstate, env, "is-pointer",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsPointer)
    );
    KON_EnvDefine(kstate, env, "is-fixnum",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsFixnum)
    );
    KON_EnvDefine(kstate, env, "is-flonum",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsFlonum)
    );
    KON_EnvDefine(kstate, env, "is-immediate-symbol",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsImmediateSymbol)
    );
    KON_EnvDefine(kstate, env, "is-char",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsChar)
    );
    KON_EnvDefine(kstate, env, "is-boolean",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsBoolean)
    );
    KON_EnvDefine(kstate, env, "is-bytes",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsBytes)
    );
    KON_EnvDefine(kstate, env, "is-string",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsString)
    );
    KON_EnvDefine(kstate, env, "is-symbol",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsSymbol)
    );
    KON_EnvDefine(kstate, env, "is-variable",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsVariable)
    );
    KON_EnvDefine(kstate, env, "is-identifier",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsIdentifier)
    );
    KON_EnvDefine(kstate, env, "is-word",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsWord)
    );
    KON_EnvDefine(kstate, env, "is-path-query",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsPathQuery)
    );
    KON_EnvDefine(kstate, env, "is-attr-slot",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsAttrSlot)
    );
    KON_EnvDefine(kstate, env, "is-pair",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsPair)
    );
    KON_EnvDefine(kstate, env, "is-pair-list",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsPairList)
    );
    KON_EnvDefine(kstate, env, "is-vector",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsVector)
    );
    KON_EnvDefine(kstate, env, "is-table",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsTable)
    );
    KON_EnvDefine(kstate, env, "is-cell",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsCell)
    );
    KON_EnvDefine(kstate, env, "is-env",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsEnv)
    );
    KON_EnvDefine(kstate, env, "is-procedure",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsProcedure)
    );
    KON_EnvDefine(kstate, env, "is-continuation",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsContinuation)
    );
    KON_EnvDefine(kstate, env, "is-cpointer",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsCpointer)
    );
    KON_EnvDefine(kstate, env, "is-exception",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryIsException)
    );
}