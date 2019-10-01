#ifndef NUNBOX_SHORT_STR_H
#define NUNBOX_SHORT_STR_H
/*
 * Short strings
 * -------------
 * Strings of up to 5 bytes
 * the layout of double's 8 byte
 * head head char1 char2 char3 char4 char5 \0
 * eg: abc
 * head head a b c \0 \0 \0
 */

#include "nunbox.h"

#define NUNBOX_MASK_SHORT_STR         0x0000ffffffffffffllu
#define NUNBOX_MIN_SHORT_STR_TAG      0x00040000
#define NUNBOX_MAX_SHORT_STR_TAG      0x0004ffff
#define NUNBOX_MIN_SHORT_STR          0x0004000000000000llu
#define NUNBOX_MAX_SHORT_STR          0x0004ffffffffffffllu


static inline bool NUNBOX_NAME(_IsShortStr)(NUNBOX_T val) {
    return val.asBits.tag >= NUNBOX_MIN_SHORT_STR_TAG &&
    val.asBits.tag <= NUNBOX_MAX_SHORT_STR_TAG;
}

// creates a short string of length byts with undefined contents
static inline NUNBOX_T NUNBOX_NAME(_ShortStrCreateUndef)(unsigned length) {
    NUNBOX_T val;
    val.asU64 = NUNBOX_MIN_SHORT_STR;
    val.asBits.tag = NUNBOX_MIN_SHORT_STR_TAG;
    return val;
}

// copies length bytes of chars. (nul bytes are copied like any other byte)
static inline NUNBOX_T
NUNBOX_NAME(_ShortStrCreate)(const char *chars, unsigned int length) {
    NUNBOX_T val = NUNBOX_NAME(_ShortStrCreateUndef)(length);
    
#ifdef NUNBOX_BIG_ENDIAN
    int start = 2;
    for (int i = 0; i < length; i++) {
        val.asBytes[start + i] = chars[i];
    }
#else
    int start = 0;
    for (int i = 0; i < length; i++) {
        val.asBytes[start + i] = chars[i];
    }
#endif
    assert(NUNBOX_NAME(_IsShortStr)(val));
    return val;
}


static inline char* NUNBOX_NAME(_ShortStrChars)(NUNBOX_T* val) {
    assert(NUNBOX_NAME(_IsShortStr)(*val));
#ifdef NUNBOX_BIG_ENDIAN
    return (char*)(val) + 2;
#else
    return (char*)(val);
#endif
    
}

static inline unsigned NUNBOX_NAME(_ShortStrLength)(NUNBOX_T val) {
    assert(NUNBOX_NAME(_IsShortStr)(val));
#ifdef NUNBOX_BIG_ENDIAN
    int i = 2;
    for (; i < 7; i++) {
        if (val.asBytes[i] == '\0') {
            break;
        }
    }
    return i - 2;
#else
    int i = 0;
    for (; i < 5; i++) {
        if (val.asBytes[i] == '\0') {
            break;
        }
    }
    return i;
#endif
}




#endif
