#include "tokenizer.h"
#include <tbox/tbox.h>
#include <ctype.h>


void KSON_TokenToString(KonTokenizer* tokenizer)
{
    KonStringBuffer* tokenKind = KonStringBuffer_New();

    switch (tokenizer->TokenKind) {
        case KON_TOKEN_EOF:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_EOF");
            break;
        case KON_TOKEN_LIST_START:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_LIST_START");
            break;
        case KON_TOKEN_LIST_END:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_LIST_END");
            break;
        case KON_TOKEN_VECTOR_START:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_VECTOR_START");
            break;
        case KON_TOKEN_VECTOR_END:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_VECTOR_END");
            break;
        case KON_TOKEN_TABLE_START:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_TABLE_START");
            break;
        case KON_TOKEN_TABLE_END:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_TABLE_END");
            break;
        case KON_TOKEN_CELL_START:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_CELL_START");
            break;
        case KON_TOKEN_CELL_END:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_CELL_END");
            break;
        case KON_TOKEN_TABLE_TAG:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_TABLE_TAG");
            break;
        case KON_TOKEN_APPLY:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_APPLY");
            break;
        case KON_TOKEN_EXEC_MSG:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_EXEC_MSG");
            break;
        case KON_TOKEN_PIPE:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_PIPE");
            break;
        
        case KON_TOKEN_CLAUSE_END:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_CLAUSE_END");
            break;
        case KON_TOKEN_WHITESPACE:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_WHITESPACE");
            break;
        case KON_TOKEN_KEYWORD_NIL:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_KEYWORD_NIL");
            break;
        case KON_TOKEN_KEYWORD_NULL:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_KEYWORD_NULL");
            break;
        case KON_TOKEN_KEYWORD_UKN:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_KEYWORD_UKN");
            break;
        case KON_TOKEN_KEYWORD_TRUE:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_KEYWORD_TRUE");
            break;
        case KON_TOKEN_KEYWORD_FALSE:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_KEYWORD_FALSE");
            break;
        case KON_TOKEN_KEYWORD_EITHER:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_KEYWORD_EITHER");
            break;
        case KON_TOKEN_KEYWORD_BOTH:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_KEYWORD_BOTH");
            break;
        case KON_TOKEN_LITERAL_NUMBER:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_LITERAL_NUMBER");
            break;
        case KON_TOKEN_LITERAL_STRING:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_LITERAL_STRING");
            break;
        case KON_TOKEN_SYM_WORD:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_SYM_WORD");
            break;
        case KON_TOKEN_LITERAL_RAW_STRING:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_LITERAL_RAW_STRING");
            break;
        case KON_TOKEN_SYM_VARIABLE:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_SYM_VARIABLE");
            break;
        case KON_TOKEN_SYM_PREFIX_WORD:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_SYM_PREFIX_WORD");
            break;
        case KON_TOKEN_SYM_SUFFIX_WORD:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_SYM_SUFFIX_WORD");
            break;
        case KON_TOKEN_SYM_IDENTIFIER:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_SYM_IDENTIFIER");
            break;
        case KON_TOKEN_SYM_STRING:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_SYM_STRING");
            break;
        case KON_TOKEN_QUOTE_VECTOR:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_QUOTE_VECTOR");
            break;
        case KON_TOKEN_QUOTE_LIST:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_QUOTE_LIST");
            break;
        case KON_TOKEN_QUOTE_TABLE:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_QUOTE_TABLE");
            break;
        case KON_TOKEN_QUOTE_CELL:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_QUOTE_CELL");
            break;
        case KON_TOKEN_QUASI_VECTOR:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_QUASI_VECTOR");
            break;
        case KON_TOKEN_QUASI_LIST:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_QUASI_LIST");
            break;
        case KON_TOKEN_QUASI_TABLE:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_QUASI_TABLE");
            break;
        case KON_TOKEN_QUASI_CELL:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_QUASI_CELL");
            break;

        case KON_TOKEN_EXPAND_REPLACE:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_EXPAND_REPLACE");
            break;
        case KON_TOKEN_EXPAND_KV:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_EXPAND_KV");
            break;
        case KON_TOKEN_EXPAND_SEQ:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_EXPAND_SEQ");
            break;
        case KON_TOKEN_UNQUOTE_REPLACE:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_UNQUOTE_REPLACE");
            break;
        case KON_TOKEN_UNQUOTE_KV:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_UNQUOTE_KV");
            break;
        case KON_TOKEN_UNQUOTE_SEQ:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_UNQUOTE_SEQ");
            break;

        case KON_TOKEN_COMMENT_SINGLE_LINE:
            KonStringBuffer_AppendCstr(tokenKind, "KON_TOKEN_COMMENT_SINGLE_LINE");
            break;
        default:
            break;
    }
    // format
//    KON_DEBUG("<%s (:row %ld, :col %ld, :row-end %ld, :col-end %ld) [%s]>", KonStringBuffer_Cstr(tokenKind), tokenizer->RowStart, tokenizer->ColStart, tokenizer->RowEnd, tokenizer->ColEnd, KonStringBuffer_Cstr(tokenizer->Content));
    KON_DEBUG("<%s (:row %d, :col %d) [%s]>", KonStringBuffer_Cstr(tokenKind), tokenizer->RowStart, tokenizer->ColStart, KonStringBuffer_Cstr(tokenizer->Content));
}

