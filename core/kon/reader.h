#ifndef KN_KSON_READER_H
#define KN_KSON_READER_H

#include <stdbool.h>

#include "prefix.h"
#include "value.h"
#include "tokenizer.h"
#include "state_stack.h"
#include "value_builder.h"

typedef struct
{
    Kana* kana;
    // the reader stream
    KxStringBuffer* readStream;
    bool isOwnedStream;
    StateStack* stateStack;
    BuilderStack* builderStack;

    // convert word to identifer when in quote and quasiquote scope
    bool wordAsIdentifier;

    KonTokenizer* tokenizer;

    KN root;
} KonReader;


KonReader* KSON_ReaderInit(Kana* kana);

bool KSON_ReaderFromFile(KonReader* reader, const char* sourceFilePath);
bool KSON_ReaderFromCstr(KonReader* reader, const char* sourceCstr);

KN KSON_Parse(KonReader* reader);

void KSON_ReaderCloseStream(KonReader* reader);

void KSON_ReaderExit(KonReader* reader);



#endif
