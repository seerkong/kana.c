#include "tokenizer.h"
#include <ctype.h>


void KSON_TokenToString(KonTokenizer* tokenizer)
{
    KxStringBuffer* tokenKind = KxStringBuffer_New();

    switch (tokenizer->tokenKind) {
        case KN_TOKEN_EOF:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_EOF");
            break;
        case KN_TOKEN_LIST_START:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_LIST_START");
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
        case KN_TOKEN_TABLE_END:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_TABLE_END");
            break;
        case KN_TOKEN_CELL_START:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_CELL_START");
            break;
        case KN_TOKEN_CELL_END:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_CELL_END");
            break;
        case KN_TOKEN_KV_PAIR_TAG:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_KV_PAIR_TAG");
            break;
        case KN_TOKEN_CHAR:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_CHAR");
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
        case KN_TOKEN_LITERAL_NUMBER:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_LITERAL_NUMBER");
            break;
        case KN_TOKEN_LITERAL_STRING:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_LITERAL_STRING");
            break;
        case KN_TOKEN_SYM_WORD:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_SYM_WORD");
            break;
        case KN_TOKEN_GET_LVALUE:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_GET_LVALUE");
            break;
        case KN_TOKEN_GET_RVALUE:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_GET_RVALUE");
            break;
        case KN_TOKEN_LITERAL_RAW_STRING:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_LITERAL_RAW_STRING");
            break;
        case KN_TOKEN_SYM_VARIABLE:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_SYM_VARIABLE");
            break;
        case KN_TOKEN_SYM_MARCRO:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_SYM_MARCRO");
            break;
        case KN_TOKEN_PREFIX_WRAPPER:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_PREFIX_WRAPPER");
            break;
        case KN_TOKEN_SUFFIX_WRAPPER:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_SUFFIX_WRAPPER");
            break;
        case KN_TOKEN_TXT_MARCRO:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_TXT_MARCRO");
            break;
        case KN_TOKEN_OBJ_BUILDER:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_OBJ_BUILDER");
            break;
        case KN_TOKEN_SYM_CELL_SEG_END:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_SYM_CELL_SEG_END");
            break;
        case KN_TOKEN_SYM_IDENTIFIER:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_SYM_IDENTIFIER");
            break;
        case KN_TOKEN_SYM_STRING:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_SYM_STRING");
            break;
        case KN_TOKEN_QUOTE:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_QUOTE");
            break;
        case KN_TOKEN_QUASI:
            KxStringBuffer_AppendCstr(tokenKind, "KN_TOKEN_QUASI");
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
//    KN_DEBUG("<%s (:row %ld, :col %ld, :row-end %ld, :col-end %ld) [%s]>", KxStringBuffer_Cstr(tokenKind), tokenizer->rowStart, tokenizer->colStart, tokenizer->rowEnd, tokenizer->colEnd, KxStringBuffer_Cstr(tokenizer->content));
    KN_DEBUG("<%s (:row %d, :col %d) [%s]>", KxStringBuffer_Cstr(tokenKind), tokenizer->rowStart, tokenizer->colStart, KxStringBuffer_Cstr(tokenizer->content));
}

KonTokenizer* KSON_TokenizerInit(KonState* kstate)
{
    // init
    KonTokenizer* tokenizer = (KonTokenizer*)tb_malloc(sizeof(KonTokenizer));

    // init string
    tokenizer->content = KxStringBuffer_New();

    tokenizer->readCursor = 0;
    // stream offset
    tokenizer->currRow = 1;
    tokenizer->currCol = 1;

    // token's start end row col
    tokenizer->rowStart = 1;
    tokenizer->rowEnd = 1;
    tokenizer->colStart = 1;
    tokenizer->colEnd = 1;

    tokenizer->numIsPositive = true;
    tokenizer->numIsPowerPositive = true;
    tokenizer->numBeforeDot = KxStringBuffer_New();
    tokenizer->numAfterDot = KxStringBuffer_New();
    tokenizer->numAfterPower = KxStringBuffer_New();

    return tokenizer;
}

bool KSON_TokenizerBegin(KonTokenizer* tokenizer, KxStringBuffer* codeString)
{
    // init the reade stream
    tokenizer->codeString = codeString;
    tokenizer->codeLen = KxStringBuffer_Length(codeString);
    // clear text
    KxStringBuffer_Clear(tokenizer->content);

    return true;
}

void KSON_TokenizerEnd(KonTokenizer* tokenizer)
{

    // clear text
    KxStringBuffer_Clear(tokenizer->content);
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

    if (tokenizer->readCursor + n > tokenizer->codeLen) {
        return NULL;
    }
    const char* pc = KxStringBuffer_Cstr(tokenizer->codeString) + tokenizer->readCursor;

    return pc;
}

// move stream offset
const char ForwardChar(KonTokenizer* tokenizer)
{
    if (tokenizer->readCursor >= tokenizer->codeLen) {
        return '\0';
    }
    
    const char ch = KxStringBuffer_CharAt(tokenizer->codeString, tokenizer->readCursor);
    if (ch != '\0') {
        if (ch == '\n') {
            tokenizer->currRow = tokenizer->currRow + 1;
            tokenizer->currCol = 1;
        }
        else {
            tokenizer->currCol = tokenizer->currCol + 1;
        }

        tokenizer->readCursor += 1;
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

bool IsIdentifierStopWord(char ch)
{
    char dest[] = " =:.,;/\\|@#$%~^*&<>[](){}";
    if (strchr(dest, ch) > 0) {
        return true;
    }
    else {
        return false;
    }
}

// $.>= $.<=
bool IsQuoteSymStopWord(char ch)
{
    char dest[] = " :.,;/\\|@#$%~^*&[](){}";
    if (strchr(dest, ch) > 0) {
        return true;
    }
    else {
        return false;
    }
}

void ForwardToken(KonTokenizer* tokenizer, int len)
{
    tokenizer->rowStart = tokenizer->currRow;
    tokenizer->colStart = tokenizer->currCol;
    for (int i = 0; i < len; i++) {
        ForwardChar(tokenizer);
    }
    tokenizer->rowEnd = tokenizer->currRow;
    tokenizer->colEnd = tokenizer->currCol;
}

void ParseIdentifier(KonTokenizer* tokenizer)
{
    tokenizer->rowStart = tokenizer->currRow;
    tokenizer->colStart = tokenizer->currCol;
    KxStringBuffer_Clear(tokenizer->content);

    char ch = ForwardChar(tokenizer);
    // add identifier first char
    KxStringBuffer_NAppendChar(tokenizer->content, ch, 1);

    const char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (IsSpace(pc[0]) || IsIdentifierStopWord(pc[0])) {
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->content, ch, 1);
    }
    tokenizer->rowEnd = tokenizer->currRow;
    tokenizer->colEnd = tokenizer->currCol;
}

void ParseQuoteSym(KonTokenizer* tokenizer)
{
    tokenizer->rowStart = tokenizer->currRow;
    tokenizer->colStart = tokenizer->currCol;
    KxStringBuffer_Clear(tokenizer->content);

    char ch = ForwardChar(tokenizer);
    // add identifier first char
    KxStringBuffer_NAppendChar(tokenizer->content, ch, 1);

    const char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (IsSpace(pc[0]) || IsQuoteSymStopWord(pc[0])) {
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->content, ch, 1);
    }
    tokenizer->rowEnd = tokenizer->currRow;
    tokenizer->colEnd = tokenizer->currCol;
}

void ParseChar(KonTokenizer* tokenizer)
{
    tokenizer->rowStart = tokenizer->currRow;
    tokenizer->colStart = tokenizer->currCol;
    KxStringBuffer_Clear(tokenizer->content);

    ForwardChar(tokenizer);
    ForwardChar(tokenizer);
    ForwardChar(tokenizer);
    char ch;
    const char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == ';') {
            ForwardChar(tokenizer);
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->content, ch, 1);
    }
    tokenizer->rowEnd = tokenizer->currRow;
    tokenizer->colEnd = tokenizer->currCol;
}

