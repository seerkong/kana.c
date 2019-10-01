#ifndef KN_PREFIX_TYPE_H
#define KN_PREFIX_TYPE_H

////
// type alias start

#if KN_64_BIT
typedef unsigned int kon_tag_t;
typedef unsigned long long kon_uint_t;
typedef long long kon_int_t;
#else
typedef unsigned short kon_tag_t;
typedef unsigned int kon_uint_t;
typedef int kon_int_t;
#endif

typedef unsigned char kon_uint8_t;
typedef unsigned int kon_uint32_t;
typedef int kon_int32_t;


// type alias end
////

#endif

