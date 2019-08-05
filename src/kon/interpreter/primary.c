#include "primary.h"

// bool KON_IsPrimaryFunc(KonState* kstate, char* funcName)
// {
//     if (
//         // math
//         strcmp(funcName, "+")
//         || strcmp(funcName, "-")
//         || strcmp(funcName, "*")
//         || strcmp(funcName, "/")
//         || strcmp(funcName, "mod")   // %
//         || strcmp(funcName, "lt")    // <
//         || strcmp(funcName, "lte")   // <=
//         || strcmp(funcName, "gt")    // >
//         || strcmp(funcName, "gte")   // >=
        

//         // value
//         || strcmp(funcName, "eq")   // == value eq
//         || strcmp(funcName, "equal")   // === pointer equal, deep
//         || strcmp(funcName, "neq")   // != value not eq
//         || strcmp(funcName, "nequal")   // !== pointer not equal, deep
//         || strcmp(funcName, "not")   // !
        

//         // predicate
//         || strcmp(funcName, "is-null")
//         || strcmp(funcName, "is-symbol")
//         || strcmp(funcName, "is-string-sym")

//         // list
//         || strcmp(funcName, "make-list")
//         || strcmp(funcName, "is-nil")
//         || strcmp(funcName, "is-list")
//         || strcmp(funcName, "is-pair")
//         || strcmp(funcName, "car")
//         || strcmp(funcName, "cdr")
//         || strcmp(funcName, "cons")
//         || strcmp(funcName, "append")
        

//         // vector
//         || strcmp(funcName, "make-vector")
//         || strcmp(funcName, "is-vector")
//         || strcmp(funcName, "vector-push")   // add tail
//         || strcmp(funcName, "vector-pop")    // get tail and remove
//         || strcmp(funcName, "vector-shift")  // get head and remove
//         || strcmp(funcName, "vector-unshift")    // add head
//         || strcmp(funcName, "vector-head")
//         || strcmp(funcName, "vector-tail")
//         || strcmp(funcName, "vector-at")
//         || strcmp(funcName, "vector-del")

//         // table
//         || strcmp(funcName, "make-table")
//         || strcmp(funcName, "is-table")
//         || strcmp(funcName, "table-set")
//         || strcmp(funcName, "table-at")
//         || strcmp(funcName, "table-del")

//         // cell
//         || strcmp(funcName, "make-cell")
//         || strcmp(funcName, "is-cell")
//         || strcmp(funcName, "cell-set-tag")
//         || strcmp(funcName, "cell-set-vector")
//         || strcmp(funcName, "cell-set-table")
//         || strcmp(funcName, "cell-set-list")
//         || strcmp(funcName, "cell-get-tag")
//         || strcmp(funcName, "cell-get-vector")
//         || strcmp(funcName, "cell-get-table")
//         || strcmp(funcName, "cell-get-list")
//         || strcmp(funcName, "cell-del-tag")  // KON_NULL
//         || strcmp(funcName, "cell-del-vector")   // KON_NULL
//         || strcmp(funcName, "cell-del-table")    // KON_NULL
//         || strcmp(funcName, "cell-del-list") // KON_NULL
//         || strcmp(funcName, "cell-clear-vector") // []
//         || strcmp(funcName, "cell-clear-table")  // ()
//         || strcmp(funcName, "cell-clear-list")   // {}

//         // env

//         // io
//         || strcmp(funcName, "newline")
//         || strcmp(funcName, "display")
//         || strcmp(funcName, "displayln")
//         || strcmp(funcName, "write")
//         || strcmp(funcName, "writeln")
//         || strcmp(funcName, "stringify") // like JSON.stringify
//         || strcmp(funcName, "parse") // like JSON.parse

//     ) {
//         return true;
//     }
//     else {
//         return false;
//     }
// }

KN KON_PrimaryPlus(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KON_CAR(iter);
    if (first == KON_NIL) {
        return KON_MAKE_FIXNUM(0);
    }

    int resFixnum = 0;
    double resFlonum = 0.0;
    bool isFixNum = false;
    // the result type is determined by the first arg
    if (KON_IS_FIXNUM(first)) {
        isFixNum = true;
    }
    // Unbox
    do {
        KN item = KON_CAR(iter);

        if (KON_IS_FIXNUM(item)) {
            int num = KON_UNBOX_FIXNUM(item);
            if (isFixNum) {
                resFixnum += num;
            }
            else {
                resFlonum += num;
            }
        }
        else if (KON_IS_FLONUM(item)) {
            double num = KON_UNBOX_FLONUM(item);
            if (isFixNum) {
                resFixnum += num;
            }
            else {
                resFlonum += num;
            }
        }

        iter = KON_CDR(iter);
    } while (iter != KON_NIL);

    
    if (isFixNum) {
        // TODO overflow check
        return KON_MAKE_FIXNUM(resFixnum);
    }
    else {
        KN result = KON_MAKE_FLONUM(kstate, resFlonum);
        return result;
    }
}


