#include "primary.h"

bool KON_IsPrimaryFunc(KonState* kstate, tb_string_ref_t funcName)
{
    if (
        // math
        tb_string_cstrcmp(funcName, "+")
        || tb_string_cstrcmp(funcName, "-")
        || tb_string_cstrcmp(funcName, "*")
        || tb_string_cstrcmp(funcName, "/")
        || tb_string_cstrcmp(funcName, "mod")   // %
        || tb_string_cstrcmp(funcName, "lt")    // <
        || tb_string_cstrcmp(funcName, "lte")   // <=
        || tb_string_cstrcmp(funcName, "gt")    // >
        || tb_string_cstrcmp(funcName, "gte")   // >=
        

        // value
        || tb_string_cstrcmp(funcName, "eq")   // == value eq
        || tb_string_cstrcmp(funcName, "equal")   // === pointer equal, deep
        || tb_string_cstrcmp(funcName, "neq")   // != value not eq
        || tb_string_cstrcmp(funcName, "nequal")   // !== pointer not equal, deep
        || tb_string_cstrcmp(funcName, "not")   // !
        

        // predicate
        || tb_string_cstrcmp(funcName, "is-null")
        || tb_string_cstrcmp(funcName, "is-symbol")
        || tb_string_cstrcmp(funcName, "is-string-sym")

        // list
        || tb_string_cstrcmp(funcName, "make-list")
        || tb_string_cstrcmp(funcName, "is-nil")
        || tb_string_cstrcmp(funcName, "is-list")
        || tb_string_cstrcmp(funcName, "is-pair")
        || tb_string_cstrcmp(funcName, "car")
        || tb_string_cstrcmp(funcName, "cdr")
        || tb_string_cstrcmp(funcName, "cons")
        || tb_string_cstrcmp(funcName, "append")
        

        // vector
        || tb_string_cstrcmp(funcName, "make-vector")
        || tb_string_cstrcmp(funcName, "is-vector")
        || tb_string_cstrcmp(funcName, "vector-push")   // add tail
        || tb_string_cstrcmp(funcName, "vector-pop")    // get tail and remove
        || tb_string_cstrcmp(funcName, "vector-shift")  // get head and remove
        || tb_string_cstrcmp(funcName, "vector-unshift")    // add head
        || tb_string_cstrcmp(funcName, "vector-head")
        || tb_string_cstrcmp(funcName, "vector-tail")
        || tb_string_cstrcmp(funcName, "vector-at")
        || tb_string_cstrcmp(funcName, "vector-del")

        // table
        || tb_string_cstrcmp(funcName, "make-table")
        || tb_string_cstrcmp(funcName, "is-table")
        || tb_string_cstrcmp(funcName, "table-set")
        || tb_string_cstrcmp(funcName, "table-at")
        || tb_string_cstrcmp(funcName, "table-del")

        // cell
        || tb_string_cstrcmp(funcName, "make-cell")
        || tb_string_cstrcmp(funcName, "is-cell")
        || tb_string_cstrcmp(funcName, "cell-set-tag")
        || tb_string_cstrcmp(funcName, "cell-set-vector")
        || tb_string_cstrcmp(funcName, "cell-set-table")
        || tb_string_cstrcmp(funcName, "cell-set-list")
        || tb_string_cstrcmp(funcName, "cell-get-tag")
        || tb_string_cstrcmp(funcName, "cell-get-vector")
        || tb_string_cstrcmp(funcName, "cell-get-table")
        || tb_string_cstrcmp(funcName, "cell-get-list")
        || tb_string_cstrcmp(funcName, "cell-del-tag")  // KON_NULL
        || tb_string_cstrcmp(funcName, "cell-del-vector")   // KON_NULL
        || tb_string_cstrcmp(funcName, "cell-del-table")    // KON_NULL
        || tb_string_cstrcmp(funcName, "cell-del-list") // KON_NULL
        || tb_string_cstrcmp(funcName, "cell-clear-vector") // []
        || tb_string_cstrcmp(funcName, "cell-clear-table")  // ()
        || tb_string_cstrcmp(funcName, "cell-clear-list")   // {}

        // env

        // io
        || tb_string_cstrcmp(funcName, "newline")
        || tb_string_cstrcmp(funcName, "display")
        || tb_string_cstrcmp(funcName, "displayln")
        || tb_string_cstrcmp(funcName, "write")
        || tb_string_cstrcmp(funcName, "writeln")
        || tb_string_cstrcmp(funcName, "stringify") // like JSON.stringify
        || tb_string_cstrcmp(funcName, "parse") // like JSON.parse

    ) {
        return true;
    }
    else {
        return false;
    }
}

