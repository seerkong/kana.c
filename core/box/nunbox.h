/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Viktor Söderqvist
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * nanbox.h
 * --------
 *
 * This file provides a is a way to store various types of data in a 64-bit
 * slot, including a type tag, using NaN-boxing.  NaN-boxing is a way to store
 * various information in unused NaN-space in the IEEE754 representation.  For
 * 64-bit platforms, unused bits in pointers are also used to encode various
 * information.  The representation in inspired by that used by Webkit's
 * JavaScriptCore.
 *
 * Datatypes that can be stored:
 *
 *   * int (int32_t)
 *   * double
 *   * obj pointer(aligned)
 *   * ext pointer(may not be aligned)
 *   * boolean (true and false)
 *   * unknown
 *   * undefined
 *   * nil
 *   * empty
 *   * deleted
 *   * aux 'auxillary data' (3 types of 48-bit values)
 *
 * Any value with the top 13 bits set represents a quiet NaN.  The remaining
 * bits are called the 'payload'. NaNs produced by hardware and C-library
 * functions typically produce a payload of zero.  We assume that all quiet
 * NaNs with a non-zero payload can be used to encode whatever we want.
 */

#ifndef NUNBOX_H
#define NUNBOX_H

/*
 * Define this before including this file to get functions and type prefixed
 * with something other than "nanbox".
 */
#ifndef NUNBOX_PREFIX
#define NUNBOX_PREFIX KnBox
#endif

/* User-defined aligned pointer type. Defaults to void*. Must be a pointer type. */
#ifndef NUNBOX_OBJ_PTR_TYPE
#define NUNBOX_OBJ_PTR_TYPE void*
//#define NUNBOX_OBJ_PTR_TYPE KonValueRef
#endif

/* User-defined pointer type, may not be aligned. Defaults to char*. Must be a pointer type. */
#ifndef NUNBOX_EXT_PTR_TYPE
#define NUNBOX_EXT_PTR_TYPE char*
#endif
#define NUNBOX_MIN_EXT_PTR  0x0005000000000000llu

/*
 * User-defined auxillary types. Default to void*. These types must be pointer
 * types or 32-bit types. (Pointers on 64-bit platforms always begin with 16
 * bits of zero.)
 */
#ifndef NUNBOX_AUX1_TYPE
#define NUNBOX_AUX1_TYPE void*
#endif
#ifndef NUNBOX_AUX2_TYPE
#define NUNBOX_AUX2_TYPE void*
#endif
#ifndef NUNBOX_AUX3_TYPE
#define NUNBOX_AUX3_TYPE void*
#endif
//#ifndef NUNBOX_AUX4_TYPE
//#define NUNBOX_AUX4_TYPE void*
//#endif
//#ifndef NUNBOX_AUX5_TYPE
//#define NUNBOX_AUX5_TYPE void*
//#endif


#include <stddef.h>  // size_t
#include <stdint.h>  // int64_t, int32_t
#include <stdbool.h> // bool, true, false
#include <string.h>  // memset
#include <assert.h>

/*
 * Macros to expand the prefix.
 */
#undef NUNBOX_XXNAME
#define NUNBOX_XXNAME(prefix, name) prefix ## name
#undef NUNBOX_XNAME
#define NUNBOX_XNAME(prefix, name) NUNBOX_XXNAME(prefix, name)
#undef NUNBOX_NAME
#define NUNBOX_NAME(name) NUNBOX_XNAME(NUNBOX_PREFIX, name)

/*
 * Detect OS and endianess.
 *
 * Most of this is inspired by WTF/wtf/Platform.h in Webkit's source code.
 */

/* Unix? */
#if defined(_AIX) \
|| defined(__APPLE__) /* Darwin */ \
|| defined(__FreeBSD__) || defined(__DragonFly__) \
|| defined(__FreeBSD_kernel__) \
|| defined(__GNU__) /* GNU/Hurd */ \
|| defined(__linux__) \
|| defined(__NetBSD__) \
|| defined(__OpenBSD__) \
|| defined(__QNXNTO__) \
|| defined(sun) || defined(__sun) /* Solaris */ \
|| defined(unix) || defined(__unix) || defined(__unix__)
#define NUNBOX_UNIX 1
#endif

