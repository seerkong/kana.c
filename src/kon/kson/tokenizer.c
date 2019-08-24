#include "tokenizer.h"
#include <ctype.h>


void KSON_TokenToString(KonTokenizer* tokenizer)
{
    KxStringBuffer* tokenKind = KxStringBuffer_New();

    switch (tokenizer->TokenKind) {
        case KN_TOKEN_EOF:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_EOF");
            break;
        case KN_TOKEN_LIST_START:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_LIST_START");
            break;
        case KN_TOKEN_BLOCK_START:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_BLOCK_START");
            break;
        case KN_TOKEN_LIST_END:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_LIST_END");
            break;
        case KN_TOKEN_VECTOR_START:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_VECTOR_START");
            break;
        case KN_TOKEN_VECTOR_END:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_VECTOR_END");
            break;
        case KN_TOKEN_TABLE_START:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_TABLE_START");
            break;
        case KN_TOKEN_PARAM_START:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_PARAM_START");
            break;
        case KN_TOKEN_TABLE_END:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_TABLE_END");
            break;
        case KN_TOKEN_CELL_START:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_CELL_START");
            break;
        case KN_TOKEN_CELL_END:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_CELL_END");
            break;
        case KN_TOKEN_TABLE_TAG:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_TABLE_TAG");
            break;
        case KN_TOKEN_APPLY:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_APPLY");
            break;
        case KN_TOKEN_EQUAL:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_EQUAL");
            break;
        case KN_TOKEN_ASSIGN:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_ASSIGN");
            break;
        case KN_TOKEN_MSG_SIGNAL:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_MSG_SIGNAL");
            break;
        case KN_TOKEN_PROC_PIPE:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_PROC_PIPE");
            break;
        
        case KN_TOKEN_CLAUSE_END:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_CLAUSE_END");
            break;
        case KN_TOKEN_WHITESPACE:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_WHITESPACE");
            break;
        case KN_TOKEN_KEYWORD_NIL:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_KEYWORD_NIL");
            break;
        case KN_TOKEN_KEYWORD_UNDEF:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_KEYWORD_UNDEF");
            break;
        case KN_TOKEN_KEYWORD_UKN:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_KEYWORD_UKN");
            break;
        case KN_TOKEN_KEYWORD_TRUE:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_KEYWORD_TRUE");
            break;
        case KN_TOKEN_KEYWORD_FALSE:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_KEYWORD_FALSE");
            break;
        case KN_TOKEN_KEYWORD_EITHER:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_KEYWORD_EITHER");
            break;
        case KN_TOKEN_KEYWORD_BOTH:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_KEYWORD_BOTH");
            break;
        case KN_TOKEN_LITERAL_NUMBER:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_LITERAL_NUMBER");
            break;
        case KN_TOKEN_LITERAL_STRING:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_LITERAL_STRING");
            break;
        case KN_TOKEN_SYM_WORD:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_SYM_WORD");
            break;
        case KN_TOKEN_GET_SLOT:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_GET_SLOT");
            break;
        case KN_TOKEN_LITERAL_RAW_STRING:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_LITERAL_RAW_STRING");
            break;
        case KN_TOKEN_SYM_VARIABLE:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_SYM_VARIABLE");
            break;
        case KN_TOKEN_SYM_PREFIX_WORD:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_SYM_PREFIX_WORD");
            break;
        case KN_TOKEN_SYM_SUFFIX_WORD:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_SYM_SUFFIX_WORD");
            break;
        case KN_TOKEN_SYM_IDENTIFIER:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_SYM_IDENTIFIER");
            break;
        case KN_TOKEN_SYM_STRING:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_SYM_STRING");
            break;
        case KN_TOKEN_QUOTE_LIST:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_QUOTE_LIST");
            break;
        case KN_TOKEN_QUOTE_CELL:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_QUOTE_CELL");
            break;
        case KN_TOKEN_QUASI_LIST:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_QUASI_LIST");
            break;
        case KN_TOKEN_QUASI_CELL:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_QUASI_CELL");
            break;

        case KN_TOKEN_EXPAND_REPLACE:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_EXPAND_REPLACE");
            break;
        case KN_TOKEN_EXPAND_KV:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_EXPAND_KV");
            break;
        case KN_TOKEN_EXPAND_SEQ:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_EXPAND_SEQ");
            break;
        case KN_TOKEN_UNQUOTE_REPLACE:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_UNQUOTE_REPLACE");
            break;
        case KN_TOKEN_UNQUOTE_KV:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_UNQUOTE_KV");
            break;
        case KN_TOKEN_UNQUOTE_SEQ:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_UNQUOTE_SEQ");
            break;

        case KN_TOKEN_COMMENT_SINGLE_LINE:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_COMMENT_SINGLE_LINE");
            return;
            // break;
        default:
            break;
    }
    // format
//    KN_DEBUG("<%s (:row %ld, :col %ld, :row-end %ld, :col-end %ld) [%s]>", KxStringBuffer_Cstr(tokenKind), tokenizer->RowStart, tokenizer->ColStart, tokenizer->RowEnd, tokenizer->ColEnd, KxStringBuffer_Cstr(tokenizer->Content));
    KN_DEBUG("<%s (:row %d, :col %d) [%s]>", KxStringBuffer_Cstr(tokenKind), tokenizer->RowStart, tokenizer->ColStart, KxStringBuffer_Cstr(tokenizer->Content));
}