KonTokenizer* KSON_TokenizerInit(KonState* kstate)
{
    // init
    KonTokenizer* tokenizer = (KonTokenizer*)malloc(sizeof(KonTokenizer));
    tb_assert_and_check_return_val(tokenizer, tb_null);

    // init string
    tokenizer->Content = KonStringBuffer_New();
    // stream offset
    tokenizer->CurrRow = 1;
    tokenizer->CurrCol = 1;

    // token's start end row col
    tokenizer->RowStart = 1;
    tokenizer->RowEnd = 1;
    tokenizer->ColStart = 1;
    tokenizer->ColEnd = 1;

    tokenizer->NumIsPositive = true;
    tokenizer->NumIsPowerPositive = true;
    tokenizer->NumBeforeDot = KonStringBuffer_New();
    tokenizer->NumAfterDot = KonStringBuffer_New();
    tokenizer->NumAfterPower = KonStringBuffer_New();

    return tokenizer;
}

bool KSON_TokenizerOpenStream(KonTokenizer* tokenizer, tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(tokenizer && stream, false);

    // done
    bool ok = false;
//    do {
//        // open the reade stream if be not opened
//        if (!tb_stream_is_opened(tokenizer->ReadStream) && !tb_stream_open(tokenizer->ReadStream)) {
//            break;
//        }
//        ok = true;
//    } while (0);
    
    if (!tb_stream_open(stream)) {
        return false;
    }
    
    // init the reade stream
    tokenizer->ReadStream = stream;
    
    
    // clear text
    KonStringBuffer_Clear(tokenizer->Content);
//    ok = true;

    // failed? close it
//    if (!ok) {
//        KSON_TokenizerCloseStream(tokenizer);
//    }

    // ok?
    return true;
}

void KSON_TokenizerCloseStream(KonTokenizer* tokenizer)
{
    // check
    tb_assert_and_check_return(tokenizer);

    // clos the reade stream
    if (tokenizer->ReadStream) {
        tb_stream_clos(tokenizer->ReadStream);
    }
    tokenizer->ReadStream = tb_null;

    // clear text
    KonStringBuffer_Clear(tokenizer->Content);
}

void KSON_TokenizerExit(KonTokenizer* tokenizer)
{

}

// don't move stream offset
tb_char_t* PeekChars(KonTokenizer* tokenizer, int n)
{
    if (n <= 0) {
        n = 1;
    }
    tb_char_t* pc = tb_null;
    if (!tb_stream_need(tokenizer->ReadStream, (tb_byte_t**)&pc, n) || !pc) {
        // peek char to stream end
        return tb_null;
    }
    return pc;
}