KN KON_PrimaryMinus(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KON_CAR(iter);
    iter = KON_CDR(iter);
    if (first == KON_NIL) {
        return KON_MAKE_FIXNUM(0);
    }
    else if (iter == KON_NIL) {
        return first;
    }
    

    int resFixnum = 0;
    double resFlonum = 0.0;
    bool isFixNum = false;
    // the result type is determined by the first arg
    if (KON_IS_FIXNUM(first)) {
        isFixNum = true;
        resFixnum = KON_UNBOX_FIXNUM(first);
    }
    else {
        resFlonum =  KON_UNBOX_FLONUM(first);
    }
    // Unbox
    do {
        KN item = KON_CAR(iter);

        if (KON_IS_FIXNUM(item)) {
            int num = KON_UNBOX_FIXNUM(item);
            if (isFixNum) {
                resFixnum -= num;
            }
            else {
                resFlonum -= num;
            }
        }
        else if (KON_IS_FLONUM(item)) {
            double num = KON_UNBOX_FLONUM(item);
            if (isFixNum) {
                resFixnum -= num;
            }
            else {
                resFlonum -= num;
            }
        }

        iter = KON_CDR(iter);
    } while (iter != KON_NIL);

    
    if (isFixNum) {
        // TODO overflow check
        return KON_MAKE_FIXNUM(resFixnum);
    }
    else {
        KN result = KON_MAKE_FLONUM(kstate, resFlonum);
        return result;
    }
}


KN KON_PrimaryMultiply(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KON_CAR(iter);
    if (first == KON_NIL) {
        return KON_MAKE_FIXNUM(0);
    }

    int resFixnum = 1;
    double resFlonum = 1.0;
    bool isFixNum = false;
    // the result type is determined by the first arg
    if (KON_IS_FIXNUM(first)) {
        isFixNum = true;
    }
    // Unbox
    do {
        KN item = KON_CAR(iter);

        if (KON_IS_FIXNUM(item)) {
            int num = KON_UNBOX_FIXNUM(item);
            if (isFixNum) {
                resFixnum *= num;
            }
            else {
                resFlonum *= num;
            }
        }
        else if (KON_IS_FLONUM(item)) {
            double num = KON_UNBOX_FLONUM(item);
            if (isFixNum) {
                resFixnum *= num;
            }
            else {
                resFlonum *= num;
            }
        }

        iter = KON_CDR(iter);
    } while (iter != KON_NIL);

    
    if (isFixNum) {
        // TODO overflow check
        return KON_MAKE_FIXNUM(resFixnum);
    }
    else {
        KN result = KON_MAKE_FLONUM(kstate, resFlonum);
        return result;
    }
}

KN KON_PrimaryDivide(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KON_CAR(iter);
    iter = KON_CDR(iter);
    if (first == KON_NIL) {
        return KON_MAKE_FIXNUM(0);
    }
    else if (iter == KON_NIL) {
        return first;
    }
    

    int resFixnum = 0;
    double resFlonum = 0.0;
    bool isFixNum = false;
    // the result type is determined by the first arg
    if (KON_IS_FIXNUM(first)) {
        isFixNum = true;
        resFixnum = KON_UNBOX_FIXNUM(first);
    }
    else {
        resFlonum = KON_UNBOX_FLONUM(first);
    }
    // Unbox
    do {
        KN item = KON_CAR(iter);

        if (KON_IS_FIXNUM(item)) {
            int num = KON_UNBOX_FIXNUM(item);
            if (isFixNum) {
                resFixnum /= num;
            }
            else {
                resFlonum /= num;
            }
        }
        else if (KON_IS_FLONUM(item)) {
            double num = KON_UNBOX_FLONUM(item);
            if (isFixNum) {
                resFixnum /= num;
            }
            else {
                resFlonum /= num;
            }
        }

        iter = KON_CDR(iter);
    } while (iter != KON_NIL);

    
    if (isFixNum) {
        // TODO overflow check
        return KON_MAKE_FIXNUM(resFixnum);
    }
    else {
        KN result = KON_MAKE_FLONUM(kstate, resFlonum);
        return result;
    }
}


