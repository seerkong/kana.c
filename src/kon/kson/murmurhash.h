
/**
 * `murmurhash.h' - murmurhash
 *
 * copyright (c) 2014 joseph werle <joseph.werle@gmail.com>
 */

#ifndef MURMURHASH_H
#define MURMURHASH_H 1



#define MURMURHASH_VERSION "0.0.3"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * Returns a murmur hash of `key' based on `seed'
 * using the MurmurHash3 algorithm
 */

uint32_t MurmurHash32(const char *, uint32_t, uint32_t);

uint64_t MurmurHash64A( const void * key, int len, unsigned int seed );
#ifdef __cplusplus
}
#endif

#endif