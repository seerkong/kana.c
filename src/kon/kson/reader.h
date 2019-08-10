#ifndef KON_KSON_READER_H
#define KON_KSON_READER_H

#include <stdbool.h>

#include "prefix.h"
#include "node.h"
#include "tokenizer.h"

#include "state_stack.h"
#include "value_builder.h"

typedef struct
{
    KonState* Kstate;
    // the reader stream
    tb_stream_ref_t ReadStream;
    bool IsOwnedStream;
    StateStack* StateStack;
    BuilderStack * BuilderStack;

    // convert word to identifer when in quote and quasiquote scope
    bool WordAsIdentifier;

    KonTokenizer* Tokenizer;

    KN Root;
} KonReader;


KonReader* KSON_ReaderInit(KonState* kstate);

bool KSON_ReaderOpenStream(KonReader* reader, tb_stream_ref_t stream, bool isOwnedStream);

KN KSON_Parse(KonReader* reader);

void KSON_ReaderCloseStream(KonReader* reader);

void KSON_ReaderExit(KonReader* reader);



#endif
