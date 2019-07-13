#include "tokenizer.h"
#include <tbox/tbox.h>
#include <ctype.h>


void KSON_TokenToString(KonTokenizer* tokenizer)
{
    tb_string_t tokenKind;
    tb_string_init(&tokenKind);

    switch (tokenizer->TokenKind) {
        case KON_TOKEN_EOF:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_EOF");
            break;
        case KON_TOKEN_LIST_START:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_LIST_START");
            break;
        case KON_TOKEN_LIST_END:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_LIST_END");
            break;
        case KON_TOKEN_VECTOR_START:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_VECTOR_START");
            break;
        case KON_TOKEN_VECTOR_END:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_VECTOR_END");
            break;
        case KON_TOKEN_TABLE_START:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_TABLE_START");
            break;
        case KON_TOKEN_TABLE_END:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_TABLE_END");
            break;
        case KON_TOKEN_CELL_START:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_CELL_START");
            break;
        case KON_TOKEN_CELL_END:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_CELL_END");
            break;
        case KON_TOKEN_TABLE_TAG:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_TABLE_TAG");
            break;
        case KON_TOKEN_APPLY:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_APPLY");
            break;
        case KON_TOKEN_EXEC_MSG:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_EXEC_MSG");
            break;
        case KON_TOKEN_PIPE:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_PIPE");
            break;
        
        case KON_TOKEN_CLAUSE_END:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_CLAUSE_END");
            break;
        case KON_TOKEN_WHITESPACE:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_WHITESPACE");
            break;
        case KON_TOKEN_KEYWORD_NIL:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_KEYWORD_NIL");
            break;
        case KON_TOKEN_KEYWORD_NULL:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_KEYWORD_NULL");
            break;
        case KON_TOKEN_KEYWORD_UKN:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_KEYWORD_UKN");
            break;
        case KON_TOKEN_KEYWORD_TRUE:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_KEYWORD_TRUE");
            break;
        case KON_TOKEN_KEYWORD_FALSE:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_KEYWORD_FALSE");
            break;
        case KON_TOKEN_KEYWORD_EITHER:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_KEYWORD_EITHER");
            break;
        case KON_TOKEN_KEYWORD_BOTH:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_KEYWORD_BOTH");
            break;
        case KON_TOKEN_LITERAL_NUMBER:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_LITERAL_NUMBER");
            break;
        case KON_TOKEN_LITERAL_STRING:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_LITERAL_STRING");
            break;
        case KON_TOKEN_SYM_IDENTIFIER:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_SYM_IDENTIFIER");
            break;
        case KON_TOKEN_SYM_STRING:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_SYM_STRING");
            break;
        case KON_TOKEN_SYM_VARIABLE:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_SYM_VARIABLE");
            break;
        case KON_TOKEN_SYM_FORM_WORD:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_SYM_FORM_WORD");
            break;
        case KON_TOKEN_QUOTE_IDENTIFER:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_QUOTE_IDENTIFER");
            break;
        case KON_TOKEN_QUOTE_SYMBOL:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_QUOTE_SYMBOL");
            break;
        case KON_TOKEN_QUOTE_VECTOR:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_QUOTE_VECTOR");
            break;
        case KON_TOKEN_QUOTE_LIST:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_QUOTE_LIST");
            break;
        case KON_TOKEN_QUOTE_TABLE:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_QUOTE_TABLE");
            break;
        case KON_TOKEN_QUOTE_CELL:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_QUOTE_CELL");
            break;
        case KON_TOKEN_QUASI_VECTOR:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_QUASI_VECTOR");
            break;
        case KON_TOKEN_QUASI_LIST:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_QUASI_LIST");
            break;
        case KON_TOKEN_QUASI_TABLE:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_QUASI_TABLE");
            break;
        case KON_TOKEN_QUASI_CELL:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_QUASI_CELL");
            break;

        case KON_TOKEN_EXPAND_REPLACE:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_EXPAND_REPLACE");
            break;
        case KON_TOKEN_EXPAND_VECTOR:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_EXPAND_VECTOR");
            break;
        case KON_TOKEN_EXPAND_TABLE:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_EXPAND_TABLE");
            break;
        case KON_TOKEN_EXPAND_LIST:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_EXPAND_LIST");
            break;
        case KON_TOKEN_UNQUOTE_REPLACE:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_UNQUOTE_REPLACE");
            break;
        case KON_TOKEN_UNQUOTE_VECTOR:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_UNQUOTE_VECTOR");
            break;
        case KON_TOKEN_UNQUOTE_TABLE:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_UNQUOTE_TABLE");
            break;
        case KON_TOKEN_UNQUOTE_LIST:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_UNQUOTE_LIST");
            break;

        case KON_TOKEN_COMMENT_SINGLE_LINE:
            tb_string_cstrcpy(&tokenKind, "KON_TOKEN_COMMENT_SINGLE_LINE");
            break;
        default:
            break;
    }
    // format
    printf("<%s (:row %ld, :col %ld, :row-end %ld, :col-end %ld) [%s]>\n", tb_string_cstr(&tokenKind), tokenizer->RowStart, tokenizer->ColStart, tokenizer->RowEnd, tokenizer->ColEnd, tb_string_cstr(&tokenizer->Content));
}