// move stream offset
tb_char_t ForwardChar(KonTokenizer* tokenizer)
{
    tb_char_t ch = '\0';
    if (tb_stream_bread_s8(tokenizer->ReadStream, (tb_sint8_t*)&ch)) {
        if (ch == '\n') {
            tokenizer->CurrRow = tokenizer->CurrRow + 1;
            tokenizer->CurrCol = 1;
        }
        else {
            tokenizer->CurrCol = tokenizer->CurrCol + 1;
        }
        return ch;
    }
    else {
        return -1;
    }
}

void SkipWhiteSpaces(KonTokenizer* tokenizer)
{
    tb_char_t* pc = PeekChars(tokenizer, 1);
    if (pc == tb_null) {
        // file end
        return;
    }
    if (isspace(pc[0])) {
        ForwardChar(tokenizer);
    }
    return;
}

bool IsDigit(tb_char_t ch)
{
    if (ch >= '0' && ch <= '9') {
        return true;
    }
    else {
        return false;
    }
}

bool IsIdentiferPrefixChar(tb_char_t ch)
{
    if ((ch >= 'A' && ch <= 'Z')
        || (ch >= 'a' && ch <= 'z')
        || ch == '_'
    ) {
        return true;
    }
    else {
        return false;
    }
}

bool IsIdentiferChar(tb_char_t ch)
{
    if ((ch >= 'A' && ch <= 'Z')
        || (ch >= 'a' && ch <= 'z')
        || (ch >= '0' && ch <= '9')
        || ch == '_' || ch == '-'
    ) {
        return true;
    }
    else {
        return false;
    }
}

bool IsSpace(tb_char_t ch)
{
    return isspace(ch);
}

bool IsStopWord(tb_char_t ch)
{
    char dest[16] = ":%.|![](){}<>;";
    if (strchr(dest, ch) > 0) {
        return true;
    }
    else {
        return false;
    }
}

void ForwardToken(KonTokenizer* tokenizer, int len)
{
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    for (int i = 0; i < len; i++) {
        ForwardChar(tokenizer);
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}

void ParseIdentifier(KonTokenizer* tokenizer)
{
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    KonStringBuffer_Clear(tokenizer->Content);

    tb_char_t ch = ForwardChar(tokenizer);
    // add identifier first char
    KonStringBuffer_NAppendChar(tokenizer->Content, ch, 1);

    tb_char_t* pc = tb_null;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (IsSpace(pc[0]) || IsStopWord(pc[0])) {
            break;
        }
        ch = ForwardChar(tokenizer);
        KonStringBuffer_NAppendChar(tokenizer->Content, ch, 1);
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}


void ParseString(KonTokenizer* tokenizer)
{
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    KonStringBuffer_Clear(tokenizer->Content);

    tb_char_t ch = ForwardChar(tokenizer);

    tb_char_t* pc = tb_null;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == '\"') {
            ForwardChar(tokenizer);
            break;
        }
        ch = ForwardChar(tokenizer);
        KonStringBuffer_NAppendChar(tokenizer->Content, ch, 1);
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}


// like 'abc'
void ParseRawString(KonTokenizer* tokenizer)
{
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    KonStringBuffer_Clear(tokenizer->Content);

    tb_char_t ch = ForwardChar(tokenizer);

    tb_char_t* pc = tb_null;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == '\'') {
            ForwardChar(tokenizer);
            break;
        }
        ch = ForwardChar(tokenizer);
        KonStringBuffer_NAppendChar(tokenizer->Content, ch, 1);
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}

// single line comment like // xxx or ` xxx
void ParseSingleLineComment(KonTokenizer* tokenizer)
{
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    KonStringBuffer_Clear(tokenizer->Content);
    // forward two char  "//"
    ForwardChar(tokenizer);
    ForwardChar(tokenizer);

    tb_char_t ch = '\0';
    tb_char_t* pc = tb_null;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == '\n') {
            break;
        }
        ch = ForwardChar(tokenizer);
        KonStringBuffer_NAppendChar(tokenizer->Content, ch, 1);
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}

