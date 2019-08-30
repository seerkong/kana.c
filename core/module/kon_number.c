#include "kon_number.h"

KN KN_PrimaryPlus(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KN_CAR(iter);
    if (first == KN_NIL) {
        return KN_MAKE_FIXNUM(0);
    }

    int resFixnum = 0;
    double resFlonum = 0.0;
    bool isFixNum = false;
    // the result type is determined by the first arg
    if (KN_IS_FIXNUM(first)) {
        isFixNum = true;
    }
    // Unbox
    do {
        KN item = KN_CAR(iter);

        if (KN_IS_FIXNUM(item)) {
            int num = KN_UNBOX_FIXNUM(item);
            if (isFixNum) {
                resFixnum += num;
            }
            else {
                resFlonum += num;
            }
        }
        else if (KN_IS_FLONUM(item)) {
            double num = KN_UNBOX_FLONUM(item);
            if (isFixNum) {
                resFixnum += num;
            }
            else {
                resFlonum += num;
            }
        }

        iter = KN_CDR(iter);
    } while (iter != KN_NIL);

    
    if (isFixNum) {
        // TODO overflow check
        return KN_MAKE_FIXNUM(resFixnum);
    }
    else {
        KN result = KN_MAKE_FLONUM(kstate, resFlonum);
        return result;
    }
}


KN KN_PrimaryMinus(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KN_CAR(iter);
    iter = KN_CDR(iter);
    if (first == KN_NIL) {
        return KN_MAKE_FIXNUM(0);
    }
    else if (iter == KN_NIL) {
        return first;
    }
    

    int resFixnum = 0;
    double resFlonum = 0.0;
    bool isFixNum = false;
    // the result type is determined by the first arg
    if (KN_IS_FIXNUM(first)) {
        isFixNum = true;
        resFixnum = KN_UNBOX_FIXNUM(first);
    }
    else {
        resFlonum =  KN_UNBOX_FLONUM(first);
    }
    // Unbox
    do {
        KN item = KN_CAR(iter);

        if (KN_IS_FIXNUM(item)) {
            int num = KN_UNBOX_FIXNUM(item);
            if (isFixNum) {
                resFixnum -= num;
            }
            else {
                resFlonum -= num;
            }
        }
        else if (KN_IS_FLONUM(item)) {
            double num = KN_UNBOX_FLONUM(item);
            if (isFixNum) {
                resFixnum -= num;
            }
            else {
                resFlonum -= num;
            }
        }

        iter = KN_CDR(iter);
    } while (iter != KN_NIL);

    
    if (isFixNum) {
        // TODO overflow check
        return KN_MAKE_FIXNUM(resFixnum);
    }
    else {
        KN result = KN_MAKE_FLONUM(kstate, resFlonum);
        return result;
    }
}


KN KN_PrimaryMultiply(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KN_CAR(iter);
    if (first == KN_NIL) {
        return KN_MAKE_FIXNUM(0);
    }

    int resFixnum = 1;
    double resFlonum = 1.0;
    bool isFixNum = false;
    // the result type is determined by the first arg
    if (KN_IS_FIXNUM(first)) {
        isFixNum = true;
    }
    // Unbox
    do {
        KN item = KN_CAR(iter);

        if (KN_IS_FIXNUM(item)) {
            int num = KN_UNBOX_FIXNUM(item);
            if (isFixNum) {
                resFixnum *= num;
            }
            else {
                resFlonum *= num;
            }
        }
        else if (KN_IS_FLONUM(item)) {
            double num = KN_UNBOX_FLONUM(item);
            if (isFixNum) {
                resFixnum *= num;
            }
            else {
                resFlonum *= num;
            }
        }

        iter = KN_CDR(iter);
    } while (iter != KN_NIL);

    
    if (isFixNum) {
        // TODO overflow check
        return KN_MAKE_FIXNUM(resFixnum);
    }
    else {
        KN result = KN_MAKE_FLONUM(kstate, resFlonum);
        return result;
    }
}

KN KN_PrimaryDivide(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KN_CAR(iter);
    iter = KN_CDR(iter);
    if (first == KN_NIL) {
        return KN_MAKE_FIXNUM(0);
    }
    else if (iter == KN_NIL) {
        return first;
    }
    

    int resFixnum = 0;
    double resFlonum = 0.0;
    bool isFixNum = false;
    // the result type is determined by the first arg
    if (KN_IS_FIXNUM(first)) {
        isFixNum = true;
        resFixnum = KN_UNBOX_FIXNUM(first);
    }
    else {
        resFlonum = KN_UNBOX_FLONUM(first);
    }
    // Unbox
    do {
        KN item = KN_CAR(iter);

        if (KN_IS_FIXNUM(item)) {
            int num = KN_UNBOX_FIXNUM(item);
            if (isFixNum) {
                resFixnum /= num;
            }
            else {
                resFlonum /= num;
            }
        }
        else if (KN_IS_FLONUM(item)) {
            double num = KN_UNBOX_FLONUM(item);
            if (isFixNum) {
                resFixnum /= num;
            }
            else {
                resFlonum /= num;
            }
        }

        iter = KN_CDR(iter);
    } while (iter != KN_NIL);

    
    if (isFixNum) {
        // TODO overflow check
        return KN_MAKE_FIXNUM(resFixnum);
    }
    else {
        KN result = KN_MAKE_FLONUM(kstate, resFlonum);
        return result;
    }
}


