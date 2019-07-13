#include "primary.h"

bool KON_IsPrimaryFunc(Kon* kstate, tb_string_ref_t funcName)
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

Kon* KON_PrimaryPlus(Kon* kstate, Kon* args)
{
    Kon* iter = args;
    Kon* first = kon_car(iter);
    if (first == KON_NIL) {
        return kon_make_fixnum(0);
    }

    int resFixnum = 0;
    double resFlonum = 0.0;
    bool isFixNum = false;
    // the result type is determined by the first arg
    if (kon_is_fixnum(first)) {
        isFixNum = true;
    }
    // Unbox
    do {
        Kon* item = kon_car(iter);

        if (kon_is_fixnum(item)) {
            int num = kon_unbox_fixnum(item);
            if (isFixNum) {
                resFixnum += num;
            }
            else {
                resFlonum += num;
            }
        }
        else if (kon_is_flonum(item)) {
            double num = item->Value.Flonum;
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
        Kon* result = KON_AllocTagged(kstate, sizeof(double), KON_FLONUM);
        return result;
    }
}


Kon* KON_PrimaryMinus(Kon* kstate, Kon* args)
{
    Kon* iter = args;
    Kon* first = kon_car(iter);
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
    if (kon_is_fixnum(first)) {
        isFixNum = true;
        resFixnum = kon_unbox_fixnum(first);
    }
    else {
        resFlonum = first->Value.Flonum;
    }
    // Unbox
    do {
        Kon* item = kon_car(iter);

        if (kon_is_fixnum(item)) {
            int num = kon_unbox_fixnum(item);
            if (isFixNum) {
                resFixnum -= num;
            }
            else {
                resFlonum -= num;
            }
        }
        else if (kon_is_flonum(item)) {
            double num = item->Value.Flonum;
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
        Kon* result = KON_AllocTagged(kstate, sizeof(double), KON_FLONUM);
        return result;
    }
}


Kon* KON_PrimaryMultiply(Kon* kstate, Kon* args)
{
    Kon* iter = args;
    Kon* first = kon_car(iter);
    if (first == KON_NIL) {
        return kon_make_fixnum(0);
    }

    int resFixnum = 1;
    double resFlonum = 1.0;
    bool isFixNum = false;
    // the result type is determined by the first arg
    if (kon_is_fixnum(first)) {
        isFixNum = true;
    }
    // Unbox
    do {
        Kon* item = kon_car(iter);

        if (kon_is_fixnum(item)) {
            int num = kon_unbox_fixnum(item);
            if (isFixNum) {
                resFixnum *= num;
            }
            else {
                resFlonum *= num;
            }
        }
        else if (kon_is_flonum(item)) {
            double num = item->Value.Flonum;
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
        Kon* result = KON_AllocTagged(kstate, sizeof(double), KON_FLONUM);
        return result;
    }
}

Kon* KON_PrimaryDivide(Kon* kstate, Kon* args)
{
    Kon* iter = args;
    Kon* first = kon_car(iter);
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
    if (kon_is_fixnum(first)) {
        isFixNum = true;
        resFixnum = kon_unbox_fixnum(first);
    }
    else {
        resFlonum = first->Value.Flonum;
    }
    // Unbox
    do {
        Kon* item = kon_car(iter);

        if (kon_is_fixnum(item)) {
            int num = kon_unbox_fixnum(item);
            if (isFixNum) {
                resFixnum /= num;
            }
            else {
                resFlonum /= num;
            }
        }
        else if (kon_is_flonum(item)) {
            double num = item->Value.Flonum;
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
        Kon* result = KON_AllocTagged(kstate, sizeof(double), KON_FLONUM);
        return result;
    }
}


Kon* KON_PrimaryMod(Kon* kstate, Kon* args)
{
    Kon* iter = args;
    Kon* first = kon_car(iter);
    iter = kon_cdr(iter);
    if (first == KON_NIL) {
        return kon_make_fixnum(0);
    }
    else if (iter == KON_NIL) {
        return first;
    }

    Kon* second = kon_car(iter);


    int resFixnum = 0;
    if (kon_is_fixnum(first)) {
        resFixnum = kon_unbox_fixnum(first);
    }
    else {
        resFixnum = (int)(first->Value.Flonum);
    }

    int mod = 1;
    if (kon_is_fixnum(first)) {
        mod = kon_unbox_fixnum(second);
    }
    else {
        mod = (int)(second->Value.Flonum);
    }
 
    return kon_make_fixnum(resFixnum % mod);
}

Kon* KON_PrimaryLowerThan(Kon* kstate, Kon* args)
{
    Kon* iter = args;
    Kon* first = kon_car(iter);
    if (first == KON_NIL) {
        return KON_TRUE;
    }
    iter = kon_cdr(iter);

    double lastNum = 0.0;

    if (kon_is_fixnum(first)) {
        lastNum = 1.0 * kon_unbox_fixnum(first);
    }
    else {
        lastNum = first->Value.Flonum;
    }

    do {
        Kon* item = kon_car(iter);

        double cmpNum = 0.0;
        if (kon_is_fixnum(item)) {
            cmpNum = kon_unbox_fixnum(item);
        }
        else if (kon_is_flonum(item)) {
            cmpNum = item->Value.Flonum;
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

Kon* KON_PrimaryLowerOrEqual(Kon* kstate, Kon* args)
{
    Kon* iter = args;
    Kon* first = kon_car(iter);
    if (first == KON_NIL) {
        return KON_TRUE;
    }
    iter = kon_cdr(iter);

    double lastNum = 0.0;

    if (kon_is_fixnum(first)) {
        lastNum = 1.0 * kon_unbox_fixnum(first);
    }
    else {
        lastNum = first->Value.Flonum;
    }

    do {
        Kon* item = kon_car(iter);

        double cmpNum = 0.0;
        if (kon_is_fixnum(item)) {
            cmpNum = kon_unbox_fixnum(item);
        }
        else if (kon_is_flonum(item)) {
            cmpNum = item->Value.Flonum;
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

Kon* KON_PrimaryGreaterThan(Kon* kstate, Kon* args)
{
    Kon* iter = args;
    Kon* first = kon_car(iter);
    if (first == KON_NIL) {
        return KON_TRUE;
    }
    iter = kon_cdr(iter);

    double lastNum = 0.0;

    if (kon_is_fixnum(first)) {
        lastNum = 1.0 * kon_unbox_fixnum(first);
    }
    else {
        lastNum = first->Value.Flonum;
    }

    do {
        Kon* item = kon_car(iter);

        double cmpNum = 0.0;
        if (kon_is_fixnum(item)) {
            cmpNum = kon_unbox_fixnum(item);
        }
        else if (kon_is_flonum(item)) {
            cmpNum = item->Value.Flonum;
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

Kon* KON_PrimaryGreaterOrEqual(Kon* kstate, Kon* args)
{
    Kon* iter = args;
    Kon* first = kon_car(iter);
    if (first == KON_NIL) {
        return KON_TRUE;
    }
    iter = kon_cdr(iter);

    double lastNum = 0.0;

    if (kon_is_fixnum(first)) {
        lastNum = 1.0 * kon_unbox_fixnum(first);
    }
    else {
        lastNum = first->Value.Flonum;
    }

    do {
        Kon* item = kon_car(iter);

        double cmpNum = 0.0;
        if (kon_is_fixnum(item)) {
            cmpNum = kon_unbox_fixnum(item);
        }
        else if (kon_is_flonum(item)) {
            cmpNum = item->Value.Flonum;
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

Kon* KON_PrimaryNewline(Kon* kstate, Kon* args)
{
    printf("\n");
    return kon_make_fixnum(1);
}

Kon* KON_PrimaryDisplay(Kon* kstate, Kon* args)
{
    Kon* iter = args;
    tb_string_t merged;
    tb_string_init(&merged);

    int state = 1; // 1 need verb, 2 need objects
    do {
        Kon* item = kon_car(iter);

        Kon* formated = KON_ToFormatString(&kstate, item, false, 0, "  ");
        tb_string_strcat(&merged, &formated->Value.String);
        
        iter = kon_cdr(iter);
    } while (iter != KON_NIL);
    printf("%s", tb_string_cstr(&merged));
    int size = tb_string_size(&merged);
    return kon_make_fixnum(size);
}

Kon* KON_PrimaryDisplayln(Kon* kstate, Kon* args)
{
    Kon* size = KON_PrimaryDisplay(kstate, args);
    KON_PrimaryNewline(kstate, args);
    return kon_make_fixnum(kon_unbox_fixnum(size) + 1);
}

// TODO select output
Kon* KON_PrimaryWrite(Kon* kstate, Kon* args)
{
    Kon* iter = args;
    tb_string_t merged;
    tb_string_init(&merged);

    int state = 1; // 1 need verb, 2 need objects
    do {
        Kon* item = kon_car(iter);

        if (kon_is_string(item)) {
            tb_string_strcat(&merged, &item->Value.String);
        }
        else {
            Kon* formated = KON_ToFormatString(&kstate, item, false, 0, "  ");
            tb_string_strcat(&merged, &formated->Value.String);
        }
        iter = kon_cdr(iter);
    } while (iter != KON_NIL);
    printf("%s", tb_string_cstr(&merged));
    int size = tb_string_size(&merged);
    return kon_make_fixnum(size);
}

// TODO select output
Kon* KON_PrimaryWriteln(Kon* kstate, Kon* args)
{
    Kon* size = KON_PrimaryWrite(kstate, args);
    KON_PrimaryNewline(kstate, args);
    return kon_make_fixnum(kon_unbox_fixnum(size) + 1);
}

Kon* KON_PrimaryStringify(Kon* kstate, Kon* args)
{
    Kon* item = kon_car(args);
    Kon* formated = KON_ToFormatString(&kstate, item, false, 0, "  ");

    return formated;
}
