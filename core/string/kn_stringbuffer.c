
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "kn_stringbuffer.h"
#include "../../lib/tbox/tbox.h"

#define DEFAULT_CAPACITY    16
#define FIRST_STEP_LENGTH    128
#define STEP_LENGTH    512
// reserve some chars at the beginning for prepend
#define INIT_LEFT_RESERVE   2
 
#define EXPAND_BUFFER(self, n) \
    if (KxStringBuffer_Expand(self, (n))) { \
        return -1; \
    } \

struct KxStringBuffer {
    int32_t length;
    int32_t headOffset; 
    int32_t buffSize;   // include last \0
    char* buffStart;
};
 
static int32_t KxStringBuffer_Expand(KxStringBuffer* self, int32_t min)
{
    char* newBuf = NULL;
    int32_t newBuffSize = 0;
 
    min = min + 1 - (self->buffSize - self->headOffset - self->length);
    if (min <= 0) {
        return 0;
    }
    int32_t stepLen = STEP_LENGTH;
    if (self->buffSize < FIRST_STEP_LENGTH
        && (min + self->buffSize) < FIRST_STEP_LENGTH
    ) {
        stepLen = FIRST_STEP_LENGTH;
    }
    newBuffSize = self->buffSize + (min / stepLen + 1) * stepLen;
    newBuf = tb_ralloc(self->buffStart, newBuffSize);
    if (newBuf != NULL) {
        self->buffStart = newBuf;
        memset(self->buffStart + self->headOffset + self->buffSize, 0, newBuffSize - self->buffSize);
        self->buffSize = newBuffSize;
        return 0;
    }
    else {
        return -1;
    }
}

KxStringBuffer* KxStringBuffer_New()
{
    KxStringBuffer* self = NULL;
 
    self = tb_nalloc0(1, sizeof(KxStringBuffer));
    if (self != NULL) {
        self->buffStart = tb_nalloc0(DEFAULT_CAPACITY, sizeof(char));
        if (self->buffStart != NULL) {
            self->length = 0;
            self->headOffset = INIT_LEFT_RESERVE;
            self->buffSize = DEFAULT_CAPACITY;
        }
        else {
            tb_free(self);
            self = NULL;
        }
    }
    return self;
}

void KxStringBuffer_Destroy(KxStringBuffer* self)
{
    if (self) {
        if (self->buffStart) {
            tb_free(self->buffStart);
        }
        tb_free(self);
    }
}

int32_t KxStringBuffer_Length(KxStringBuffer* self)
{
    return self ? self->length : 0;
}

int32_t KxStringBuffer_BuffSize(KxStringBuffer* self)
{
    return self ? (self->buffSize - 1) : 0;
}

extern const char* KxStringBuffer_Cstr(KxStringBuffer* self)
{
    return self ? (self->buffStart + self->headOffset) : NULL;
}

const char KxStringBuffer_CharAt(KxStringBuffer* self, int index)
{
    if (index >= self->length) {
        return '\0';
    }
    else {
        return *(self->buffStart + self->headOffset + index);
    }
}

void KxStringBuffer_Clear(KxStringBuffer* self)
{
    if (self != NULL) {
        memset(self->buffStart, 0, self->buffSize);
        self->length = 0;
        self->headOffset = 0;
    }
}

int32_t KxStringBuffer_NAppendCstr(KxStringBuffer* self, const char* str, int32_t n)
{
    if (self == NULL) {
        return -1;
    }
 
    EXPAND_BUFFER(self, n);
    memcpy(self->buffStart + self->headOffset + self->length, str, n);
    self->length += n;
    *(self->buffStart + self->headOffset + self->length) = '\0';
    return n;
}

int32_t KxStringBuffer_AppendCstrWithFormat(KxStringBuffer* self, const char* format, ...)
{
    int32_t n = 0;
    int32_t ret = 0;
    va_list args;
 
    if (self == NULL) {
        ret = -1;
    }
 
    
    while (1) {
        n = self->buffSize - self->length;
        va_start(args, format);
        ret = vsnprintf(self->buffStart + self->headOffset + self->length, n, format, args);
        va_end(args);
 
        if (ret >= 0 && ret < n) {
            self->length += ret;
            break;
        } else if (ret < 0) {
            // error
            break;
        } else {
            // no enough space in buffer
            if (KxStringBuffer_Expand(self, ret - n)) {
                // expand failed
                ret = -1;
                break;
            }
        }
    }
    
    return ret;
}

int32_t KxStringBuffer_NAppendChar(KxStringBuffer* self, char value, int32_t num)
{
    if (self == NULL) {
        return -1;
    }
 
    EXPAND_BUFFER(self, num);
 
    memset(self->buffStart + self->headOffset + self->length, value, num);
    self->length += num;
    *(self->buffStart + self->headOffset + self->length) = '\0';
    return num;
}


int32_t KxStringBuffer_AppendCstr(KxStringBuffer* self, const char* str)
{
    return KxStringBuffer_NAppendCstr(self, str, strlen(str));
}

int  KxStringBuffer_AppendStringBuffer(KxStringBuffer* self, KxStringBuffer* other)
{
    return KxStringBuffer_NAppendCstr(self, KxStringBuffer_Cstr(other), KxStringBuffer_Length(other));
}

// prepend n chars
int32_t KxStringBuffer_NPrependCstr(KxStringBuffer* self, const char *str, int32_t n)
{
    if (self == NULL) {
        return -1;
    }

    // if have slots at head
    if (self->headOffset >= n) {
        char* startPtr = self->buffStart + self->headOffset - n;
        for (int i = 0; i < n; i++) {
            *(startPtr + i) = str[i];
        }
        
        self->headOffset -= n;
        self->length += n;
        return n;
    }
 
    char* newBuf = NULL;
    int32_t newBuffSize = self->buffSize;
 
    int min = n + 1 - (self->buffSize - self->headOffset - self->length);

    if (min > 0) {
        if (self->buffSize < FIRST_STEP_LENGTH
            && (min + self->buffSize) < FIRST_STEP_LENGTH
        ) {
            newBuffSize = self->buffSize + FIRST_STEP_LENGTH;
        }
        else {
            newBuffSize = self->buffSize + (min / STEP_LENGTH + 1) * STEP_LENGTH;
        }
    }

    newBuf = tb_nalloc0(newBuffSize, sizeof(char));

    if (newBuf != NULL) {
        utf8ncat(newBuf, str, n);
        utf8ncat(newBuf, (self->buffStart + self->headOffset), self->length);
        self->buffStart = newBuf;
        self->headOffset = 0;
        self->buffSize = newBuffSize;
        self->length += n;
        return n;
    }
    else {
        return -1;
    }
}

// prepend cstr
int32_t KxStringBuffer_PrependCstr(KxStringBuffer* self, const char *str)
{
    return KxStringBuffer_NPrependCstr(self, str, strlen(str));
}

const char* KxStringBuffer_OffsetPtr(KxStringBuffer* self, int startOffset)
{
    return (self->buffStart + self->headOffset + startOffset);
}