KN KON_PrimaryMod(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KON_CAR(iter);
    iter = KON_CDR(iter);
    if (first == KON_NIL) {
        return KON_MAKE_FIXNUM(0);
    }
    else if (iter == KON_NIL) {
        return first;
    }

    KN second = KON_CAR(iter);


    int resFixnum = 0;
    if (KON_IS_FIXNUM(first)) {
        resFixnum = KON_UNBOX_FIXNUM(first);
    }
    else {
        resFixnum = (int)(KON_UNBOX_FLONUM(first));
    }

    int mod = 1;
    if (KON_IS_FIXNUM(first)) {
        mod = KON_UNBOX_FIXNUM(second);
    }
    else {
        mod = (int)(KON_UNBOX_FLONUM(second));
    }
 
    return KON_MAKE_FIXNUM(resFixnum % mod);
}

KN KON_PrimaryLowerThan(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KON_CAR(iter);
    if (first == KON_NIL) {
        return KON_TRUE;
    }
    iter = KON_CDR(iter);

    double lastNum = 0.0;

    if (KON_IS_FIXNUM(first)) {
        lastNum = 1.0 * KON_UNBOX_FIXNUM(first);
    }
    else {
        lastNum = KON_UNBOX_FLONUM(first);
    }

    do {
        KN item = KON_CAR(iter);

        double cmpNum = 0.0;
        if (KON_IS_FIXNUM(item)) {
            cmpNum = KON_UNBOX_FIXNUM(item);
        }
        else if (KON_IS_FLONUM(item)) {
            cmpNum = KON_UNBOX_FLONUM(item);
        }

        if (lastNum >= cmpNum) {
            return KON_FALSE;
        }
        else {
            lastNum = cmpNum;
        }

        iter = KON_CDR(iter);
    } while (iter != KON_NIL);

    
    return KON_TRUE;
}

KN KON_PrimaryLowerOrEqual(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KON_CAR(iter);
    if (first == KON_NIL) {
        return KON_TRUE;
    }
    iter = KON_CDR(iter);

    double lastNum = 0.0;

    if (KON_IS_FIXNUM(first)) {
        lastNum = 1.0 * KON_UNBOX_FIXNUM(first);
    }
    else {
        lastNum = KON_UNBOX_FLONUM(first);
    }

    do {
        KN item = KON_CAR(iter);

        double cmpNum = 0.0;
        if (KON_IS_FIXNUM(item)) {
            cmpNum = KON_UNBOX_FIXNUM(item);
        }
        else if (KON_IS_FLONUM(item)) {
            cmpNum = KON_UNBOX_FLONUM(item);
        }

        if (lastNum > cmpNum) {
            return KON_FALSE;
        }
        else {
            lastNum = cmpNum;
        }

        iter = KON_CDR(iter);
    } while (iter != KON_NIL);

    
    return KON_TRUE;
}

KN KON_PrimaryGreaterThan(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KON_CAR(iter);
    if (first == KON_NIL) {
        return KON_TRUE;
    }
    iter = KON_CDR(iter);

    double lastNum = 0.0;

    if (KON_IS_FIXNUM(first)) {
        lastNum = 1.0 * KON_UNBOX_FIXNUM(first);
    }
    else {
        lastNum = KON_UNBOX_FLONUM(first);
    }

    do {
        KN item = KON_CAR(iter);

        double cmpNum = 0.0;
        if (KON_IS_FIXNUM(item)) {
            cmpNum = KON_UNBOX_FIXNUM(item);
        }
        else if (KON_IS_FLONUM(item)) {
            cmpNum = KON_UNBOX_FLONUM(item);
        }

        if (lastNum <= cmpNum) {
            return KON_FALSE;
        }
        else {
            lastNum = cmpNum;
        }

        iter = KON_CDR(iter);
    } while (iter != KON_NIL);

    
    return KON_TRUE;
}

KN KON_PrimaryGreaterOrEqual(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KON_CAR(iter);
    if (first == KON_NIL) {
        return KON_TRUE;
    }
    iter = KON_CDR(iter);

    double lastNum = 0.0;

    if (KON_IS_FIXNUM(first)) {
        lastNum = 1.0 * KON_UNBOX_FIXNUM(first);
    }
    else {
        lastNum = KON_UNBOX_FLONUM(first);
    }

    do {
        KN item = KON_CAR(iter);

        double cmpNum = 0.0;
        if (KON_IS_FIXNUM(item)) {
            cmpNum = KON_UNBOX_FIXNUM(item);
        }
        else if (KON_IS_FLONUM(item)) {
            cmpNum = KON_UNBOX_FLONUM(item);
        }

        if (lastNum < cmpNum) {
            return KON_FALSE;
        }
        else {
            lastNum = cmpNum;
        }

        iter = KON_CDR(iter);
    } while (iter != KON_NIL);

    
    return KON_TRUE;
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

        if (kon_is_string(item)) {
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