void ParseNumber(KonTokenizer* tokenizer)
{
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    // initialize
    KonStringBuffer_Clear(tokenizer->Content);
    tokenizer->NumIsPositive = true;
    tokenizer->NumIsPowerPositive = true;
    KonStringBuffer_Clear(tokenizer->NumBeforeDot);
    KonStringBuffer_Clear(tokenizer->NumAfterDot);
    KonStringBuffer_Clear(tokenizer->NumAfterPower);

    // add first char of this num
    tb_char_t ch = ForwardChar(tokenizer);
    KonStringBuffer_NAppendChar(tokenizer->Content, ch, 1);

    if (ch == '-') {
        tokenizer->NumIsPositive = false;
    }
    else if (ch == '+') {
        tokenizer->NumIsPositive = true;
    }
    else {
        // is positive number
        KonStringBuffer_NAppendChar(tokenizer->NumBeforeDot, ch, 1);
    }
    // state of parsing number
    // 1 parse num before dot, 2 parse num after dot, 3 parse powver
    int state = 1;

    tb_char_t* pc = tb_null;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        char stopChars[16] = ":%|![](){}<>;";
        if (IsSpace(pc[0]) || strchr(stopChars, pc[0]) > 0) {
            break;
        }
        ch = ForwardChar(tokenizer);
        KonStringBuffer_NAppendChar(tokenizer->Content, ch, 1);

        if (state == 1 && ch != '.' && ch != 'e' && ch != 'E') {
            KonStringBuffer_NAppendChar(tokenizer->NumBeforeDot, ch, 1);
        }
        else if (state == 2 && ch != 'e' && ch != 'E') {
            KonStringBuffer_NAppendChar(tokenizer->NumAfterDot, ch, 1);
        }
        else if (state == 3 && ch == '-') {
            tokenizer->NumIsPowerPositive = false;
        }
        else if (state == 3 && ch != '-') {
            KonStringBuffer_NAppendChar(tokenizer->NumAfterPower, ch, 1);
        }

        if (pc[0] == '.') {
            state = 2;
        }
        else if (pc[0] == 'e' || pc[0] == 'E') {
            state = 3;
        }
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}

void UpdateTokenContent(KonTokenizer* tokenizer, char* newContent)
{
    KonStringBuffer_Clear(tokenizer->Content);
    KonStringBuffer_AppendCstr(tokenizer->Content, newContent);
}