void ParseTextBuilder(KonTokenizer* tokenizer)
{
    tokenizer->rowStart = tokenizer->currRow;
    tokenizer->colStart = tokenizer->currCol;
    KxStringBuffer_Clear(tokenizer->content);

    char ch = ForwardChar(tokenizer);
    const char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == '.') {
            ForwardChar(tokenizer);
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->content, ch, 1);
    }
    tokenizer->rowEnd = tokenizer->currRow;
    tokenizer->colEnd = tokenizer->currCol;
}

void ParseString(KonTokenizer* tokenizer)
{
    tokenizer->rowStart = tokenizer->currRow;
    tokenizer->colStart = tokenizer->currCol;
    KxStringBuffer_Clear(tokenizer->content);

    char ch = ForwardChar(tokenizer);

    char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == '\"') {
            ForwardChar(tokenizer);
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->content, ch, 1);
    }
    tokenizer->rowEnd = tokenizer->currRow;
    tokenizer->colEnd = tokenizer->currCol;
}


// like 'abc'
void ParseRawString(KonTokenizer* tokenizer)
{
    tokenizer->rowStart = tokenizer->currRow;
    tokenizer->colStart = tokenizer->currCol;
    KxStringBuffer_Clear(tokenizer->content);

    char ch = ForwardChar(tokenizer);

    char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == '\'') {
            ForwardChar(tokenizer);
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->content, ch, 1);
    }
    tokenizer->rowEnd = tokenizer->currRow;
    tokenizer->colEnd = tokenizer->currCol;
}