KonTokenizer* KSON_TokenizerInit(Kon* kstate)
{
    // init
    KonTokenizer* tokenizer = (KonTokenizer*)malloc(sizeof(KonTokenizer));
    tb_assert_and_check_return_val(tokenizer, tb_null);

    // init string
    tb_string_init(&tokenizer->Content);
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
    tb_string_init(&tokenizer->NumBeforeDot);
    tb_string_init(&tokenizer->NumAfterDot);
    tb_string_init(&tokenizer->NumAfterPower);

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
    tb_string_clear(&tokenizer->Content);
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
    tb_string_clear(&tokenizer->Content);

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
            // printf("meet newline CurrRow %d\n", tokenizer->CurrRow);
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
    if (ch == ':' || ch == '%' || ch == '.' || ch == '|' || ch == '!'
        || ch == '[' || ch == '(' || ch == '{' || ch == '<'
        || ch == ']' || ch == ')' || ch == '}' || ch == '>'
        || ch == ';'
    ) {
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
    tb_string_clear(&tokenizer->Content);

    tb_char_t ch = ForwardChar(tokenizer);
    // add identifier first char
    tb_string_chrcat(&tokenizer->Content, ch);

    tb_char_t* pc = tb_null;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (IsSpace(pc[0]) || IsStopWord(pc[0])) {
            break;
        }
        ch = ForwardChar(tokenizer);
        tb_string_chrcat(&tokenizer->Content, ch);
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}


void ParseString(KonTokenizer* tokenizer)
{
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    tb_string_clear(&tokenizer->Content);

    tb_char_t ch = ForwardChar(tokenizer);

    tb_char_t* pc = tb_null;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == '\"') {
            ForwardChar(tokenizer);
            break;
        }
        ch = ForwardChar(tokenizer);
        tb_string_chrcat(&tokenizer->Content, ch);
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}


// like 'abc'
void ParseRawString(KonTokenizer* tokenizer)
{
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    tb_string_clear(&tokenizer->Content);

    tb_char_t ch = ForwardChar(tokenizer);

    tb_char_t* pc = tb_null;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == '\'') {
            ForwardChar(tokenizer);
            break;
        }
        ch = ForwardChar(tokenizer);
        tb_string_chrcat(&tokenizer->Content, ch);
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}

// single line comment like // xxx
void ParseSingleLineComment(KonTokenizer* tokenizer)
{
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    tb_string_clear(&tokenizer->Content);
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
        tb_string_chrcat(&tokenizer->Content, ch);
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}

