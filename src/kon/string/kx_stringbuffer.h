#ifndef KX_STRINGBUFFER_H
#define KX_STRINGBUFFER_H 1

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include "utf8.h"
#include <stdint.h>
#include "kx_stringview.h"


typedef struct KxStringBuffer KxStringBuffer;

// return NULL if fail
extern KxStringBuffer* KxStringBuffer_New();

extern void KxStringBuffer_Destroy(KxStringBuffer* self);

// num of stored items
extern int32_t KxStringBuffer_Length(KxStringBuffer* self);

extern int32_t KxStringBuffer_BuffSize(KxStringBuffer* self);

// Get the c string
extern const char *KxStringBuffer_Cstr(KxStringBuffer* self);

// Clear all data in the buffer
extern void KxStringBuffer_Clear(KxStringBuffer* self);

// append c string with max n chars
extern int KxStringBuffer_NAppendCstr(KxStringBuffer* self, const char *str, int32_t n);

// Add string to the buffer
// Return length of str on success, -1 on failure.
extern int KxStringBuffer_AppendCstr(KxStringBuffer* self, const char *str);

// append n char with value
extern int KxStringBuffer_NAppendChar(KxStringBuffer* self, char value, int32_t n);

extern int KxStringBuffer_AppendCstrWithFormat(KxStringBuffer* self, const char *format, ...);

extern int  KxStringBuffer_AppendStringBuffer(KxStringBuffer* self, KxStringBuffer* other);

// prepend n chars
extern int KxStringBuffer_NPrependCstr(KxStringBuffer* self, const char *str, int32_t n);;
// prepend cstr
extern int KxStringBuffer_PrependCstr(KxStringBuffer* self, const char *str);


#ifdef __cplusplus
}
#endif

#endif