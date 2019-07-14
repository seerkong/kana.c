#ifndef KON_KSON_TOKENIZER_H
#define KON_KSON_TOKENIZER_H

#include <stdbool.h>

#include "prefix.h"
#include "node.h"

typedef enum
{
    KON_TOKEN_NONE = 0,
    KON_TOKEN_EOF,
    KON_TOKEN_WHITESPACE,
    KON_TOKEN_COMMENT_SINGLE_LINE, // //
    
    KON_TOKEN_LIST_START,   // {
    KON_TOKEN_LIST_END,     // }
    KON_TOKEN_VECTOR_START,  // [
    KON_TOKEN_VECTOR_END,    // ]
    KON_TOKEN_TABLE_START,    // (
    KON_TOKEN_TABLE_END,      // )
    KON_TOKEN_CELL_START,   // <
    KON_TOKEN_CELL_END,     // >

    KON_TOKEN_KEYWORD_NIL,  //  #nil;   list end
    KON_TOKEN_KEYWORD_NULL,  //  #null;  container placeholder
    KON_TOKEN_KEYWORD_UKN,  //  #ukn; unknown
    KON_TOKEN_KEYWORD_TRUE,  //  #t;
    KON_TOKEN_KEYWORD_FALSE,  //  #f;
    KON_TOKEN_KEYWORD_EITHER,  //  either #either,t,0.4;
    KON_TOKEN_KEYWORD_BOTH,  //  #both,0.4;

    KON_TOKEN_LITERAL_NUMBER,
    KON_TOKEN_LITERAL_STRING,   // "abc"

    KON_TOKEN_SYM_STRING,       // 'abc'
    KON_TOKEN_SYM_IDENTIFIER,   // abC
    KON_TOKEN_SYM_FORM_WORD, // !cond !define
    KON_TOKEN_SYM_VARIABLE, // $abc

    // @abc @'' @[] @() @<> @{}
    KON_TOKEN_QUOTE_IDENTIFER,
    KON_TOKEN_QUOTE_SYMBOL,
    KON_TOKEN_QUOTE_VECTOR,
    KON_TOKEN_QUOTE_LIST,
    KON_TOKEN_QUOTE_TABLE,
    KON_TOKEN_QUOTE_CELL,

    // $[] $() $<> ${}
    KON_TOKEN_QUASI_VECTOR,
    KON_TOKEN_QUASI_LIST,
    KON_TOKEN_QUASI_TABLE,
    KON_TOKEN_QUASI_CELL,

    KON_TOKEN_EXPAND_REPLACE,   // $.abc
    KON_TOKEN_EXPAND_VECTOR,     // $[].
    KON_TOKEN_EXPAND_TABLE,     // $().
    KON_TOKEN_EXPAND_LIST,      // ${}.

    KON_TOKEN_UNQUOTE_REPLACE,  // $e.abc $e.{5 .+ $.a}
    KON_TOKEN_UNQUOTE_VECTOR,    // $[]e.{[1 2 3]}
    KON_TOKEN_UNQUOTE_TABLE,    // $()e.{(:a 1 :b 2)}
    KON_TOKEN_UNQUOTE_LIST,     // ${}e.{@{1 2 3}}

    KON_TOKEN_TABLE_TAG,    // :
    
    KON_TOKEN_APPLY,        // %
    KON_TOKEN_EXEC_MSG,     // .
    KON_TOKEN_PIPE,         // |
    KON_TOKEN_CLAUSE_END,   // ;

} KonTokenKind;


typedef struct
{
    // the event
    KonTokenKind TokenKind;

    int CurrRow;
    int CurrCol;

    int RowStart;
    int RowEnd;
    int ColStart;
    int ColEnd;

    // the reader stream
    tb_stream_ref_t ReadStream;

    // the element
    tb_string_t Content;

    // 解析number用的
    bool NumIsPositive;
    tb_string_t NumBeforeDot;   // 234.
    tb_string_t NumAfterDot;    // .12
    bool NumIsPowerPositive;    // e-2
    tb_string_t NumAfterPower;  // e5

} KonTokenizer;

KonTokenizer* KSON_TokenizerInit(KonState* kstate);

bool KSON_TokenizerOpenStream(KonTokenizer* reader, tb_stream_ref_t stream);

void KSON_TokenizerCloseStream(KonTokenizer* reader);

void KSON_TokenizerExit(KonTokenizer* reader);

KonTokenKind KSON_TokenizerNext(KonTokenizer* reader);

void KSON_TokenToString(KonTokenizer* reader);

#endif