KonTokenizer* KSON_TokenizerInit(KonState* kstate)
{
    // init
    KonTokenizer* tokenizer = (KonTokenizer*)tb_malloc(sizeof(KonTokenizer));

    // init string
    tokenizer->Content = KxStringBuffer_New();

    tokenizer->ReadCursor = 0;
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
    tokenizer->NumBeforeDot = KxStringBuffer_New();
    tokenizer->NumAfterDot = KxStringBuffer_New();
    tokenizer->NumAfterPower = KxStringBuffer_New();

    return tokenizer;
}

bool KSON_TokenizerBegin(KonTokenizer* tokenizer, KxStringBuffer* codeString)
{
    // init the reade stream
    tokenizer->CodeString = codeString;
    tokenizer->CodeLen = KxStringBuffer_Length(codeString);
    // clear text
    KxStringBuffer_Clear(tokenizer->Content);

    return true;
}

void KSON_TokenizerEnd(KonTokenizer* tokenizer)
{

    // clear text
    KxStringBuffer_Clear(tokenizer->Content);
}

void KSON_TokenizerExit(KonTokenizer* tokenizer)
{

}

// don't move stream offset
const char* PeekChars(KonTokenizer* tokenizer, int n)
{
    if (n <= 0) {
        n = 1;
    }

    if (tokenizer->ReadCursor + n > tokenizer->CodeLen) {
        return NULL;
    }
    const char* pc = KxStringBuffer_Cstr(tokenizer->CodeString) + tokenizer->ReadCursor;

    return pc;
}

// move stream offset
const char ForwardChar(KonTokenizer* tokenizer)
{
    if (tokenizer->ReadCursor >= tokenizer->CodeLen) {
        return '\0';
    }
    
    const char ch = KxStringBuffer_CharAt(tokenizer->CodeString, tokenizer->ReadCursor);
    if (ch != '\0') {
        if (ch == '\n') {
            tokenizer->CurrRow = tokenizer->CurrRow + 1;
            tokenizer->CurrCol = 1;
        }
        else {
            tokenizer->CurrCol = tokenizer->CurrCol + 1;
        }

        tokenizer->ReadCursor += 1;
        return ch;
    }
    else {
        return ch;
    }
}

void SkipWhiteSpaces(KonTokenizer* tokenizer)
{
    const char* pc = PeekChars(tokenizer, 1);
    if (pc == NULL) {
        // file end
        return;
    }
    if (isspace(pc[0])) {
        ForwardChar(tokenizer);
    }
    return;
}

bool IsDigit(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return true;
    }
    else {
        return false;
    }
}

bool IsIdentiferPrefixChar(char ch)
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

bool IsIdentiferChar(char ch)
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

bool IsSpace(char ch)
{
    return isspace(ch);
}