/* Windows? */
#if defined(WIN32) || defined(_WIN32)
#define NUNBOX_WINDOWS 1
#endif

/* 64-bit mode? (Mostly equivallent to how WebKit does it) */
#if ((defined(__x86_64__) || defined(_M_X64)) \
&& (defined(NUNBOX_UNIX) || defined(NUNBOX_WINDOWS))) \
|| (defined(__ia64__) && defined(__LP64__)) /* Itanium in LP64 mode */ \
|| defined(__alpha__) /* DEC Alpha */ \
|| (defined(__sparc__) && defined(__arch64__) || defined (__sparcv9)) /* BE */ \
|| defined(__s390x__) /* S390 64-bit (BE) */ \
|| (defined(__ppc64__) || defined(__PPC64__)) \
|| defined(__aarch64__) /* ARM 64-bit */
#define NUNBOX_64 1
#else
#define NUNBOX_32 1
#endif

/* Big endian? (Mostly equivallent to how WebKit does it) */
#if defined(__MIPSEB__) /* MIPS 32-bit */ \
|| defined(__ppc__) || defined(__PPC__) /* CPU(PPC) - PowerPC 32-bit */ \
|| defined(__powerpc__) || defined(__powerpc) || defined(__POWERPC__) \
|| defined(_M_PPC) || defined(__PPC) \
|| defined(__ppc64__) || defined(__PPC64__) /* PowerPC 64-bit */ \
|| defined(__sparc)   /* Sparc 32bit */  \
|| defined(__sparc__) /* Sparc 64-bit */ \
|| defined(__s390x__) /* S390 64-bit */ \
|| defined(__s390__)  /* S390 32-bit */ \
|| defined(__ARMEB__) /* ARM big endian */ \
|| ((defined(__CC_ARM) || defined(__ARMCC__)) /* ARM RealView compiler */ \
&& defined(__BIG_ENDIAN))
#define NUNBOX_BIG_ENDIAN 1
#endif

/*
 * In 32-bit mode, the double is unmasked. In 64-bit mode, the pointer is
 * unmasked.
 */
union NUNBOX_NAME(_u) {
    unsigned char asBytes[sizeof(double)];
    uint64_t asU64;
//    unsigned long long asLLU;
    double asF64;
    void* asPointer;
    
#if defined(NUNBOX_64)
    NUNBOX_OBJ_PTR_TYPE asObjPtr;
#endif
    // #if defined(NUNBOX_64)
    NUNBOX_EXT_PTR_TYPE asExtPtr;
    // #endif
    
#ifdef NUNBOX_BIG_ENDIAN
    struct {
        uint32_t tag;
        uint32_t payload;
    } asBits;
#else
    struct {
        uint32_t payload;
        uint32_t tag;
    } asBits;
#endif
};

#undef NUNBOX_T
//#define NUNBOX_T NUNBOX_NAME(_t)
#define NUNBOX_T NUNBOX_PREFIX
typedef union NUNBOX_NAME(_u) NUNBOX_T;

#if defined(NUNBOX_64)

/*
 * 64-bit platforms
 *
 * This range of NaN space is represented by 64-bit numbers begining with
 * 13 bits of ones. That is, the first 16 bits are 0xFFF8 or higher.  In
 * practice, no higher value is used for NaNs.  We rely on the fact that no
 * valid double-precision numbers will be "higher" than this (compared as an
 * uint64).
 *
 * By adding 7 * 2^48 as a 64-bit integer addition, we shift the first 16 bits
 * in the doubles from the range 0000..FFF8 to the range 0007..FFFF.  Doubles
 * are decoded by reversing this operation, i.e. substracting the same number.
 *
 * The top 16-bits denote the type of the encoded nanbox_t:
 *
 *     {  0000:PPPP:PPPP:PPPP   object pointer, aligned
 *      / 0001:xxxx:xxxx:xxxx   \
 *     {           ...           Aux
 *      \ 0003:xxxx:xxxx:xxxx   /
 *     {  0004:xxxx:xxxx:xxxx   short string
 *     {  0005:PPPP:PPPP:PPPP   ext pointer, no need to be aligined
 *     {  0006:0000:IIII:IIII   integer
 *      / 0007:****:****:****   \
 *     {          ...            double
 *      \ FFFF:****:****:****   /
 *
 * 32-bit signed integers are marked with the 16-bit tag 0x0006.
 *
 * The tags 0x0001..0x0003 can be used to store five additional types of
 * 48-bit auxillary data, each storing up to 48 bits of payload.
 *
 * The tag 0x0000 denotes a pointer, or another form of tagged immediate.
 * Boolean, 'unknown', 'undefined', 'nil' and 'deleted' are represented by specific,
 * invalid pointer values:
 */
