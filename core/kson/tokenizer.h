#ifndef KN_KSON_TOKENIZER_H
#define KN_KSON_TOKENIZER_H

#include <stdbool.h>

#include "prefix.h"
#include "../node.h"
#include "../string/kx_stringbuffer.h"

typedef enum
{
    KN_TOKEN_NONE = 0,
    KN_TOKEN_EOF,
    KN_TOKEN_WHITESPACE,
    KN_TOKEN_UNIX_SCRIPT_FIST_LINE, // #!/XX/XX/X
    KN_TOKEN_COMMENT_SINGLE_LINE, // ``
    KN_TOKEN_COMMENT_MULTI_LINE, // `* *`
    
    KN_TOKEN_LIST_START,   // [
    KN_TOKEN_BLOCK_START,   // #[
    KN_TOKEN_LIST_END,     // ]
    KN_TOKEN_VECTOR_START,  // <
    KN_TOKEN_VECTOR_END,    // >
    KN_TOKEN_TABLE_START,    // #(
    KN_TOKEN_PARAM_START,    // (
    KN_TOKEN_TABLE_END,      // )
    KN_TOKEN_CELL_START,   // {
    KN_TOKEN_CELL_END,     // }

    KN_TOKEN_KEYWORD_NIL,  //  #nil;   list end
    KN_TOKEN_KEYWORD_UNDEF,  //  #undef;
    KN_TOKEN_KEYWORD_UKN,  //  #ukn; unknown, container placeholder
    KN_TOKEN_KEYWORD_TRUE,  //  #t;
    KN_TOKEN_KEYWORD_FALSE,  //  #f;
    KN_TOKEN_KEYWORD_EITHER,  //  either #either,t,0.4;
    KN_TOKEN_KEYWORD_BOTH,  //  #both,0.4;

    KN_TOKEN_LITERAL_NUMBER,
    KN_TOKEN_LITERAL_STRING,   // "abc"
    KN_TOKEN_LITERAL_RAW_STRING,       // 'abc'

    KN_TOKEN_SYM_PREFIX_WORD, // !cond !define
    KN_TOKEN_SYM_SUFFIX_WORD, // ^await
    KN_TOKEN_SYM_WORD,   // abC
    KN_TOKEN_SYM_VARIABLE, // @abc
    KN_TOKEN_SYM_IDENTIFIER,  // $abc
    KN_TOKEN_SYM_STRING, // $''



    KN_TOKEN_QUOTE_LIST,   // $[]
    KN_TOKEN_QUOTE_CELL,   // ${}

    KN_TOKEN_QUASI_LIST,   // @[]
    KN_TOKEN_QUASI_CELL,   // @{}

    KN_TOKEN_EXPAND_REPLACE,   // $.abc
    KN_TOKEN_EXPAND_KV,        // $%.abc
    KN_TOKEN_EXPAND_SEQ,        // $~.abc

    KN_TOKEN_UNQUOTE_REPLACE,  // @.abc @.[5 .+ $.a]
    KN_TOKEN_UNQUOTE_KV,        // @%.abc
    KN_TOKEN_UNQUOTE_SEQ,        // @~.abc

    KN_TOKEN_TABLE_TAG,    // :
    
    KN_TOKEN_APPLY,        // %
    KN_TOKEN_REST,     // ... like scheme . eg [func [a ... b]]
    KN_TOKEN_CLAUSE_END,   // ;
    KN_TOKEN_EQUAL,   //  [= 1 2]
    KN_TOKEN_ASSIGN,   // {a ff := 22}
    // TODO change select path(/abc), exec msg(.length), pipe proc(|abc)
    // to wrapper type
    KN_TOKEN_GET_SLOT, // /tag /. /.. /~
    KN_TOKEN_MSG_SIGNAL,     // . length
    KN_TOKEN_PROC_PIPE,         // | abc

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

    // full code string
    KxStringBuffer* CodeString;
    int CodeLen;
    int ReadCursor;

    // the element
    KxStringBuffer* Content;

    // 解析number用的
    bool NumIsPositive;
    KxStringBuffer* NumBeforeDot;   // 234.
    KxStringBuffer* NumAfterDot;    // .12
    bool NumIsPowerPositive;    // e-2
    KxStringBuffer* NumAfterPower;  // e5

} KonTokenizer;

KonTokenizer* KSON_TokenizerInit(KonState* kstate);

bool KSON_TokenizerBegin(KonTokenizer* reader, KxStringBuffer* codeString);

void KSON_TokenizerEnd(KonTokenizer* reader);

void KSON_TokenizerExit(KonTokenizer* reader);

KonTokenKind KSON_TokenizerNext(KonTokenizer* reader);

void KSON_TokenToString(KonTokenizer* reader);

#endif