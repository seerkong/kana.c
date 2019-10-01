
#ifndef nunbox_char_h
#define nunbox_char_h

/*
 * instance char
 * eg: a
 * bigendian: head head \0 a \0 \0 \0 1D
 * smallendian: 1D a \0 \0 \0 \0 head head
 */

#include "nunbox.h"

#define NUNBOX_MASK_CHAR         0x00000000000000ffllu
#define NUNBOX_CHAR_LAST_BYTE      0x1D
#define NUNBOX_CHAR_TAG      0x0


static inline bool NUNBOX_NAME(_IsChar)(NUNBOX_T val) {
    return (val.asBits.tag == NUNBOX_CHAR_TAG) && ((val.asU64 & NUNBOX_MASK_CHAR) == NUNBOX_CHAR_LAST_BYTE);
}


static inline NUNBOX_T
NUNBOX_NAME(_FromChar)(const char origin) {
    NUNBOX_T val;
    val.asU64 = NUNBOX_CHAR_LAST_BYTE;
    val.asBits.tag = NUNBOX_CHAR_TAG;
    
#ifdef NUNBOX_BIG_ENDIAN
    int start = 2;
#else
    int start = 0;
#endif
    val.asBytes[start + 1] = origin;
    val.asBytes[start + 2] = '\0';
    
    assert(NUNBOX_NAME(_IsChar)(val));
    return val;
}

static inline char NUNBOX_NAME(_ToChar)(NUNBOX_T val) {
    assert(NUNBOX_NAME(_IsChar)(val));
#ifdef NUNBOX_BIG_ENDIAN
    return *((char*)val.asBytes + 3);
#else
    return *((char*)val.asBytes + 1);
#endif
    
}

static inline char* NUNBOX_NAME(_ToStr)(NUNBOX_T val) {
    assert(NUNBOX_NAME(_IsChar)(val));
#ifdef NUNBOX_BIG_ENDIAN
    return (char*)(val.asBytes) + 3;
#else
    return (char*)(val.asBytes) + 1;
#endif
    
}


#endif /* nunbox_char_h */