KN KN_PrimaryMod(KonState* kstate, KN first, KN second)
{
    int resFixnum = 0;
    if (KN_IS_FIXNUM(first)) {
        resFixnum = KN_UNBOX_FIXNUM(first);
    }
    else {
        resFixnum = (int)(KN_UNBOX_FLONUM(first));
    }

    int mod = 1;
    if (KN_IS_FIXNUM(first)) {
        mod = KN_UNBOX_FIXNUM(second);
    }
    else {
        mod = (int)(KN_UNBOX_FLONUM(second));
    }
 
    return KN_MAKE_FIXNUM(resFixnum % mod);
}


KN KN_PrimaryLowerThan(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KN_CAR(iter);
    if (first == KN_NIL) {
        return KN_TRUE;
    }
    iter = KN_CDR(iter);

    double lastNum = 0.0;

    if (KN_IS_FIXNUM(first)) {
        lastNum = 1.0 * KN_UNBOX_FIXNUM(first);
    }
    else {
        lastNum = KN_UNBOX_FLONUM(first);
    }

    do {
        KN item = KN_CAR(iter);

        double cmpNum = 0.0;
        if (KN_IS_FIXNUM(item)) {
            cmpNum = KN_UNBOX_FIXNUM(item);
        }
        else if (KN_IS_FLONUM(item)) {
            cmpNum = KN_UNBOX_FLONUM(item);
        }

        if (lastNum >= cmpNum) {
            return KN_FALSE;
        }
        else {
            lastNum = cmpNum;
        }

        iter = KN_CDR(iter);
    } while (iter != KN_NIL);

    
    return KN_TRUE;
}

KN KN_PrimaryLowerOrEqual(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KN_CAR(iter);
    if (first == KN_NIL) {
        return KN_TRUE;
    }
    iter = KN_CDR(iter);

    double lastNum = 0.0;

    if (KN_IS_FIXNUM(first)) {
        lastNum = 1.0 * KN_UNBOX_FIXNUM(first);
    }
    else {
        lastNum = KN_UNBOX_FLONUM(first);
    }

    do {
        KN item = KN_CAR(iter);

        double cmpNum = 0.0;
        if (KN_IS_FIXNUM(item)) {
            cmpNum = KN_UNBOX_FIXNUM(item);
        }
        else if (KN_IS_FLONUM(item)) {
            cmpNum = KN_UNBOX_FLONUM(item);
        }

        if (lastNum > cmpNum) {
            return KN_FALSE;
        }
        else {
            lastNum = cmpNum;
        }

        iter = KN_CDR(iter);
    } while (iter != KN_NIL);

    
    return KN_TRUE;
}

KN KN_PrimaryGreaterThan(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KN_CAR(iter);
    if (first == KN_NIL) {
        return KN_TRUE;
    }
    iter = KN_CDR(iter);

    double lastNum = 0.0;

    if (KN_IS_FIXNUM(first)) {
        lastNum = 1.0 * KN_UNBOX_FIXNUM(first);
    }
    else {
        lastNum = KN_UNBOX_FLONUM(first);
    }

    do {
        KN item = KN_CAR(iter);

        double cmpNum = 0.0;
        if (KN_IS_FIXNUM(item)) {
            cmpNum = KN_UNBOX_FIXNUM(item);
        }
        else if (KN_IS_FLONUM(item)) {
            cmpNum = KN_UNBOX_FLONUM(item);
        }

        if (lastNum <= cmpNum) {
            return KN_FALSE;
        }
        else {
            lastNum = cmpNum;
        }

        iter = KN_CDR(iter);
    } while (iter != KN_NIL);

    
    return KN_TRUE;
}

KN KN_PrimaryGreaterOrEqual(KonState* kstate, KN args)
{
    KN iter = args;
    KN first = KN_CAR(iter);
    if (first == KN_NIL) {
        return KN_TRUE;
    }
    iter = KN_CDR(iter);

    double lastNum = 0.0;

    if (KN_IS_FIXNUM(first)) {
        lastNum = 1.0 * KN_UNBOX_FIXNUM(first);
    }
    else {
        lastNum = KN_UNBOX_FLONUM(first);
    }

    do {
        KN item = KN_CAR(iter);

        double cmpNum = 0.0;
        if (KN_IS_FIXNUM(item)) {
            cmpNum = KN_UNBOX_FIXNUM(item);
        }
        else if (KN_IS_FLONUM(item)) {
            cmpNum = KN_UNBOX_FLONUM(item);
        }

        if (lastNum < cmpNum) {
            return KN_FALSE;
        }
        else {
            lastNum = cmpNum;
        }

        iter = KN_CDR(iter);
    } while (iter != KN_NIL);

    return KN_TRUE;
}


KonAccessor* KonNumber_Export(KonState* kstate, KonEnv* env)
{
    KN_EnvDefine(kstate, env, "+",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryPlus, 0, 1, 0)
    );
    KN_EnvDefine(kstate, env, "-",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryMinus, 0, 1, 0)
    );
    KN_EnvDefine(kstate, env, "*",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryMultiply, 0, 1, 0)
    );
    KN_EnvDefine(kstate, env, "/",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryDivide, 0, 1, 0)
    );
    KN_EnvDefine(kstate, env, "mod",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryMod, 2, 0, 0)
    );

    KN_EnvDefine(kstate, env, "<",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryLowerThan, 0, 1, 0)
    );
    KN_EnvDefine(kstate, env, "<=",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryLowerOrEqual, 0, 1, 0)
    );
    KN_EnvDefine(kstate, env, ">",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryGreaterThan, 0, 1, 0)
    );
    KN_EnvDefine(kstate, env, ">=",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryGreaterOrEqual, 0, 1, 0)
    );

    KonAccessor* slot = (KonAccessor*)KN_MakeDirAccessor(kstate, "dr", NULL);

    return slot;
}