void SkipUnixScriptFistLine(KonTokenizer* tokenizer)
{
    tokenizer->rowStart = tokenizer->currRow;
    tokenizer->colStart = tokenizer->currCol;
    KxStringBuffer_Clear(tokenizer->content);
    // forward two char  "#!"
    ForwardChar(tokenizer);
    ForwardChar(tokenizer);

    char ch = '\0';
    char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == '\n') {
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->content, ch, 1);
    }
    tokenizer->rowEnd = tokenizer->currRow;
    tokenizer->colEnd = tokenizer->currCol;
}

// single line comment like `` xxx
void ParseSingleLineComment(KonTokenizer* tokenizer)
{
    tokenizer->rowStart = tokenizer->currRow;
    tokenizer->colStart = tokenizer->currCol;
    KxStringBuffer_Clear(tokenizer->content);
    // forward two char  "``"
    ForwardChar(tokenizer);
    ForwardChar(tokenizer);

    char ch = '\0';
    char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        if (pc[0] == '\n') {
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->content, ch, 1);
    }
    tokenizer->rowEnd = tokenizer->currRow;
    tokenizer->colEnd = tokenizer->currCol;
}

// `* * adsf * *`
void ParseMultiLineComment(KonTokenizer* tokenizer)
{
    tokenizer->rowStart = tokenizer->currRow;
    tokenizer->colStart = tokenizer->currCol;
    KxStringBuffer_Clear(tokenizer->content);
    // forward two char  "``"
    ForwardChar(tokenizer);
    ForwardChar(tokenizer);

    char ch = '\0';
    char* pc = NULL;
    char lastCh = '\0';
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        lastCh = ch;
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->content, ch, 1);
        if (lastCh == '*' && ch == '`') {
            break;
        }
    }
    tokenizer->rowEnd = tokenizer->currRow;
    tokenizer->colEnd = tokenizer->currCol;
}

