#ifndef KN_KSON_TOKENIZER_H
#define KN_KSON_TOKENIZER_H

#include <stdbool.h>

#include "prefix.h"
#include "value.h"

typedef enum
{
    KN_TOKEN_NONE = 0,
    KN_TOKEN_EOF,
    KN_TOKEN_WHITESPACE,
    KN_TOKEN_UNIX_SCRIPT_FIST_LINE, // #!/XX/XX/X
    KN_TOKEN_COMMENT_SINGLE_LINE, // ``
    KN_TOKEN_COMMENT_MULTI_LINE, // `* *`
    
    KN_TOKEN_LIST_START,   // {
    KN_TOKEN_BLOCK_START,   // #[   TODO remove this token, use @blk.{} instead
    KN_TOKEN_LIST_END,     // }
    KN_TOKEN_VECTOR_START,  // <
    KN_TOKEN_ARRAY_START,  // #<   TODO add array start token
    KN_TOKEN_VECTOR_END,    // >
    KN_TOKEN_TABLE_START,    // #(
    KN_TOKEN_PARAM_START,    // (
    KN_TOKEN_TABLE_END,      // )
    KN_TOKEN_CELL_START,   // [     TODO change to sentence start
    KN_TOKEN_MAP_START,   // #[     TODO change to cell data wrapper start
    KN_TOKEN_CELL_END,     // ]

    KN_TOKEN_KEYWORD_NIL,  //  nil   list end
    KN_TOKEN_KEYWORD_UNDEF,  // undefined
    KN_TOKEN_KEYWORD_UKN,  //  ukn unknown, container placeholder
    KN_TOKEN_KEYWORD_TRUE,  // true
    KN_TOKEN_KEYWORD_FALSE,  // false

    // TODO abandon token either and both
    KN_TOKEN_KEYWORD_EITHER,  //  either %either,t,0.4; or ^either,t,0.4;
    KN_TOKEN_KEYWORD_BOTH,  //  %both,0.4; or ^both,0.4;

    KN_TOKEN_LITERAL_NUMBER,
    KN_TOKEN_LITERAL_STRING,   // "abc"
    KN_TOKEN_LITERAL_RAW_STRING,       // 'abc'

    KN_TOKEN_PREFIX_WRAPPER, // !await
    KN_TOKEN_SUFFIX_WRAPPER, // ~await

    KN_TOKEN_SYM_MARCRO, // each!
    KN_TOKEN_SYM_CELL_SEG_END, // {text ^text} or {text %text}
    KN_TOKEN_SYM_WORD,   // abC
    
    KN_TOKEN_SYM_STRING, // $''
    KN_TOKEN_SYM_VARIABLE, // @.abc
    KN_TOKEN_SYM_IDENTIFIER,  // $.abc

    KN_TOKEN_QUOTE,   // $.[] $.{}

    KN_TOKEN_QUASI,   // @.[] @.{}

    // TODO abandon thess three token
    KN_TOKEN_EXPAND_REPLACE,   // @^abc @^{+ a 2} or @%abc
    KN_TOKEN_EXPAND_KV,        // @%abc or @~abc
    KN_TOKEN_EXPAND_SEQ,        // @~abc or @^abc

    KN_TOKEN_UNQUOTE_REPLACE,  // $^abc $^{+ a 2} or $%abc
    KN_TOKEN_UNQUOTE_KV,        // $%abc or $~abc
    KN_TOKEN_UNQUOTE_SEQ,        // $~abc or $^abc

    KN_TOKEN_KV_PAIR_TAG,    // :
    
    KN_TOKEN_CHAR,          // %c,xxx; or ^c,xxx;
    KN_TOKEN_TEXT_BUILDER,   // %xxx.'abc' or ^xxx.'abc'
    KN_TOKEN_APPLY,        // % [write % 1 2 3]
    KN_TOKEN_REST,     // ... like scheme . eg [func (a ... b) { }]
    KN_TOKEN_CLAUSE_END,   // ;
    KN_TOKEN_EQUAL,   //  {= 1 2}
    KN_TOKEN_ASSIGN,   // [a ff := 22] TODO abandon
    // TODO change select path(\abc), exec msg(.length), pipe proc(|abc)
    // to wrapper type
    KN_TOKEN_GET_LVALUE, // \tag
    KN_TOKEN_GET_RVALUE, // \:tag
    KN_TOKEN_MSG_SIGNAL,     // . length
    KN_TOKEN_PROC_PIPE,         // | abc

} KonTokenKind;


typedef struct
{
    // the event
    KonTokenKind tokenKind;

    int currRow;
    int currCol;

    int rowStart;
    int rowEnd;
    int colStart;
    int colEnd;

    // full code string
    KxStringBuffer* codeString;
    int codeLen;
    int readCursor;

    // the element
    KxStringBuffer* content;

    // 解析number用的
    bool numIsPositive;
    KxStringBuffer* numBeforeDot;   // 234.
    KxStringBuffer* numAfterDot;    // .12
    bool numIsPowerPositive;    // e-2
    KxStringBuffer* numAfterPower;  // e5

} KonTokenizer;

KonTokenizer* KSON_TokenizerInit(KonState* kstate);

bool KSON_TokenizerBegin(KonTokenizer* reader, KxStringBuffer* codeString);

void KSON_TokenizerEnd(KonTokenizer* reader);

void KSON_TokenizerExit(KonTokenizer* reader);

KonTokenKind KSON_TokenizerNext(KonTokenizer* reader);

void KSON_TokenToString(KonTokenizer* reader);

#endif