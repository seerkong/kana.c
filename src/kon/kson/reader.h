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

    KonTokenizer* Tokenizer;

    KN Root;
} KonReader;

typedef struct
{
    unsigned int Type;

} KonContainerBuilder;

// state
typedef enum
{
    // s0
    KON_READER_ROOT = 0,
    // s1 parse arr, need value or arr end
    KON_READER_PARSE_VECTOR,
    // s2 parse list, need value or list end
    KON_READER_PARSE_LIST,
    // s3 parse table, need ':' or table end
    KON_READER_PARSE_TABLE,
    // s4 parse table key, need identifier, symbol('') or $abc
    KON_READER_PARSE_TABLE_KEY,
    // s5 parse table value or next kv pair
    KON_READER_PARSE_TABLE_VAL_OR_NEXT,
    // s6 parse cell, need tag, identifier, symbol('') or $abc, or [, <, (
    KON_READER_PARSE_CELL_TAG,
    // s7 parse cell inner array, table, or list
    KON_READER_PARSE_CELL_INNER_CONTAINER,
    // // s8 parse cell table
    // KON_READER_PARSE_CELL_INNER_TABLE,
    // // s9 parse cell arr
    // KON_READER_PARSE_CELL_INNER_VECTOR,
    // // s10 parse cell list
    // KON_READER_PARSE_CELL_INNER_LIST,

    // s11 parse tpl expand need identifier $.abc
    KON_READER_PARSE_EXPAND_REPLACE,
    // s12 parse tpl expand to arr, need arr or arr var $[].
    KON_READER_PARSE_EXPAND_VECTOR,
    // s13 parse tpl expand to table, need table or table var ${}.
    KON_READER_PARSE_EXPAND_LIST,
    // s14 parse tpl expand to list, need list or list var $().
    KON_READER_PARSE_EXPAND_TABLE,
    

    // s15 parse tpl expr var slot, need identifier or list
    // 4 types : $e. $[]e. $()e. ${}e.
    KON_READER_PARSE_UNQUOTE,
    // s16 quasiquote container $[ $( $< ${ need [ ( < {
    KON_READER_PARSE_QUASIQUOTE,
    // s17 quote, need identifier or [ ( < { .eg @abc @[ @( @< @{ 
    KON_READER_PARSE_QUOTE,
} KonReaderState;

KonReader* KSON_ReaderInit(KonState* kstate);

bool KSON_ReaderOpenStream(KonReader* reader, tb_stream_ref_t stream, bool isOwnedStream);

KN KSON_Parse(KonReader* reader);

void KSON_ReaderCloseStream(KonReader* reader);

void KSON_ReaderExit(KonReader* reader);



#endif