KN KON_PrimaryPlus(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = kon_car(iter);
    if (first == KON_NIL) {
        return kon_make_fixnum(0);
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
        KN item = kon_car(iter);

        if (KON_IS_FIXNUM(item)) {
            int num = kon_unbox_fixnum(item);
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

        iter = kon_cdr(iter);
    } while (iter != KON_NIL);

    
    if (isFixNum) {
        // TODO overflow check
        return kon_make_fixnum(resFixnum);
    }
    else {
        KN result = KON_MAKE_FLONUM(kstate, resFlonum);
        return result;
    }
}


KN KON_PrimaryMinus(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = kon_car(iter);
    iter = kon_cdr(iter);
    if (first == KON_NIL) {
        return kon_make_fixnum(0);
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
        resFixnum = kon_unbox_fixnum(first);
    }
    else {
        resFlonum =  KON_UNBOX_FLONUM(first);
    }
    // Unbox
    do {
        KN item = kon_car(iter);

        if (KON_IS_FIXNUM(item)) {
            int num = kon_unbox_fixnum(item);
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

        iter = kon_cdr(iter);
    } while (iter != KON_NIL);

    
    if (isFixNum) {
        // TODO overflow check
        return kon_make_fixnum(resFixnum);
    }
    else {
        KN result = KON_MAKE_FLONUM(kstate, resFlonum);
        return result;
    }
}


KN KON_PrimaryMultiply(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = kon_car(iter);
    if (first == KON_NIL) {
        return kon_make_fixnum(0);
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
        KN item = kon_car(iter);

        if (KON_IS_FIXNUM(item)) {
            int num = kon_unbox_fixnum(item);
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

        iter = kon_cdr(iter);
    } while (iter != KON_NIL);

    
    if (isFixNum) {
        // TODO overflow check
        return kon_make_fixnum(resFixnum);
    }
    else {
        KN result = KON_MAKE_FLONUM(kstate, resFlonum);
        return result;
    }
}

KN KON_PrimaryDivide(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = kon_car(iter);
    iter = kon_cdr(iter);
    if (first == KON_NIL) {
        return kon_make_fixnum(0);
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
        resFixnum = kon_unbox_fixnum(first);
    }
    else {
        resFlonum = KON_UNBOX_FLONUM(first);
    }
    // Unbox
    do {
        KN item = kon_car(iter);

        if (KON_IS_FIXNUM(item)) {
            int num = kon_unbox_fixnum(item);
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

        iter = kon_cdr(iter);
    } while (iter != KON_NIL);

    
    if (isFixNum) {
        // TODO overflow check
        return kon_make_fixnum(resFixnum);
    }
    else {
        KN result = KON_MAKE_FLONUM(kstate, resFlonum);
        return result;
    }
}


KN KON_PrimaryMod(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = kon_car(iter);
    iter = kon_cdr(iter);
    if (first == KON_NIL) {
        return kon_make_fixnum(0);
    }
    else if (iter == KON_NIL) {
        return first;
    }

    KN second = kon_car(iter);


    int resFixnum = 0;
    if (KON_IS_FIXNUM(first)) {
        resFixnum = kon_unbox_fixnum(first);
    }
    else {
        resFixnum = (int)(KON_UNBOX_FLONUM(first));
    }

    int mod = 1;
    if (KON_IS_FIXNUM(first)) {
        mod = kon_unbox_fixnum(second);
    }
    else {
        mod = (int)(KON_UNBOX_FLONUM(second));
    }
 
    return kon_make_fixnum(resFixnum % mod);
}

KN KON_PrimaryLowerThan(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = kon_car(iter);
    if (first == KON_NIL) {
        return KON_TRUE;
    }
    iter = kon_cdr(iter);

    double lastNum = 0.0;

    if (KON_IS_FIXNUM(first)) {
        lastNum = 1.0 * kon_unbox_fixnum(first);
    }
    else {
        lastNum = KON_UNBOX_FLONUM(first);
    }

    do {
        KN item = kon_car(iter);

        double cmpNum = 0.0;
        if (KON_IS_FIXNUM(item)) {
            cmpNum = kon_unbox_fixnum(item);
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

        iter = kon_cdr(iter);
    } while (iter != KON_NIL);

    
    return KON_TRUE;
}

KN KON_PrimaryLowerOrEqual(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = kon_car(iter);
    if (first == KON_NIL) {
        return KON_TRUE;
    }
    iter = kon_cdr(iter);

    double lastNum = 0.0;

    if (KON_IS_FIXNUM(first)) {
        lastNum = 1.0 * kon_unbox_fixnum(first);
    }
    else {
        lastNum = KON_UNBOX_FLONUM(first);
    }

    do {
        KN item = kon_car(iter);

        double cmpNum = 0.0;
        if (KON_IS_FIXNUM(item)) {
            cmpNum = kon_unbox_fixnum(item);
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

        iter = kon_cdr(iter);
    } while (iter != KON_NIL);

    
    return KON_TRUE;
}

KN KON_PrimaryGreaterThan(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = kon_car(iter);
    if (first == KON_NIL) {
        return KON_TRUE;
    }
    iter = kon_cdr(iter);

    double lastNum = 0.0;

    if (KON_IS_FIXNUM(first)) {
        lastNum = 1.0 * kon_unbox_fixnum(first);
    }
    else {
        lastNum = KON_UNBOX_FLONUM(first);
    }

    do {
        KN item = kon_car(iter);

        double cmpNum = 0.0;
        if (KON_IS_FIXNUM(item)) {
            cmpNum = kon_unbox_fixnum(item);
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

        iter = kon_cdr(iter);
    } while (iter != KON_NIL);

    
    return KON_TRUE;
}

KN KON_PrimaryGreaterOrEqual(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = kon_car(iter);
    if (first == KON_NIL) {
        return KON_TRUE;
    }
    iter = kon_cdr(iter);

    double lastNum = 0.0;

    if (KON_IS_FIXNUM(first)) {
        lastNum = 1.0 * kon_unbox_fixnum(first);
    }
    else {
        lastNum = KON_UNBOX_FLONUM(first);
    }

    do {
        KN item = kon_car(iter);

        double cmpNum = 0.0;
        if (KON_IS_FIXNUM(item)) {
            cmpNum = kon_unbox_fixnum(item);
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

        iter = kon_cdr(iter);
    } while (iter != KON_NIL);

    
    return KON_TRUE;
}

KN KON_PrimaryNewline(KonState* kstate, KN args)
{
    printf("\n");
    return kon_make_fixnum(1);
}

KN KON_PrimaryDisplay(KonState* kstate, KN args)
{
    KN iter = args;
    tb_string_t merged;
    tb_string_init(&merged);

    int state = 1; // 1 need verb, 2 need objects
    do {
        KN item = kon_car(iter);

        KN formated = KON_ToFormatString(kstate, item, false, 0, "  ");
        tb_string_strcat(&merged, &KON_UNBOX_STRING(formated));
        
        iter = kon_cdr(iter);
    } while (iter != KON_NIL);
    printf("%s", tb_string_cstr(&merged));
    int size = tb_string_size(&merged);
    return kon_make_fixnum(size);
}

KN KON_PrimaryDisplayln(KonState* kstate, KN args)
{
    KN size = KON_PrimaryDisplay(kstate, args);
    KON_PrimaryNewline(kstate, args);
    return kon_make_fixnum(kon_unbox_fixnum(size) + 1);
}

// TODO select output
KN KON_PrimaryWrite(KonState* kstate, KN args)
{
    KN iter = args;
    tb_string_t merged;
    tb_string_init(&merged);

    int state = 1; // 1 need verb, 2 need objects
    do {
        KN item = kon_car(iter);

        if (kon_is_string(item)) {
            tb_string_strcat(&merged, &KON_UNBOX_STRING(item));
        }
        else {
            KN formated = KON_ToFormatString(kstate, item, false, 0, "  ");
            tb_string_strcat(&merged, &KON_UNBOX_STRING(formated));
        }
        iter = kon_cdr(iter);
    } while (iter != KON_NIL);
    printf("%s", tb_string_cstr(&merged));
    int size = tb_string_size(&merged);
    return kon_make_fixnum(size);
}

// TODO select output
KN KON_PrimaryWriteln(KonState* kstate, KN args)
{
    KN size = KON_PrimaryWrite(kstate, args);
    KON_PrimaryNewline(kstate, args);
    return kon_make_fixnum(kon_unbox_fixnum(size) + 1);
}

KN KON_PrimaryStringify(KonState* kstate, KN args)
{
    KN item = kon_car(args);
    KN formated = KON_ToFormatString(&kstate, item, false, 0, "  ");

    return formated;
}
