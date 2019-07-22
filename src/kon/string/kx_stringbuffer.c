
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "kx_stringbuffer.h"

#define DEFAULT_CAPACITY    32
#define FIRST_STEP_LENGTH    128
#define STEP_LENGTH    512
 
#define EXPAND_BUFFER(self, n) \
    if (KxStringBuffer_Expand(self, (n))) { \
        return -1; \
    } \

struct KxStringBuffer {
    int32_t Length;
    int32_t HeadOffset; 
    int32_t BuffSize;   // include last \0
    char* BuffStart;
};
 
static int32_t KxStringBuffer_Expand(KxStringBuffer* self, int32_t min)
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

KxStringBuffer* KxStringBuffer_New()
{
    KxStringBuffer* self = NULL;
 
    self = calloc(1, sizeof(KxStringBuffer));
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

void KxStringBuffer_Destroy(KxStringBuffer* self)
{
    if (self) {
        if (self->BuffStart) {
            free(self->BuffStart);
        }
        free(self);
    }
}

int32_t KxStringBuffer_Length(KxStringBuffer* self)
{
    return self ? self->Length : 0;
}

int32_t KxStringBuffer_BuffSize(KxStringBuffer* self)
{
    return self ? (self->BuffSize - 1) : 0;
}

extern const char* KxStringBuffer_Cstr(KxStringBuffer* self)
{
    return self ? (self->BuffStart + self->HeadOffset) : NULL;
}

void KxStringBuffer_Clear(KxStringBuffer* self)
{
    if (self != NULL) {
        memset(self->BuffStart, 0, self->BuffSize);
        self->Length = 0;
        self->HeadOffset = 0;
    }
}

int32_t KxStringBuffer_NAppendCstr(KxStringBuffer* self, const char* str, int32_t n)
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

int32_t KxStringBuffer_AppendCstrWithFormat(KxStringBuffer* self, const char* format, ...)
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
 
    memset(self->BuffStart + self->HeadOffset + self->Length, value, num);
    self->Length += num;
    *(self->BuffStart + self->HeadOffset + self->Length) = '\0';
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
int32_t KxStringBuffer_PrependCstr(KxStringBuffer* self, const char *str)
{
    return KxStringBuffer_NPrependCstr(self, str, strlen(str));
}