void ParseNumber(KonTokenizer* tokenizer)
{
    tokenizer->rowStart = tokenizer->currRow;
    tokenizer->colStart = tokenizer->currCol;
    // initialize
    KxStringBuffer_Clear(tokenizer->content);
    tokenizer->numIsPositive = true;
    tokenizer->numIsPowerPositive = true;
    KxStringBuffer_Clear(tokenizer->numBeforeDot);
    KxStringBuffer_Clear(tokenizer->numAfterDot);
    KxStringBuffer_Clear(tokenizer->numAfterPower);

    // add first char of this num
    char ch = ForwardChar(tokenizer);
    KxStringBuffer_NAppendChar(tokenizer->content, ch, 1);

    if (ch == '-') {
        tokenizer->numIsPositive = false;
    }
    else if (ch == '+') {
        tokenizer->numIsPositive = true;
    }
    else {
        // is positive number
        KxStringBuffer_NAppendChar(tokenizer->numBeforeDot, ch, 1);
    }
    // state of parsing number
    // 1 parse num before dot, 2 parse num after dot, 3 parse powver
    int state = 1;
    char stopChars[16] = ":%|![](){}<>;";

    char* pc = NULL;
    while ((pc = PeekChars(tokenizer, 1)) && pc) {
        
        if (IsSpace(pc[0]) || strchr(stopChars, pc[0]) > 0) {
            break;
        }
        ch = ForwardChar(tokenizer);
        KxStringBuffer_NAppendChar(tokenizer->content, ch, 1);

        if (state == 1 && ch != '.' && ch != 'e' && ch != 'E') {
            KxStringBuffer_NAppendChar(tokenizer->numBeforeDot, ch, 1);
        }
        else if (state == 2 && ch != 'e' && ch != 'E') {
            KxStringBuffer_NAppendChar(tokenizer->numAfterDot, ch, 1);
        }
        else if (state == 3 && ch == '-') {
            tokenizer->numIsPowerPositive = false;
        }
        else if (state == 3 && ch != '-') {
            KxStringBuffer_NAppendChar(tokenizer->numAfterPower, ch, 1);
        }

        if (pc[0] == '.') {
            state = 2;
        }
        else if (pc[0] == 'e' || pc[0] == 'E') {
            state = 3;
        }
    }
    tokenizer->rowEnd = tokenizer->currRow;
    tokenizer->colEnd = tokenizer->currCol;
}

void UpdateTokenContent(KonTokenizer* tokenizer, char* newContent)
{
    KxStringBuffer_Clear(tokenizer->content);
    KxStringBuffer_AppendCstr(tokenizer->content, newContent);
}