bool IsStopWord(char ch)
{
    char dest[] = "=:%./|!#$@<>[](){};";
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
    KxStringBuffer_Clear(tokenizer->Content);

    char ch = ForwardChar(tokenizer);
    // add identifier first char
    KxStringBuffer_NAppendChar(tokenizer->Content, ch, 1);

    const char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (IsSpace(pc[0]) || IsStopWord(pc[0])) {
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->Content, ch, 1);
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}


void ParseString(KonTokenizer* tokenizer)
{
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    KxStringBuffer_Clear(tokenizer->Content);

    char ch = ForwardChar(tokenizer);

    char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == '\"') {
            ForwardChar(tokenizer);
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->Content, ch, 1);
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}


// like 'abc'
void ParseRawString(KonTokenizer* tokenizer)
{
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    KxStringBuffer_Clear(tokenizer->Content);

    char ch = ForwardChar(tokenizer);

    char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == '\'') {
            ForwardChar(tokenizer);
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->Content, ch, 1);
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}

// single line comment like `` xxx
void ParseSingleLineComment(KonTokenizer* tokenizer)
{
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    KxStringBuffer_Clear(tokenizer->Content);
    // forward two char  "//"
    ForwardChar(tokenizer);
    ForwardChar(tokenizer);

    char ch = '\0';
    char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == '\n') {
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->Content, ch, 1);
    }
    tokenizer->RowEnd = tokenizer->CurrRow;
    tokenizer->ColEnd = tokenizer->CurrCol;
}