KonTokenKind KSON_TokenizerNext(KonTokenizer* tokenizer)
{
    tb_assert_and_check_return_val(tokenizer && tokenizer->ReadStream, KON_TOKEN_EOF);

    // reset
    tokenizer->TokenKind = KON_TOKEN_EOF;
    while (tokenizer->TokenKind == KON_TOKEN_EOF) {
        // peek character
        tb_char_t* pc = PeekChars(tokenizer, 1);
        if (pc == tb_null) {
            // reach file end
            break;
        }
        if (IsSpace(pc[0])) {
            SkipWhiteSpaces(tokenizer);
        }
        
        else if (pc[0] == '(') {
            UpdateTokenContent(tokenizer, "(");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_TABLE_START;
            break;
        }
        else if (pc[0] == ')') {
            UpdateTokenContent(tokenizer, ")");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_TABLE_END;
            break;
        }
        else if (pc[0] == '[') {
            UpdateTokenContent(tokenizer, "[");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_VECTOR_START;
            break;
        }
        else if (pc[0] == ']') {
            UpdateTokenContent(tokenizer, "]");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_VECTOR_END;
            break;
        }
        else if (pc[0] == '{') {
            UpdateTokenContent(tokenizer, "{");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_LIST_START;
            break;
        }
        else if (pc[0] == '}') {
            UpdateTokenContent(tokenizer, "}");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_LIST_END;
            break;
        }
        else if (pc[0] == '<') {
            UpdateTokenContent(tokenizer, "<");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_CELL_START;
            break;
        }
        else if (pc[0] == '>') {
            UpdateTokenContent(tokenizer, ">");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_CELL_END;
            break;
        }
        else if (pc[0] == '-') {
            tb_char_t* nextChars = PeekChars(tokenizer, 2);
            if (nextChars == tb_null) {
                break;
            }
            if (IsSpace(nextChars[1])) {
                UpdateTokenContent(tokenizer, "-");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_SYM_WORD;
                break;
            }
            else if (IsDigit(nextChars[1])) {
                ParseNumber(tokenizer);
                tokenizer->TokenKind = KON_TOKEN_LITERAL_NUMBER;
                break;
            }
        }
        else if (pc[0] == '+') {
            tb_char_t* nextChars = PeekChars(tokenizer, 2);
            if (nextChars == tb_null) {
                break;
            }
            if (IsSpace(nextChars[1])) {
                UpdateTokenContent(tokenizer, "+");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_SYM_WORD;
                break;
            }
            else if (IsDigit(nextChars[1])) {
                ParseNumber(tokenizer);
                tokenizer->TokenKind = KON_TOKEN_LITERAL_NUMBER;
                break;
            }
        }
        else if (IsDigit(pc[0])) {
            ParseNumber(tokenizer);
            tokenizer->TokenKind = KON_TOKEN_LITERAL_NUMBER;
            break;
        }
        else if (pc[0] == '#') {
            tb_char_t* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == tb_null) {
                break;
            }
            // TODO other immediate atom builders
            if (nextChars[1] == 'n' && nextChars[2] == 'i') {
                UpdateTokenContent(tokenizer, "#nil;");
                ForwardToken(tokenizer, 5);
                tokenizer->TokenKind = KON_TOKEN_KEYWORD_NIL;
                break;
            }
            else if (nextChars[1] == 'n' && nextChars[2] == 'u') {
                UpdateTokenContent(tokenizer, "#null;");
                ForwardToken(tokenizer, 6);
                tokenizer->TokenKind = KON_TOKEN_KEYWORD_NULL;
                break;
            }
            else if (nextChars[1] == 'u' && nextChars[2] == 'k') {
                UpdateTokenContent(tokenizer, "#ukn;");
                ForwardToken(tokenizer, 5);
                tokenizer->TokenKind = KON_TOKEN_KEYWORD_UKN;
                break;
            }
            else if (nextChars[1] == 't' && nextChars[2] == ';') {
                UpdateTokenContent(tokenizer, "#t;");
                ForwardToken(tokenizer, 3);
                tokenizer->TokenKind = KON_TOKEN_KEYWORD_TRUE;
                break;
            }
            else if (nextChars[1] == 'f' && nextChars[2] == ';') {
                UpdateTokenContent(tokenizer, "#f;");
                ForwardToken(tokenizer, 3);
                tokenizer->TokenKind = KON_TOKEN_KEYWORD_FALSE;
                break;
            }
        }
        else if (pc[0] == ':') {
            UpdateTokenContent(tokenizer, ":");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_TABLE_TAG;
        }
        else if (pc[0] == '%') {
            UpdateTokenContent(tokenizer, "%");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_APPLY;
        }
        else if (pc[0] == '.') {
            UpdateTokenContent(tokenizer, ".");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_EXEC_MSG;
        }
        else if (pc[0] == '|') {
            UpdateTokenContent(tokenizer, "|");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_PIPE;
        }
        else if (pc[0] == ';') {
            UpdateTokenContent(tokenizer, ";");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_CLAUSE_END;
        }
        else if (pc[0] == '\"') {
            ParseString(tokenizer);
            tokenizer->TokenKind = KON_TOKEN_LITERAL_STRING;
        }
        else if (pc[0] == '\'') {
            ParseRawString(tokenizer);
            tokenizer->TokenKind = KON_TOKEN_LITERAL_RAW_STRING;
        }
        else if (pc[0] == '$') {
            tb_char_t* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == tb_null) {
                break;
            }
            else if (nextChars[1] == '.') {
                UpdateTokenContent(tokenizer, "$.");
                ForwardToken(tokenizer, 2);
                tokenizer->TokenKind = KON_TOKEN_EXPAND_REPLACE;
            }
            else if (nextChars[1] == '%' && nextChars[2] == '.') {
                UpdateTokenContent(tokenizer, "$%.");
                ForwardToken(tokenizer, 3);
                tokenizer->TokenKind = KON_TOKEN_EXPAND_KV;
            }
            else if (nextChars[1] == '~' && nextChars[2] == '.') {
                UpdateTokenContent(tokenizer, "$~.");
                ForwardToken(tokenizer, 3);
                tokenizer->TokenKind = KON_TOKEN_EXPAND_SEQ;
            }
            else if (nextChars[1] == '[') {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_QUOTE_VECTOR;
            }
            else if (nextChars[1] == '(') {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_QUOTE_TABLE;
            }
            else if (nextChars[1] == '<') {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_QUOTE_CELL;
            }
            else if (nextChars[1] == '{') {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_QUOTE_LIST;
            }
            else if (nextChars[1] == '\'') {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_SYM_STRING;
            }
            else {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_SYM_IDENTIFIER;
            }
        }
        else if (pc[0] == '@') {
            tb_char_t* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == tb_null) {
                break;
            }
            else if (nextChars[1] == '.') {
                UpdateTokenContent(tokenizer, "@.");
                ForwardToken(tokenizer, 2);
                tokenizer->TokenKind = KON_TOKEN_UNQUOTE_REPLACE;
            }
            else if (nextChars[1] == '%' && nextChars[2] == '.') {
                UpdateTokenContent(tokenizer, "@%.");
                ForwardToken(tokenizer, 3);
                tokenizer->TokenKind = KON_TOKEN_UNQUOTE_KV;
            }
            else if (nextChars[1] == '~' && nextChars[2] == '.') {
                UpdateTokenContent(tokenizer, "@~.");
                ForwardToken(tokenizer, 3);
                tokenizer->TokenKind = KON_TOKEN_UNQUOTE_SEQ;
            }
            else if (nextChars[1] == '[') {
                UpdateTokenContent(tokenizer, "@");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_QUASI_VECTOR;
            }
            else if (nextChars[1] == '(') {
                UpdateTokenContent(tokenizer, "@");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_QUASI_TABLE;
            }
            else if (nextChars[1] == '<') {
                UpdateTokenContent(tokenizer, "@");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_QUASI_CELL;
            }
            else if (nextChars[1] == '{') {
                UpdateTokenContent(tokenizer, "@");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_QUASI_LIST;
            }
            else {
                UpdateTokenContent(tokenizer, "@");
                ForwardToken(tokenizer, 1);
                ParseIdentifier(tokenizer);
                tokenizer->TokenKind = KON_TOKEN_SYM_VARIABLE;
            }
        }
        else if (pc[0] == '!') {
            ForwardToken(tokenizer, 1);
            ParseIdentifier(tokenizer);
            tokenizer->TokenKind = KON_TOKEN_SYM_PREFIX_WORD;
        }
        else if (pc[0] == '^') {
            ForwardToken(tokenizer, 1);
            ParseIdentifier(tokenizer);
            tokenizer->TokenKind = KON_TOKEN_SYM_SUFFIX_WORD;
        }
        else if (IsIdentiferPrefixChar(pc[0])) {
            ParseIdentifier(tokenizer);
            tokenizer->TokenKind = KON_TOKEN_SYM_WORD;
            break;
        }
        else if (pc[0] == '`') {
            ParseSingleLineComment(tokenizer);
            tokenizer->TokenKind = KON_TOKEN_COMMENT_SINGLE_LINE;
            break;
        }
        else if (pc[0] == '/') {
            tb_char_t* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == tb_null) {
                break;
            }
            if (nextChars[1] == '/') {
                ParseSingleLineComment(tokenizer);
                tokenizer->TokenKind = KON_TOKEN_COMMENT_SINGLE_LINE;
                break;
            }
            else {
                // divide identifier
                UpdateTokenContent(tokenizer, "/");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_SYM_WORD;
                break;
            }
        }
        else if (pc[0] == '*') {
            // multiply identifier
            UpdateTokenContent(tokenizer, "*");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_SYM_WORD;
            break;
        }
        else {
            KON_DEBUG("neet non space char %d", pc[0]);
            break;
        }
    }
    if (tokenizer->TokenKind == KON_TOKEN_NONE) {
        tokenizer->TokenKind = KON_TOKEN_EOF;
    }
    return tokenizer->TokenKind;
}