// pointer tagging system
//   bits end in
//                0000:  empty or obj pointer
//                0010:  false 0x02
//                0011:  true 0x03
//                0110:  nil 0x06
//                1110:  undefined 0x0E
//                0111:  unknown 0x07
//                1111:  deleted 0x0F
//                1001:  reserved
//                1011:  reserved
//                1101:  reserved as 8bit tag flag
//            00011101:  char
//            00101101:  type label (optional)
//            00111101:  extended label (optional)

#define KNBOX_EMPTY       0x0llu
#define KNBOX_FALSE       0x2llu
#define KNBOX_TRUE        0x3llu
#define KNBOX_NIL         0x6llu
#define KNBOX_UNDEF       0x0Ellu
#define KNBOX_UKN         0x7llu
#define KNBOX_DELETED     0x0Fllu

// #define KNBOX_EMPTY       0x0lu
// #define KNBOX_FALSE       0x2lu
// #define KNBOX_TRUE        0x3lu
// #define KNBOX_NIL         0x6lu
// #define KNBOX_UNDEF       0x0Elu
// #define KNBOX_UKN         0x7lu
// #define KNBOX_DELETED     0x0Flu

#define BOXED_KN_EMPTY       ((KN)KNBOX_EMPTY)
#define BOXED_KN_FALSE       ((KN)KNBOX_FALSE)
#define BOXED_KN_TRUE        ((KN)KNBOX_TRUE)
#define BOXED_KN_NIL         ((KN)KNBOX_NIL)
#define BOXED_KN_UNDEF       ((KN)KNBOX_UNDEF)
#define BOXED_KN_UKN         ((KN)KNBOX_UKN)
#define BOXED_KN_DELETED     ((KN)KNBOX_DELETED)

typedef double c;
#define BOXED_T_VAL(NAME, VALUE) ((NAME)BOXED_##VALUE)

// This value is 7 * 2^48, used to encode doubles such that the encoded value
// will begin with a 16-bit pattern within the range 0x0007..0xFFFF.
#define NUNBOX_DOUBLE_ENCODE_OFFSET 0x0007000000000000llu
// If the 16 first bits are 0x0002, this indicates an integer number.  Any
// larger value is a double, so we can use >= to check for either integer or
// double.
#define NUNBOX_MIN_NUMBER           0x0006000000000000llu
#define NUNBOX_HIGH16_TAG           0xffff000000000000llu

// There are 4 * 2^48 auxillary values can be stored in the 64-bit integer
// range NUNBOX_MIN_AUX..NUNBOX_MAX_AUX.
#define NUNBOX_MIN_AUX_TAG          0x00010000
#define NUNBOX_MAX_AUX_TAG          0x0004ffff
#define NUNBOX_MIN_AUX              0x0001000000000000llu
#define NUNBOX_MAX_AUX              0x0004ffffffffffffllu

// NUNBOX_MASK_POINTER  defines the allowed non-zero bits in a pointer.
#define NUNBOX_MASK_OBJ_PTR         0x0000fffffffffffcllu

#define NUNBOX_MASK_EXT_PTR         0x0000ffffffffffffllu
#define NUNBOX_MIN_EXT_PTR_TAG      0x00050000
#define NUNBOX_MAX_EXT_PTR_TAG      0x0005ffff
#define NUNBOX_MIN_EXT_PTR          0x0005000000000000llu
#define NUNBOX_MAX_EXT_PTR          0x0005ffffffffffffllu


