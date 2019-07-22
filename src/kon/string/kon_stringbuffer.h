#ifndef KON_STRINGBUFFER_H
#define KON_STRINGBUFFER_H 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include "utf8.h"
#include <stdint.h>
#include "kon_stringview.h"


typedef struct KonStringBuffer KonStringBuffer;

// return NULL if fail
extern KonStringBuffer* KonStringBuffer_New();

extern void KonStringBuffer_Destroy(KonStringBuffer* self);

// num of stored items
extern int32_t KonStringBuffer_Length(KonStringBuffer* self);

extern int32_t KonStringBuffer_BuffSize(KonStringBuffer* self);

// Get the c string
extern const char *KonStringBuffer_Cstr(KonStringBuffer* self);

// Clear all data in the buffer
extern void KonStringBuffer_Clear(KonStringBuffer* self);

// append c string with max n chars
extern int KonStringBuffer_NAppendCstr(KonStringBuffer* self, const char *str, int32_t n);

// Add string to the buffer
// Return length of str on success, -1 on failure.
extern int KonStringBuffer_AppendCstr(KonStringBuffer* self, const char *str);

// append n char with value
extern int KonStringBuffer_NAppendChar(KonStringBuffer* self, char value, int32_t n);

extern int KonStringBuffer_AppendCstrWithFormat(KonStringBuffer* self, const char *format, ...);

extern int  KonStringBuffer_AppendStringBuffer(KonStringBuffer* self, KonStringBuffer* other);

// prepend n chars
extern int KonStringBuffer_NPrependCstr(KonStringBuffer* self, const char *str, int32_t n);;
// prepend cstr
extern int KonStringBuffer_PrependCstr(KonStringBuffer* self, const char *str);


#ifdef __cplusplus
}
#endif

#endif