void ParseNumber(KonTokenizer* tokenizer)
{
    tokenizer->RowStart = tokenizer->CurrRow;
    tokenizer->ColStart = tokenizer->CurrCol;
    // initialize
    KxStringBuffer_Clear(tokenizer->Content);
    tokenizer->NumIsPositive = true;
    tokenizer->NumIsPowerPositive = true;
    KxStringBuffer_Clear(tokenizer->NumBeforeDot);
    KxStringBuffer_Clear(tokenizer->NumAfterDot);
    KxStringBuffer_Clear(tokenizer->NumAfterPower);

    // add first char of this num
    char ch = ForwardChar(tokenizer);
    KxStringBuffer_NAppendChar(tokenizer->Content, ch, 1);

    if (ch == '-') {
        tokenizer->NumIsPositive = false;
    }
    else if (ch == '+') {
        tokenizer->NumIsPositive = true;
    }
    else {
        // is positive number
        KxStringBuffer_NAppendChar(tokenizer->NumBeforeDot, ch, 1);
    }
    // state of parsing number
    // 1 parse num before dot, 2 parse num after dot, 3 parse powver
    int state = 1;

    char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        char stopChars[16] = ":%|![](){}<>;";
        if (IsSpace(pc[0]) || strchr(stopChars, pc[0]) > 0) {
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->Content, ch, 1);

        if (state == 1 && ch != '.' && ch != 'e' && ch != 'E') {
            KxStringBuffer_NAppendChar(tokenizer->NumBeforeDot, ch, 1);
        }
        else if (state == 2 && ch != 'e' && ch != 'E') {
            KxStringBuffer_NAppendChar(tokenizer->NumAfterDot, ch, 1);
        }
        else if (state == 3 && ch == '-') {
            tokenizer->NumIsPowerPositive = false;
        }
        else if (state == 3 && ch != '-') {
            KxStringBuffer_NAppendChar(tokenizer->NumAfterPower, ch, 1);
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
    KxStringBuffer_Clear(tokenizer->Content);
    KxStringBuffer_AppendCstr(tokenizer->Content, newContent);
}

KonTokenKind KSON_TokenizerNext(KonTokenizer* tokenizer)
{
    // reset
    tokenizer->TokenKind = KN_TOKEN_EOF;
    while (tokenizer->TokenKind == KN_TOKEN_EOF) {
        // peek character
        char* pc = PeekChars(tokenizer, 1);
        if (pc == NULL) {
            // reach file end
            break;
        }
        if (IsSpace(pc[0])) {
            SkipWhiteSpaces(tokenizer);
        }
        
        else if (pc[0] == '(') {
            UpdateTokenContent(tokenizer, "(");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KN_TOKEN_PARAM_START;
            break;
        }
        else if (pc[0] == ')') {
            UpdateTokenContent(tokenizer, ")");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KN_TOKEN_TABLE_END;
            break;
        }
        else if (pc[0] == '<') {
            const char* nextChars = PeekChars(tokenizer, 2);
            if (nextChars[1] == '=') {
                UpdateTokenContent(tokenizer, "<=");
                ForwardToken(tokenizer, 2);
                tokenizer->TokenKind = KN_TOKEN_SYM_WORD;
            }
            else if (IsSpace(nextChars[1]) || IsStopWord(nextChars[1])) {
                UpdateTokenContent(tokenizer, "<");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KN_TOKEN_SYM_WORD;
                //     tokenizer->TokenKind = KN_TOKEN_VECTOR_START;
            }
            break;
        }
        else if (pc[0] == '>') {
            const char* nextChars = PeekChars(tokenizer, 2);
            if (nextChars[1] == '=') {
                UpdateTokenContent(tokenizer, ">=");
                ForwardToken(tokenizer, 2);
                tokenizer->TokenKind = KN_TOKEN_SYM_WORD;
            }
            else if (IsSpace(nextChars[1]) || IsStopWord(nextChars[1])) {
                UpdateTokenContent(tokenizer, ">");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KN_TOKEN_VECTOR_END;
            }
            break;
        }
        else if (pc[0] == '[') {
            UpdateTokenContent(tokenizer, "[");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KN_TOKEN_LIST_START;
            break;
        }
        else if (pc[0] == ']') {
            UpdateTokenContent(tokenizer, "]");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KN_TOKEN_LIST_END;
            break;
        }
        else if (pc[0] == '{') {
            UpdateTokenContent(tokenizer, "{");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KN_TOKEN_CELL_START;
            break;
        }
        else if (pc[0] == '}') {
            UpdateTokenContent(tokenizer, "}");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KN_TOKEN_CELL_END;
            break;
        }
        else if (pc[0] == '-') {
            const char* nextChars = PeekChars(tokenizer, 2);
            if (nextChars == NULL) {
                break;
            }
            if (IsSpace(nextChars[1])) {
                UpdateTokenContent(tokenizer, "-");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KN_TOKEN_SYM_WORD;
                break;
            }
            else if (IsDigit(nextChars[1])) {
                ParseNumber(tokenizer);
                tokenizer->TokenKind = KN_TOKEN_LITERAL_NUMBER;
                break;
            }
            else {
                ParseIdentifier(tokenizer);
                tokenizer->TokenKind = KN_TOKEN_SYM_WORD;
            }
        }
        else if (pc[0] == '+') {
            const char* nextChars = PeekChars(tokenizer, 2);
            if (nextChars == NULL) {
                break;
            }
            if (IsSpace(nextChars[1])) {
                UpdateTokenContent(tokenizer, "+");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KN_TOKEN_SYM_WORD;
                break;
            }
            else if (IsDigit(nextChars[1])) {
                ParseNumber(tokenizer);
                tokenizer->TokenKind = KN_TOKEN_LITERAL_NUMBER;
                break;
            }
        }
        else if (IsDigit(pc[0])) {
            ParseNumber(tokenizer);
            tokenizer->TokenKind = KN_TOKEN_LITERAL_NUMBER;
            break;
        }
        else if (pc[0] == '#') {
            const char* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == NULL) {
                break;
            }
            if (nextChars[1] == '<') {
                UpdateTokenContent(tokenizer, "#<");
                ForwardToken(tokenizer, 2);
                tokenizer->TokenKind = KN_TOKEN_VECTOR_START;
                break;
            }
            else if (nextChars[1] == '[') {
                UpdateTokenContent(tokenizer, "#[");
                ForwardToken(tokenizer, 2);
                tokenizer->TokenKind = KN_TOKEN_BLOCK_START;
            }
            else if (nextChars[1] == '(') {
                UpdateTokenContent(tokenizer, "#(");
                ForwardToken(tokenizer, 2);
                tokenizer->TokenKind = KN_TOKEN_TABLE_START;
            }
            
            // TODO other immediate atom builders
            else if (nextChars[1] == 'n' && nextChars[2] == 'i') {
                UpdateTokenContent(tokenizer, "#nil;");
                ForwardToken(tokenizer, 5);
                tokenizer->TokenKind = KN_TOKEN_KEYWORD_NIL;
                break;
            }
            else if (nextChars[1] == 'u' && nextChars[2] == 'n') {
                UpdateTokenContent(tokenizer, "#undef;");
                ForwardToken(tokenizer, 7);
                tokenizer->TokenKind = KN_TOKEN_KEYWORD_UNDEF;
                break;
            }
            else if (nextChars[1] == 'u' && nextChars[2] == 'k') {
                UpdateTokenContent(tokenizer, "#ukn;");
                ForwardToken(tokenizer, 5);
                tokenizer->TokenKind = KN_TOKEN_KEYWORD_UKN;
                break;
            }
            else if (nextChars[1] == 't' && nextChars[2] == ';') {
                UpdateTokenContent(tokenizer, "#t;");
                ForwardToken(tokenizer, 3);
                tokenizer->TokenKind = KN_TOKEN_KEYWORD_TRUE;
                break;
            }
            else if (nextChars[1] == 'f' && nextChars[2] == ';') {
                UpdateTokenContent(tokenizer, "#f;");
                ForwardToken(tokenizer, 3);
                tokenizer->TokenKind = KN_TOKEN_KEYWORD_FALSE;
                break;
            }
        }
        else if (pc[0] == ':') {
            const char* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == NULL) {
                break;
            }
            else if (nextChars[1] == '=') {
                UpdateTokenContent(tokenizer, ":=");
                ForwardToken(tokenizer, 2);
                tokenizer->TokenKind = KN_TOKEN_ASSIGN;
            }
            else {
                UpdateTokenContent(tokenizer, ":");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KN_TOKEN_TABLE_TAG;
            }
        }
        else if (pc[0] == '%') {
            UpdateTokenContent(tokenizer, "%");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KN_TOKEN_APPLY;
        }
        else if (pc[0] == '=') {
            UpdateTokenContent(tokenizer, "=");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KN_TOKEN_EQUAL;
        }
        else if (pc[0] == '.') {
            UpdateTokenContent(tokenizer, ".");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KN_TOKEN_MSG_SIGNAL;
        }
        else if (pc[0] == '|') {
            UpdateTokenContent(tokenizer, "|");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KN_TOKEN_PROC_PIPE;
        }
        else if (pc[0] == ';') {
            UpdateTokenContent(tokenizer, ";");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KN_TOKEN_CLAUSE_END;
        }
        else if (pc[0] == '\"') {
            ParseString(tokenizer);
            tokenizer->TokenKind = KN_TOKEN_LITERAL_STRING;
        }
        else if (pc[0] == '\'') {
            ParseRawString(tokenizer);
            tokenizer->TokenKind = KN_TOKEN_LITERAL_RAW_STRING;
        }
        else if (pc[0] == '$') {
            const char* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == NULL) {
                break;
            }
            else if (nextChars[1] == '.') {
                UpdateTokenContent(tokenizer, "$.");
                ForwardToken(tokenizer, 2);
                tokenizer->TokenKind = KN_TOKEN_EXPAND_REPLACE;
            }
            else if (nextChars[1] == '%' && nextChars[2] == '.') {
                UpdateTokenContent(tokenizer, "$%.");
                ForwardToken(tokenizer, 3);
                tokenizer->TokenKind = KN_TOKEN_EXPAND_KV;
            }
            else if (nextChars[1] == '~' && nextChars[2] == '.') {
                UpdateTokenContent(tokenizer, "$~.");
                ForwardToken(tokenizer, 3);
                tokenizer->TokenKind = KN_TOKEN_EXPAND_SEQ;
            }
            else if (nextChars[1] == '{') {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KN_TOKEN_QUOTE_CELL;
            }
            else if (nextChars[1] == '[') {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KN_TOKEN_QUOTE_LIST;
            }
            else if (nextChars[1] == '\'') {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                ParseRawString(tokenizer);
                tokenizer->TokenKind = KN_TOKEN_SYM_STRING;
            }
            else {
                UpdateTokenContent(tokenizer, "$");
                ForwardToken(tokenizer, 1);
                ParseIdentifier(tokenizer);
                tokenizer->TokenKind = KN_TOKEN_SYM_IDENTIFIER;
            }
        }
        else if (pc[0] == '@') {
            const char* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == NULL) {
                break;
            }
            else if (nextChars[1] == '.') {
                UpdateTokenContent(tokenizer, "@.");
                ForwardToken(tokenizer, 2);
                tokenizer->TokenKind = KN_TOKEN_UNQUOTE_REPLACE;
            }
            else if (nextChars[1] == '%' && nextChars[2] == '.') {
                UpdateTokenContent(tokenizer, "@%.");
                ForwardToken(tokenizer, 3);
                tokenizer->TokenKind = KN_TOKEN_UNQUOTE_KV;
            }
            else if (nextChars[1] == '~' && nextChars[2] == '.') {
                UpdateTokenContent(tokenizer, "@~.");
                ForwardToken(tokenizer, 3);
                tokenizer->TokenKind = KN_TOKEN_UNQUOTE_SEQ;
            }
            else if (nextChars[1] == '{') {
                UpdateTokenContent(tokenizer, "@");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KN_TOKEN_QUASI_CELL;
            }
            else if (nextChars[1] == '[') {
                UpdateTokenContent(tokenizer, "@");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KN_TOKEN_QUASI_LIST;
            }
            else {
                UpdateTokenContent(tokenizer, "@");
                ForwardToken(tokenizer, 1);
                ParseIdentifier(tokenizer);
                tokenizer->TokenKind = KN_TOKEN_SYM_VARIABLE;
            }
        }
        else if (pc[0] == '!') {
            ForwardToken(tokenizer, 1);
            ParseIdentifier(tokenizer);
            tokenizer->TokenKind = KN_TOKEN_SYM_PREFIX_WORD;
        }
        else if (pc[0] == '^') {
            ForwardToken(tokenizer, 1);
            ParseIdentifier(tokenizer);
            tokenizer->TokenKind = KN_TOKEN_SYM_SUFFIX_WORD;
        }
        else if (IsIdentiferPrefixChar(pc[0])) {
            ParseIdentifier(tokenizer);
            tokenizer->TokenKind = KN_TOKEN_SYM_WORD;
            break;
        }
        else if (pc[0] == '`') {
            const char* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == NULL) {
                break;
            }
            // `` single line comment
            if (nextChars[1] == '`') {
                ParseSingleLineComment(tokenizer);
                tokenizer->TokenKind = KN_TOKEN_COMMENT_SINGLE_LINE;
                break;
            }
            else {
                // skip
                ForwardToken(tokenizer, 1);
                break;
            }
        }
        else if (pc[0] == '/') {
            const char* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == NULL) {
                break;
            }
            if (IsSpace(nextChars[1])) {
                // divide identifier
                UpdateTokenContent(tokenizer, "/");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KN_TOKEN_SYM_WORD;
                break;
            }
            else {
                UpdateTokenContent(tokenizer, "/");
                ForwardToken(tokenizer, 1);
                tokenizer->TokenKind = KN_TOKEN_GET_SLOT;
                break;
            }
        }
        else if (pc[0] == '*') {
            // multiply identifier
            UpdateTokenContent(tokenizer, "*");
            ForwardToken(tokenizer, 1);
            tokenizer->TokenKind = KN_TOKEN_SYM_WORD;
            break;
        }
        else {
            KN_DEBUG("neet non space char %d", pc[0]);
            break;
        }
    }
    if (tokenizer->TokenKind == KN_TOKEN_NONE) {
        tokenizer->TokenKind = KN_TOKEN_EOF;
    }
    return tokenizer->TokenKind;
}