// The 'empty' value is guarranteed to consist of a repeated single byte,
// so that it should be easy to memset an array of nanboxes to 'empty' using
// KNBOX_EMPTY_BYTE as the value for every byte.
#define KNBOX_EMPTY_BYTE           0x0

// Define bool nanbox_is_xxx(NUNBOX_T val) and NUNBOX_T nanbox_xxx(void)
// with empty, deleted, true, false, undefined and unknown substituted for xxx.
#define NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(NAME, VALUE)                \
static inline NUNBOX_T NUNBOX_NAME(_##NAME)(void) {        \
NUNBOX_T val;                                        \
val.asU64 = VALUE;                                \
return val;                                          \
}                                                            \
static inline bool NUNBOX_NAME(_Is##NAME)(NUNBOX_T val) { \
return val.asU64 == VALUE;                        \
}
NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(Empty, KNBOX_EMPTY)
NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(Deleted, KNBOX_DELETED)
NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(False, KNBOX_FALSE)
NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(True, KNBOX_TRUE)
NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(Undefined, KNBOX_UNDEF)
NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(Unknown, KNBOX_UKN)
NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(Nil, KNBOX_NIL)


static inline bool NUNBOX_NAME(_IsNilOrUndefined)(NUNBOX_T val) {
    // undefined and nil are the same if we remove the 'undefined' bit.
    return (val.asU64 & ~8) == KNBOX_NIL;
}

static inline bool NUNBOX_NAME(_IsBoolean)(NUNBOX_T val) {
    // True and false are the same if we remove the 'true' bit.
    return (val.asU64 & ~1) == KNBOX_FALSE;
}
static inline bool NUNBOX_NAME(_ToBoolean)(NUNBOX_T val) {
    assert(NUNBOX_NAME(_IsBoolean)(val));
    return val.asU64 & 1;
}
static inline NUNBOX_T NUNBOX_NAME(_FromBoolean)(bool b) {
    NUNBOX_T val;
    val.asU64 = b ? KNBOX_TRUE : KNBOX_FALSE;
    return val;
}

/* true if val is a double or an int */
static inline bool NUNBOX_NAME(_IsNumber)(NUNBOX_T val) {
    return val.asU64 >= NUNBOX_MIN_NUMBER;
}

static inline bool NUNBOX_NAME(_IsInt)(NUNBOX_T val) {
    return (val.asU64 & NUNBOX_HIGH16_TAG) == NUNBOX_MIN_NUMBER;
}
static inline NUNBOX_T NUNBOX_NAME(_FromInt)(int32_t i) {
    NUNBOX_T val;
    val.asU64 = NUNBOX_MIN_NUMBER | (uint32_t)i;
    return val;
}
static inline int32_t NUNBOX_NAME(_ToInt)(NUNBOX_T val) {
    assert(NUNBOX_NAME(_IsInt)(val));
    return (int32_t)val.asU64;
}

static inline bool NUNBOX_NAME(_IsDouble)(NUNBOX_T val) {
    return NUNBOX_NAME(_IsNumber)(val) && !NUNBOX_NAME(_IsInt)(val);
}
static inline NUNBOX_T NUNBOX_NAME(_FromDouble)(double d) {
    NUNBOX_T val;
    val.asF64 = d;
    val.asU64 += NUNBOX_DOUBLE_ENCODE_OFFSET;
    assert(NUNBOX_NAME(_IsDouble)(val));
    return val;
}
static inline double NUNBOX_NAME(_ToDouble)(NUNBOX_T val) {
    assert(NUNBOX_NAME(_IsDouble)(val));
    val.asU64 -= NUNBOX_DOUBLE_ENCODE_OFFSET;
    return val.asF64;
}

