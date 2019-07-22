
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "kon_stringbuffer.h"

#define DEFAULT_CAPACITY    32
#define FIRST_STEP_LENGTH    128
#define STEP_LENGTH    512
 
#define EXPAND_BUFFER(self, n) \
    if (KonStringBuffer_Expand(self, (n))) { \
        return -1; \
    } \

struct KonStringBuffer {
    int32_t Length;
    int32_t HeadOffset; 
    int32_t BuffSize;   // include last \0
    char* BuffStart;
};
 
static int32_t KonStringBuffer_Expand(KonStringBuffer* self, int32_t min)
{
    char* newBuf = NULL;
    int32_t newBuffSize = 0;
 
    min = min + 1 - (self->BuffSize - self->HeadOffset - self->Length);
    if (min <= 0) {
        return 0;
    }
    int32_t stepLen = STEP_LENGTH;
    if (self->BuffSize < FIRST_STEP_LENGTH
        && (min + self->BuffSize) < FIRST_STEP_LENGTH
    ) {
        stepLen = FIRST_STEP_LENGTH;
    }
    newBuffSize = self->BuffSize + (min / stepLen + 1) * stepLen;
    newBuf = realloc(self->BuffStart, newBuffSize);
    if (newBuf != NULL) {
        self->BuffStart = newBuf;
        memset(self->BuffStart + self->HeadOffset + self->BuffSize, 0, newBuffSize - self->BuffSize);
        self->BuffSize = newBuffSize;
        return 0;
    }
    else {
        return -1;
    }
}

KonStringBuffer* KonStringBuffer_New()
{
    KonStringBuffer* self = NULL;
 
    self = calloc(1, sizeof(KonStringBuffer));
    if (self != NULL) {
        self->BuffStart = calloc(DEFAULT_CAPACITY, sizeof(char));
        if (self->BuffStart != NULL) {
            self->Length = 0;
            self->HeadOffset = 0;
            self->BuffSize = DEFAULT_CAPACITY;
        }
        else {
            free(self);
            self = NULL;
        }
    }
    return self;
}

void KonStringBuffer_Destroy(KonStringBuffer* self)
{
    if (self) {
        if (self->BuffStart) {
            free(self->BuffStart);
        }
        free(self);
    }
}

int32_t KonStringBuffer_Length(KonStringBuffer* self)
{
    return self ? self->Length : 0;
}

int32_t KonStringBuffer_BuffSize(KonStringBuffer* self)
{
    return self ? (self->BuffSize - 1) : 0;
}

extern const char* KonStringBuffer_Cstr(KonStringBuffer* self)
{
    return self ? (self->BuffStart + self->HeadOffset) : NULL;
}

void KonStringBuffer_Clear(KonStringBuffer* self)
{
    if (self != NULL) {
        memset(self->BuffStart, 0, self->BuffSize);
        self->Length = 0;
        self->HeadOffset = 0;
    }
}

int32_t KonStringBuffer_NAppendCstr(KonStringBuffer* self, const char* str, int32_t n)
{
    if (self == NULL) {
        return -1;
    }
 
    EXPAND_BUFFER(self, n);
    memcpy(self->BuffStart + self->HeadOffset + self->Length, str, n);
    self->Length += n;
    *(self->BuffStart + self->HeadOffset + self->Length) = '\0';
    return n;
}

int32_t KonStringBuffer_AppendCstrWithFormat(KonStringBuffer* self, const char* format, ...)
{
    int32_t n = 0;
    int32_t ret = 0;
    va_list args;
 
    if (self == NULL) {
        ret = -1;
    }
 
    
    while (1) {
        n = self->BuffSize - self->Length;
        va_start(args, format);
        ret = vsnprintf(self->BuffStart + self->HeadOffset + self->Length, n, format, args);
        va_end(args);
 
        if (ret >= 0 && ret < n) {
            self->Length += ret;
            break;
        } else if (ret < 0) {
            // error
            break;
        } else {
            // no enough space in buffer
            if (KonStringBuffer_Expand(self, ret - n)) {
                // expand failed
                ret = -1;
                break;
            }
        }
    }
    
    return ret;
}

int32_t KonStringBuffer_NAppendChar(KonStringBuffer* self, char value, int32_t num)
{
    if (self == NULL) {
        return -1;
    }
 
    EXPAND_BUFFER(self, num);
 
    memset(self->BuffStart + self->HeadOffset + self->Length, value, num);
    self->Length += num;
    *(self->BuffStart + self->HeadOffset + self->Length) = '\0';
    return num;
}


int32_t KonStringBuffer_AppendCstr(KonStringBuffer* self, const char* str)
{
    return KonStringBuffer_NAppendCstr(self, str, strlen(str));
}

int  KonStringBuffer_AppendStringBuffer(KonStringBuffer* self, KonStringBuffer* other)
{
    return KonStringBuffer_NAppendCstr(self, KonStringBuffer_Cstr(other), KonStringBuffer_Length(other));
}

// prepend n chars
int32_t KonStringBuffer_NPrependCstr(KonStringBuffer* self, const char *str, int32_t n)
{
    if (self == NULL) {
        return -1;
    }
 
    char* newBuf = NULL;
    int32_t newBuffSize = self->BuffSize;
 
    int min = n + 1 - (self->BuffSize - self->HeadOffset - self->Length);

    if (min > 0) {
        if (self->BuffSize < FIRST_STEP_LENGTH
            && (min + self->BuffSize) < FIRST_STEP_LENGTH
        ) {
            newBuffSize = self->BuffSize + FIRST_STEP_LENGTH;
        }
        else {
            newBuffSize = self->BuffSize + (min / STEP_LENGTH + 1) * STEP_LENGTH;
        }
    }

    newBuf = calloc(newBuffSize, sizeof(char));
    if (newBuf != NULL) {
        utf8ncat(newBuf, str, n);
        utf8ncat(newBuf, (self->BuffStart + self->HeadOffset), self->Length);
        self->BuffStart = newBuf;
        self->BuffSize = newBuffSize;
        self->Length += n;
        return n;
    }
    else {
        return -1;
    }
}

// prepend cstr
int32_t KonStringBuffer_PrependCstr(KonStringBuffer* self, const char *str)
{
    return KonStringBuffer_NPrependCstr(self, str, strlen(str));
}

