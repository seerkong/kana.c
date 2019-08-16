#include "kon_number.h"

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


KonAttrSlot* KonNumber_Export(KonState* kstate, KonEnv* env)
{
    KON_EnvDefine(kstate, env, "+",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryPlus)
    );
    KON_EnvDefine(kstate, env, "-",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryMinus)
    );
    KON_EnvDefine(kstate, env, "*",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryMultiply)
    );
    KON_EnvDefine(kstate, env, "/",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryDivide)
    );
    KON_EnvDefine(kstate, env, "mod",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryMod)
    );

    KON_EnvDefine(kstate, env, "<<",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryLowerThan)
    );
    KON_EnvDefine(kstate, env, "<=",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryLowerOrEqual)
    );
    KON_EnvDefine(kstate, env, ">>",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryGreaterThan)
    );
    KON_EnvDefine(kstate, env, ">=",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryGreaterOrEqual)
    );

    KonAttrSlot* slot = (KonAttrSlot*)MakeAttrSlotFolder(kstate, "");

    return slot;
}