static inline bool NUNBOX_NAME(_IsObjPtr)(NUNBOX_T val) {
    return !(val.asU64 & ~NUNBOX_MASK_OBJ_PTR) && val.asU64;
}
static inline NUNBOX_OBJ_PTR_TYPE NUNBOX_NAME(_ToObjPtr)(NUNBOX_T val) {
    assert(NUNBOX_NAME(_IsObjPtr)(val));
    return val.asObjPtr;
}
static inline NUNBOX_T NUNBOX_NAME(_FromObjPtr)(NUNBOX_OBJ_PTR_TYPE pointer) {
    NUNBOX_T val;
    val.asObjPtr = pointer;
    assert(NUNBOX_NAME(_IsObjPtr)(val));
    return val;
}


static inline bool NUNBOX_NAME(_IsExtPtr)(NUNBOX_T val) {
    return val.asBits.tag >= NUNBOX_MIN_EXT_PTR_TAG &&
    val.asBits.tag <= NUNBOX_MAX_EXT_PTR_TAG;
}
static inline NUNBOX_EXT_PTR_TYPE NUNBOX_NAME(_ToExtPtr)(NUNBOX_T val) {
    return (NUNBOX_EXT_PTR_TYPE)(val.asU64 & NUNBOX_MASK_EXT_PTR);
}
static inline NUNBOX_T NUNBOX_NAME(_FromExtPtr)(NUNBOX_EXT_PTR_TYPE pointer) {
    NUNBOX_T val;
    val.asU64 = NUNBOX_MIN_EXT_PTR | (uint64_t)pointer;
    return val;
}

static inline bool NUNBOX_NAME(_IsAux)(NUNBOX_T val) {
    return val.asU64 >= NUNBOX_MIN_AUX &&
    val.asU64 <= NUNBOX_MAX_AUX;
}

/* end if NUNBOX_64 */
#elif defined(NUNBOX_32)

/*
 * On 32-bit platforms we use the following NaN-boxing scheme:
 *
 * For values that do not contain a double value, the high 32 bits contain the
 * tag values listed below, which all correspond to NaN-space. When the tag is
 * 'pointer', 'integer' and 'boolean', their values (the 'payload') are store
 * in the lower 32 bits. In the case of all other tags the payload is 0.
 */
#define NUNBOX_MAX_DOUBLE_TAG     0xfff80000
#define NUNBOX_INT_TAG            0xfff80001
#define NUNBOX_MIN_AUX_TAG        0xfff90000
#define NUNBOX_MAX_AUX_TAG        0xfffdffff
#define NUNBOX_POINTER_TAG        0xfffffffa
#define NUNBOX_BOOLEAN_TAG        0xfffffffb
#define KNBOX_UNDEFINED_TAG      0xfffffffc
#define NUNBOX_UNKNOWN_TAG           0xfffffffd
#define KNBOX_DELETED_VALUE_TAG  0xfffffffe
#define KNBOX_EMPTY_VALUE_TAG    0xffffffff

// The 'empty' value is guarranteed to consist of a repeated single byte,
// so that it should be easy to memset an array of nanboxes to 'empty' using
// KNBOX_EMPTY_BYTE as the value for every byte.
#define KNBOX_EMPTY_BYTE 0xff

/* The minimum uint64_t value for the auxillary range */
#define NUNBOX_MIN_AUX            0xfff9000000000000llu
#define NUNBOX_MAX_AUX            0xfffdffffffffffffllu

// Define nanbox_xxx and nanbox_is_xxx for deleted, undefined and unknown.
#define NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(NAME, TAG)                   \
static inline NUNBOX_T NUNBOX_NAME(_##NAME)(void) {       \
NUNBOX_T val;                                         \
val.asBits.tag = TAG;                                \
val.asBits.payload = 0;                              \
return val;                                           \
}                                                             \
static inline bool NUNBOX_NAME(_Is##NAME)(NUNBOX_T val) {  \
return val.asBits.tag == TAG;                        \
}

//NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(Empty, KNBOX_EMPTY)
NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(Deleted, KNBOX_DELETED)
NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(False, KNBOX_FALSE)
NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(True, KNBOX_TRUE)
NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(Undefined, KNBOX_UNDEF)
NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(Unknown, KNBOX_UKN)
NUNBOX_IMMIDIATE_VALUE_FUNCTIONS(Nil, KNBOX_NIL)