KonTokenKind KSON_TokenizerNext(KonTokenizer* tokenizer)
{
    // reset
    tokenizer->tokenKind = KN_TOKEN_EOF;
    while (tokenizer->tokenKind == KN_TOKEN_EOF) {
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
            tokenizer->tokenKind = KN_TOKEN_TABLE_START;
        }
        else if (pc[0] == ')') {
            UpdateTokenContent(tokenizer, ")");
            ForwardToken(tokenizer, 1);
            tokenizer->tokenKind = KN_TOKEN_TABLE_END;
        }
        else if (pc[0] == '<') {
            // const char* nextChars = PeekChars(tokenizer, 2);
            // if (nextChars[1] == '=') {
            //     UpdateTokenContent(tokenizer, "<=");
            //     ForwardToken(tokenizer, 2);
            //     tokenizer->tokenKind = KN_TOKEN_SYM_WORD;
            // }
            // else if (IsSpace(nextChars[1]) || IsIdentifierStopWord(nextChars[1])) {
                UpdateTokenContent(tokenizer, "<");
                ForwardToken(tokenizer, 1);
                tokenizer->tokenKind = KN_TOKEN_VECTOR_START;
                // tokenizer->tokenKind = KN_TOKEN_VECTOR_START;
            // }
        }
        else if (pc[0] == '>') {
            // const char* nextChars = PeekChars(tokenizer, 2);
            // if (nextChars[1] == '=') {
            //     UpdateTokenContent(tokenizer, ">=");
            //     ForwardToken(tokenizer, 2);
            //     tokenizer->tokenKind = KN_TOKEN_SYM_WORD;
            // }
            // else if (IsSpace(nextChars[1]) || IsIdentifierStopWord(nextChars[1])) {
                UpdateTokenContent(tokenizer, ">");
                ForwardToken(tokenizer, 1);
                tokenizer->tokenKind = KN_TOKEN_VECTOR_END;
            // }
        }
        else if (pc[0] == '{') {
            UpdateTokenContent(tokenizer, "{");
            ForwardToken(tokenizer, 1);
            tokenizer->tokenKind = KN_TOKEN_LIST_START;
        }
        else if (pc[0] == '}') {
            UpdateTokenContent(tokenizer, "}");
            ForwardToken(tokenizer, 1);
            tokenizer->tokenKind = KN_TOKEN_LIST_END;
        }
        else if (pc[0] == '[') {
            UpdateTokenContent(tokenizer, "[");
            ForwardToken(tokenizer, 1);
            tokenizer->tokenKind = KN_TOKEN_CELL_START;
        }
        else if (pc[0] == ']') {
            UpdateTokenContent(tokenizer, "]");
            ForwardToken(tokenizer, 1);
            tokenizer->tokenKind = KN_TOKEN_CELL_END;
        }
        else if (pc[0] == '-') {
            const char* nextChars = PeekChars(tokenizer, 2);
            if (nextChars == NULL) {
                break;
            }
            if (IsSpace(nextChars[1])) {
                UpdateTokenContent(tokenizer, "-");
                ForwardToken(tokenizer, 1);
                tokenizer->tokenKind = KN_TOKEN_SYM_WORD;
            }
            else if (IsDigit(nextChars[1])) {
                ParseNumber(tokenizer);
                tokenizer->tokenKind = KN_TOKEN_LITERAL_NUMBER;
            }
            else {
                ParseIdentifier(tokenizer);
                tokenizer->tokenKind = KN_TOKEN_SYM_WORD;
            }
        }
        else if (pc[0] == '+') {
            const char* nextChars = PeekChars(tokenizer, 2);
            if (nextChars == NULL) {
                break;
            }
            else if (IsSpace(nextChars[1])) {
                UpdateTokenContent(tokenizer, "+");
                ForwardToken(tokenizer, 1);
                tokenizer->tokenKind = KN_TOKEN_SYM_WORD;

            }
            else if (IsDigit(nextChars[1])) {
                ParseNumber(tokenizer);
                tokenizer->tokenKind = KN_TOKEN_LITERAL_NUMBER;
            }
        }
        else if (IsDigit(pc[0])) {
            ParseNumber(tokenizer);
            tokenizer->tokenKind = KN_TOKEN_LITERAL_NUMBER;
        }
        else if (pc[0] == '#') {
            const char* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == NULL) {
                break;
            }
            else if (nextChars[1] == '!') {
                SkipUnixScriptFistLine(tokenizer);
                tokenizer->tokenKind = KN_TOKEN_UNIX_SCRIPT_FIST_LINE;
            }
            // TODO nextChars[1] == '#' as typecast prefix

            else if (nextChars[1] == '<'
                || nextChars[1] == '{'
                || nextChars[1] == '('
                || nextChars[1] == '['
            ) {
                UpdateTokenContent(tokenizer, "");
                ForwardToken(tokenizer, 1);
                tokenizer->tokenKind = KN_TOKEN_OBJ_BUILDER;
            }
            else {
                ParseTextBuilder(tokenizer);
                tokenizer->tokenKind = KN_TOKEN_OBJ_BUILDER;
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
                tokenizer->tokenKind = KN_TOKEN_ASSIGN;
            }
            else {
                UpdateTokenContent(tokenizer, ":");
                ForwardToken(tokenizer, 1);
                tokenizer->tokenKind = KN_TOKEN_KV_PAIR_TAG;
            }
        }
        else if (pc[0] == '%') {
            const char* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == NULL) {
                break;
            }
            else if (IsSpace(nextChars[1])) {
                UpdateTokenContent(tokenizer, "%");
                ForwardToken(tokenizer, 1);
                tokenizer->tokenKind = KN_TOKEN_APPLY;
            }
            else {
                ForwardToken(tokenizer, 1);
                ParseIdentifier(tokenizer);
                tokenizer->tokenKind = KN_TOKEN_SYM_VARIABLE;
            }
        }
        else if (pc[0] == '^') {
            ForwardToken(tokenizer, 1);
            ParseIdentifier(tokenizer);
            tokenizer->tokenKind = KN_TOKEN_SYM_CELL_SEG_END;
        }
        else if (pc[0] == '&') {
            // text builders
            const char* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == NULL) {
                break;
            }
            else if (nextChars[1] == 'c' && nextChars[2] == ',') {
                ParseChar(tokenizer);
                tokenizer->tokenKind = KN_TOKEN_CHAR;
            }
            else {
                ParseTextBuilder(tokenizer);
                tokenizer->tokenKind = KN_TOKEN_TXT_MARCRO;
            }
        }
        else if (pc[0] == '=') {
            UpdateTokenContent(tokenizer, "=");
            ForwardToken(tokenizer, 1);
            tokenizer->tokenKind = KN_TOKEN_EQUAL;
        }
        else if (pc[0] == '.') {
            UpdateTokenContent(tokenizer, ".");
            ForwardToken(tokenizer, 1);
            tokenizer->tokenKind = KN_TOKEN_MSG_SIGNAL;
        }
        else if (pc[0] == '|') {
            UpdateTokenContent(tokenizer, "|");
            ForwardToken(tokenizer, 1);
            tokenizer->tokenKind = KN_TOKEN_PROC_PIPE;
        }
        else if (pc[0] == ';') {
            UpdateTokenContent(tokenizer, ";");
            ForwardToken(tokenizer, 1);
            tokenizer->tokenKind = KN_TOKEN_CLAUSE_END;
        }
        else if (pc[0] == '\"') {
            ParseString(tokenizer);
            tokenizer->tokenKind = KN_TOKEN_LITERAL_STRING;
        }
        else if (pc[0] == '\'') {
            ParseRawString(tokenizer);
            tokenizer->tokenKind = KN_TOKEN_LITERAL_RAW_STRING;
        }
        else if (pc[0] == '$') {
            const char* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == NULL) {
                break;
            }
            else if (nextChars[1] == '\'') {
                UpdateTokenContent(tokenizer, "");
                ForwardToken(tokenizer, 1);
                ParseRawString(tokenizer);
                tokenizer->tokenKind = KN_TOKEN_SYM_STRING;
            }
            else if (nextChars[1] == '%') {
                UpdateTokenContent(tokenizer, "$%");
                ForwardToken(tokenizer, 2);
                tokenizer->tokenKind = KN_TOKEN_UNQUOTE_REPLACE;
            }
            else if (nextChars[1] == '~') {
                UpdateTokenContent(tokenizer, "$~");
                ForwardToken(tokenizer, 2);
                tokenizer->tokenKind = KN_TOKEN_UNQUOTE_SEQ;
            }
            else if (nextChars[1] == '@') {
                UpdateTokenContent(tokenizer, "$@");
                ForwardToken(tokenizer, 2);
                tokenizer->tokenKind = KN_TOKEN_UNQUOTE_KV;
            }
            else if (nextChars[1] == '{'
                || nextChars[1] == '['
            ) {
                UpdateTokenContent(tokenizer, "");
                ForwardToken(tokenizer, 1);
                tokenizer->tokenKind = KN_TOKEN_QUOTE;
            }
            else if (nextChars[1] == '.') {
                // if (nextChars[2] == '{' || nextChars[2] == '['
                //     || nextChars[2] == '(' || nextChars[2] == '<'
                // ) {
                    UpdateTokenContent(tokenizer, "");
                    ForwardToken(tokenizer, 2);
                    tokenizer->tokenKind = KN_TOKEN_QUOTE;
                // }
                // else {
                //     ForwardToken(tokenizer, 2);
                //     ParseIdentifier(tokenizer);
                //     tokenizer->tokenKind = KN_TOKEN_SYM_IDENTIFIER;
                // }
            }
            else {
                ForwardToken(tokenizer, 1);
                ParseQuoteSym(tokenizer);
                tokenizer->tokenKind = KN_TOKEN_SYM_IDENTIFIER;
            }
        }
        else if (pc[0] == '!') {
            const char* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == NULL) {
                break;
            }
            else if (nextChars[1] == '{'
                || nextChars[1] == '['
            ) {
                UpdateTokenContent(tokenizer, "");
                ForwardToken(tokenizer, 1);
                tokenizer->tokenKind = KN_TOKEN_QUASI;
            }
            else if (nextChars[1] == '.') {
                // if (nextChars[2] == '{' || nextChars[2] == '[') {
                    UpdateTokenContent(tokenizer, "");
                    ForwardToken(tokenizer, 2);
                    tokenizer->tokenKind = KN_TOKEN_QUASI;
                // }
                // else {
                //     ForwardToken(tokenizer, 2);
                //     ParseIdentifier(tokenizer);
                //     tokenizer->tokenKind = KN_TOKEN_SYM_VARIABLE;
                // }
                
            }
            else {
                // tagged quasiquote eg: !ass.[1]
                ForwardToken(tokenizer, 1);
                ParseIdentifier(tokenizer);
                ForwardToken(tokenizer, 1);
                tokenizer->tokenKind = KN_TOKEN_QUASI;
            }
            break;
        }
        else if (pc[0] == '@') {
            UpdateTokenContent(tokenizer, "@");
            ForwardToken(tokenizer, 1);
            tokenizer->tokenKind = KN_TOKEN_PREFIX_WRAPPER;
        }
        else if (pc[0] == '~') {
            UpdateTokenContent(tokenizer, "~");
            ForwardToken(tokenizer, 1);
            tokenizer->tokenKind = KN_TOKEN_SUFFIX_WRAPPER;
        }
        
        else if (IsIdentiferPrefixChar(pc[0])) {
            ParseIdentifier(tokenizer);
            const char* content = KxStringBuffer_Cstr(tokenizer->content);
            int idLen = strlen(content);
            if (strcmp(content, "true") == 0) {
                tokenizer->tokenKind = KN_TOKEN_KEYWORD_TRUE;
            }
            else if (strcmp(content, "false") == 0) {
                tokenizer->tokenKind = KN_TOKEN_KEYWORD_FALSE;
            }
            else if (strcmp(content, "nil") == 0) {
                tokenizer->tokenKind = KN_TOKEN_KEYWORD_NIL;
            }
            else if (strcmp(content, "ukn") == 0) {
                tokenizer->tokenKind = KN_TOKEN_KEYWORD_UKN;
            }
            else if (strcmp(content, "undefined") == 0) {
                tokenizer->tokenKind = KN_TOKEN_KEYWORD_UNDEF;
            }
            else if (content[idLen-1] == '!') {
                tokenizer->tokenKind = KN_TOKEN_SYM_MARCRO;
            }
            else {
                tokenizer->tokenKind = KN_TOKEN_SYM_WORD;
            }
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
                tokenizer->tokenKind = KN_TOKEN_COMMENT_SINGLE_LINE;
                break;
            }
            if (nextChars[1] == '*') {
                ParseMultiLineComment(tokenizer);
                tokenizer->tokenKind = KN_TOKEN_COMMENT_MULTI_LINE;
                break;
            }
            else {
                // skip
                ForwardToken(tokenizer, 1);
                break;
            }
        }
        else if (pc[0] == '/') {
            UpdateTokenContent(tokenizer, "/");
            ForwardToken(tokenizer, 1);
            tokenizer->tokenKind = KN_TOKEN_SYM_WORD;
            break;
        }
        else if (pc[0] == '\\') {
            const char* nextChars = PeekChars(tokenizer, 3);
            if (nextChars == NULL) {
                break;
            }
            if (nextChars[1] == ':') {
                UpdateTokenContent(tokenizer, "\\:");
                ForwardToken(tokenizer, 2);
                tokenizer->tokenKind = KN_TOKEN_GET_LVALUE;
                break;
            }
            else {
                UpdateTokenContent(tokenizer, "\\");
                ForwardToken(tokenizer, 1);
                tokenizer->tokenKind = KN_TOKEN_GET_RVALUE;
                break;
            }
        }
        else if (pc[0] == '*') {
            // multiply identifier
            UpdateTokenContent(tokenizer, "*");
            ForwardToken(tokenizer, 1);
            tokenizer->tokenKind = KN_TOKEN_SYM_WORD;
            break;
        }
        else {
            KN_DEBUG("neet non space char %d", pc[0]);
            break;
        }
    }
    if (tokenizer->tokenKind == KN_TOKEN_NONE) {
        tokenizer->tokenKind = KN_TOKEN_EOF;
    }
    return tokenizer->tokenKind;
}