void ParseNumber(KonTokenizer* tokenizer)
{
    // printf("ParseNumber\n");
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    // initialize
    tb_string_clear(&tokenizer->Content);
    tokenizer->NumIsPositive = true;
    tokenizer->NumIsPowerPositive = true;
    tb_string_clear(&tokenizer->NumBeforeDot);
    tb_string_clear(&tokenizer->NumAfterDot);
    tb_string_clear(&tokenizer->NumAfterPower);

    // add first char of this num
    tb_char_t ch = ForwardChar(tokenizer);
    tb_string_chrcat(&tokenizer->Content, ch);

    if (ch == '-') {
        tokenizer->NumIsPositive = false;
    }
    else if (ch == '+') {
        tokenizer->NumIsPositive = true;
    }
    else {
        // is positive number
        tb_string_chrcat(&tokenizer->NumBeforeDot, ch);
    }
    // state of parsing number
    // 1 parse num before dot, 2 parse num after dot, 3 parse powver
    int state = 1;

    tb_char_t* pc = tb_null;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (IsSpace(pc[0]) || pc[0] == ':' || pc[0] == '%' || pc[0] == '|' || pc[0] == '!'
            || pc[0] == '[' || pc[0] == '(' || pc[0] == '{' || pc[0] == '<'
            || pc[0] == ']' || pc[0] == ')' || pc[0] == '}' || pc[0] == '>'
            || pc[0] == ';'
        ) {
            break;
        }
        ch = ForwardChar(tokenizer);
        tb_string_chrcat(&tokenizer->Content, ch);

        if (state == 1 && ch != '.' && ch != 'e' && ch != 'E') {
            tb_string_chrcat(&tokenizer->NumBeforeDot, ch);
        }
        else if (state == 2 && ch != 'e' && ch != 'E') {
            tb_string_chrcat(&tokenizer->NumAfterDot, ch);
        }
        else if (state == 3 && ch == '-') {
            tokenizer->NumIsPowerPositive = false;
        }
        else if (state == 3 && ch != '-') {
            tb_string_chrcat(&tokenizer->NumAfterPower, ch);
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
    tb_string_clear(&tokenizer->Content);
    tb_string_cstrcat(&tokenizer->Content, newContent);
}

KonTokenKind KSON_TokenizerNext(KonTokenizer* tokenizer)
{
    // printf("KSON_TokenizerNext before assert tokenizer %ld, tokenizer->ReadStream %ld\n", tokenizer, tokenizer->ReadStream);
    tb_assert_and_check_return_val(tokenizer && tokenizer->ReadStream, KON_TOKEN_EOF);
    // printf("KSON_TokenizerNext current offset %d\n", tb_stream_offset(tokenizer->ReadStream));
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
                tokenizer->TokenKind = KON_TOKEN_SYM_IDENTIFIER;
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
                tokenizer->TokenKind = KON_TOKEN_SYM_IDENTIFIER;
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
            tokenizer->TokenKind = KON_TOKEN_SYM_STRING;
        }
        else if (pc[0] == '@') {
            tb_char_t* nextChars = PeekChars(tokenizer, 2);
            if (nextChars == tb_null) {
                break;
            }
            if (nextChars[1] == '[') {
                tokenizer->TokenKind = KON_TOKEN_QUOTE_VECTOR;
            }
            else if (nextChars[1] == '(') {
                tokenizer->TokenKind = KON_TOKEN_QUOTE_TABLE;
            }
            else if (nextChars[1] == '<') {
                tokenizer->TokenKind = KON_TOKEN_QUOTE_CELL;
            }
            else if (nextChars[1] == '{') {
                tokenizer->TokenKind = KON_TOKEN_QUOTE_LIST;
            }
            else if (nextChars[1] == '\'') {
                tokenizer->TokenKind = KON_TOKEN_QUOTE_SYMBOL;
            }
            else {
                tokenizer->TokenKind = KON_TOKEN_QUOTE_IDENTIFER;
            }
            UpdateTokenContent(tokenizer, "@");
            ForwardToken(tokenizer, 1);
        }
        else if (pc[0] == '$') {
            tb_char_t* nextChars = PeekChars(tokenizer, 4);
            if (nextChars == tb_null) {
                break;
            }
            if (nextChars[1] == '.') {
                UpdateTokenContent(tokenizer, "$.");
                ForwardToken(tokenizer, 2);
                tokenizer->TokenKind = KON_TOKEN_EXPAND_REPLACE;
            }
            else if (nextChars[1] == 'e') {
                UpdateTokenContent(tokenizer, "$e.");
                ForwardToken(tokenizer, 3);
                tokenizer->TokenKind = KON_TOKEN_UNQUOTE_REPLACE;
            }
            else if (nextChars[1] == '[' && nextChars[2] == ']' && nextChars[3] == '.') {
                UpdateTokenContent(tokenizer, "$[].");
                ForwardToken(tokenizer, 4);
                tokenizer->TokenKind = KON_TOKEN_EXPAND_VECTOR;
            }
            else if (nextChars[1] == '[' && nextChars[2] == ']' && nextChars[3] == 'e') {
                UpdateTokenContent(tokenizer, "$[]e.");
                ForwardToken(tokenizer, 5);
                tokenizer->TokenKind = KON_TOKEN_UNQUOTE_VECTOR;
            }
            else if (nextChars[1] == '(' && nextChars[2] == ')' && nextChars[3] == '.') {
                UpdateTokenContent(tokenizer, "$().");
                ForwardToken(tokenizer, 4);
                tokenizer->TokenKind = KON_TOKEN_EXPAND_TABLE;
            }
            else if (nextChars[1] == '(' && nextChars[2] == ')' && nextChars[3] == 'e') {
                UpdateTokenContent(tokenizer, "$()e.");
                ForwardToken(tokenizer, 5);
                tokenizer->TokenKind = KON_TOKEN_UNQUOTE_TABLE;
            }
            else if (nextChars[1] == '{' && nextChars[2] == '}' && nextChars[3] == '.') {
                UpdateTokenContent(tokenizer, "${}.");
                ForwardToken(tokenizer, 4);
                tokenizer->TokenKind = KON_TOKEN_EXPAND_LIST;
            }
            else if (nextChars[1] == '{' && nextChars[2] == '}' && nextChars[3] == 'e') {
                UpdateTokenContent(tokenizer, "${}e.");
                ForwardToken(tokenizer, 5);
                tokenizer->TokenKind = KON_TOKEN_UNQUOTE_LIST;
            }
            else if (nextChars[1] == '[') {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_QUASI_VECTOR;
            }
            else if (nextChars[1] == '(') {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_QUASI_TABLE;
            }
            else if (nextChars[1] == '<') {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_QUASI_CELL;
            }
            else if (nextChars[1] == '{') {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KON_TOKEN_QUASI_LIST;
            }
            else {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                ParseIdentifier(tokenizer);
                tokenizer->TokenKind = KON_TOKEN_SYM_VARIABLE;
            }
        }
        else if (pc[0] == '!') {
            ForwardToken(tokenizer, 1);
            ParseIdentifier(tokenizer);
            tokenizer->TokenKind = KON_TOKEN_SYM_FORM_WORD;
        }
        else if (IsIdentiferPrefixChar(pc[0])) {
            ParseIdentifier(tokenizer);
            tokenizer->TokenKind = KON_TOKEN_SYM_IDENTIFIER;
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
                tokenizer->TokenKind = KON_TOKEN_SYM_IDENTIFIER;
                break;
            }
        }
        else if (pc[0] == '*') {
            // multiply identifier
            UpdateTokenContent(tokenizer, "*");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KON_TOKEN_SYM_IDENTIFIER;
            break;
        }
        else {
            printf("neet non space char %d\n", pc[0]);
            break;
        }
    }
    if (tokenizer->TokenKind == KON_TOKEN_NONE) {
        tokenizer->TokenKind = KON_TOKEN_EOF;
    }
    return tokenizer->TokenKind;
}