// The undefined and unknown tags differ only in one bit
static inline bool NUNBOX_NAME(_IsNilOrUndefined)(NUNBOX_T val) {
    return (val.asBits.tag & ~1) == KNBOX_UNDEFINED_TAG;
}

static inline NUNBOX_T NUNBOX_NAME(_Empty)(void) {
    NUNBOX_T val;
    val.asU64 = 0xffffffffffffffffllu;
    return val;
}
static inline bool NUNBOX_NAME(_IsEmpty)(NUNBOX_T val) {
    return val.asBits.tag == 0xffffffff;
}

/* Returns true if the value is auxillary space */
static inline bool NUNBOX_NAME(_IsAux)(NUNBOX_T val) {
    return val.asBits.tag >= NUNBOX_MIN_AUX_TAG &&
    val.asBits.tag < NUNBOX_POINTER_TAG;
}

// Define nanbox_is_yyy, nanbox_to_yyy and nanbox_from_yyy for
// boolean, int, pointer and aux1-aux5
#define NUNBOX_TAGGED_VALUE_FUNCTIONS(NAME, TYPE, TAG) \
static inline bool NUNBOX_NAME(_Is##NAME)(NUNBOX_T val) { \
return val.asBits.tag == TAG; \
} \
static inline TYPE NUNBOX_NAME(_To##NAME)(NUNBOX_T val) { \
assert(val.asBits.tag == TAG); \
return (TYPE)val.asBits.payload; \
} \
static inline NUNBOX_T NUNBOX_NAME(_From##NAME)(TYPE a) { \
NUNBOX_T val; \
val.asBits.tag = TAG; \
val.asBits.payload = (int32_t)a; \
return val; \
}

NUNBOX_TAGGED_VALUE_FUNCTIONS(Boolean, bool, NUNBOX_BOOLEAN_TAG)
NUNBOX_TAGGED_VALUE_FUNCTIONS(Int, int32_t, NUNBOX_INT_TAG)
NUNBOX_TAGGED_VALUE_FUNCTIONS(ObjPtr, NUNBOX_OBJ_PTR_TYPE, NUNBOX_POINTER_TAG)

static inline NUNBOX_T NUNBOX_NAME(_True)(void) {
    return NUNBOX_NAME(_FromBoolean)(true);
}
static inline NUNBOX_T NUNBOX_NAME(_False)(void) {
    return NUNBOX_NAME(_FromBoolean)(false);
}
static inline bool NUNBOX_NAME(_IsTrue)(NUNBOX_T val) {
    return val.asBits.tag == NUNBOX_BOOLEAN_TAG && val.asBits.payload;
}
static inline bool NUNBOX_NAME(_IsFalse)(NUNBOX_T val) {
    return val.asBits.tag == NUNBOX_BOOLEAN_TAG && !val.asBits.payload;
}

static inline bool NUNBOX_NAME(_IsDouble)(NUNBOX_T val) {
    return val.asBits.tag < NUNBOX_INT_TAG;
    }
    // is number = is double or is int
    static inline bool NUNBOX_NAME(_IsNumber)(NUNBOX_T val) {
        return val.asBits.tag <= NUNBOX_INT_TAG;
    }
    
    static inline NUNBOX_T NUNBOX_NAME(_FromDouble)(double d) {
        NUNBOX_T val;
        val.asF64 = d;
        assert(NUNBOX_NAME(_IsDouble)(val) &&
               val.asBits.tag <= NUNBOX_MAX_DOUBLE_TAG);
        return val;
    }
    static inline double NUNBOX_NAME(_ToDouble)(NUNBOX_T val) {
        assert(NUNBOX_NAME(_IsDouble)(val));
        return val.asF64;
    }
    
#endif /* elif NUNBOX_32 */
    
    /*
     * Representation-independent functions
     */
    
    static inline double NUNBOX_NAME(_ToNumber)(NUNBOX_T val) {
        assert(NUNBOX_NAME(_IsNumber)(val));
        return NUNBOX_NAME(_IsInt)(val) ? NUNBOX_NAME(_ToInt)(val)
        : NUNBOX_NAME(_ToDouble)(val);
    }
    
#endif /* NUNBOX_H */
