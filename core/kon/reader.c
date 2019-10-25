#include "reader.h"

void ExitTopBuilder(KonReader* reader);
void AddValueToTopBuilder(KonReader* reader, KN value);


KonReader* KSON_ReaderInit(KonState* kstate)
{
    // init reader
    KonReader* reader = (KonReader*)tb_malloc(sizeof(KonReader));
    KN_DEBUG("malloc reader %x", reader);

    reader->kstate = kstate;
    
    
    reader->tokenizer = KSON_TokenizerInit(kstate);
    KN_DEBUG("after KSON_TokenizerInit");
    
    reader->builderStack = BuilderStackInit();
    reader->stateStack = StateStackInit();
    reader->wordAsIdentifier = false;

    return reader;
}

bool KSON_ReaderFromFile(KonReader* reader, const char* sourceFilePath)
{
    KxStringBuffer* sb = KN_ReadFileContent(sourceFilePath);
    // prepend '[' at head, append ']' at tail
    KxStringBuffer_NPrependCstr(sb, "{\n", 1);
    KxStringBuffer_AppendCstr(sb, "\n}");
    bool res = KSON_ReaderFromCstr(reader, KxStringBuffer_Cstr(sb));
    KxStringBuffer_Clear(sb);
    return res;
}

bool KSON_ReaderFromCstr(KonReader* reader, const char* sourceCstr)
{
    reader->readStream = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(reader->readStream, sourceCstr);
    KSON_TokenizerBegin(reader->tokenizer, reader->readStream);
    return true;
}

bool IsSkipToken(int event)
{
    if (event == KN_TOKEN_NONE
        || event == KN_TOKEN_WHITESPACE
        || event == KN_TOKEN_COMMENT_SINGLE_LINE
    ) {
        return true;
    }
    else {
        return false;
    }
}

// { [ ( <
bool IsContainerStartToken(int event)
{
    if (event == KN_TOKEN_LIST_START
        || event == KN_TOKEN_VECTOR_START
        || event == KN_TOKEN_TABLE_START
        || event == KN_TOKEN_CELL_START
    ) {
        return true;
    }
    else {
        return false;
    }
}

bool IsContainerEndToken(int event)
{
    if (event == KN_TOKEN_LIST_END
        || event == KN_TOKEN_VECTOR_END
        || event == KN_TOKEN_TABLE_END
        || event == KN_TOKEN_CELL_END
    ) {
        return true;
    }
    else {
        return false;
    }
}

// @[] @() @<> @{}
// $[] $() $<> ${}
// $.abc $%.abc $~.abc
// @.abc @.[5 .+ $.a] @%.abc  @~.abc
// ^ss.''
bool IsWrapperToken(int event)
{
    if (event == KN_TOKEN_QUOTE
        || event == KN_TOKEN_QUASI
        || event == KN_TOKEN_UNQUOTE_REPLACE
        || event == KN_TOKEN_UNQUOTE_KV
        || event == KN_TOKEN_UNQUOTE_SEQ
        || event == KN_TOKEN_PREFIX_WRAPPER
        || event == KN_TOKEN_SUFFIX_WRAPPER
        || event == KN_TOKEN_TXT_MARCRO
        || event == KN_TOKEN_OBJ_BUILDER
    ) {
        return true;
    }
    else {
        return false;
    }
}

bool IsLiteralToken(int event)
{
    if (event == KN_TOKEN_KEYWORD_NIL
        || event == KN_TOKEN_KEYWORD_UNDEF
        || event == KN_TOKEN_KEYWORD_UKN
        || event == KN_TOKEN_KEYWORD_TRUE
        || event == KN_TOKEN_KEYWORD_FALSE
        || event == KN_TOKEN_LITERAL_NUMBER
        || event == KN_TOKEN_LITERAL_STRING
        || event == KN_TOKEN_LITERAL_RAW_STRING
        || event == KN_TOKEN_CHAR
    ) {
        return true;
    }
    else {
        return false;
    }
}

// % . | ; = :=
bool IsSyntaxToken(int event)
{
    if (event == KN_TOKEN_APPLY
        || event == KN_TOKEN_EQUAL
        || event == KN_TOKEN_ASSIGN
        || event == KN_TOKEN_PROC_PIPE
        || event == KN_TOKEN_MSG_SIGNAL
        || event == KN_TOKEN_GET_LVALUE
        || event == KN_TOKEN_GET_RVALUE
        || event == KN_TOKEN_CLAUSE_END
    ) {
        return true;
    }
    else {
        return false;
    }
}



KN MakeSyntaxMarker(KonState* kstate, KonTokenKind tokenKind)
{
    KonSyntaxMarker* value = KN_NEW_CONST_OBJ(kstate, KonSyntaxMarker, KN_T_SYNTAX_MARKER);
    switch (tokenKind) {
        case KN_TOKEN_APPLY: {
            value->type = KN_SYNTAX_MARKER_APPLY;
            break;
        }
        case KN_TOKEN_EQUAL: {
            value->type = KN_SYNTAX_MARKER_EQUAL;
            break;
        }
        case KN_TOKEN_ASSIGN: {
            value->type = KN_SYNTAX_MARKER_ASSIGN;
            break;
        }
        case KN_TOKEN_CLAUSE_END: {
            value->type = KN_SYNTAX_MARKER_CLAUSE_END;
            break;
        }
        case KN_TOKEN_PROC_PIPE: {
            value->type = KN_SYNTAX_MARKER_PROC_PIPE;
            break;
        }
        case KN_TOKEN_MSG_SIGNAL: {
            value->type = KN_SYNTAX_MARKER_MSG_SIGNAL;
            break;
        }
        case KN_TOKEN_GET_LVALUE: {
            value->type = KN_SYNTAX_MARKER_GET_LVALUE;
            break;
        }
        case KN_TOKEN_GET_RVALUE: {
            value->type = KN_SYNTAX_MARKER_GET_RVALUE;
            break;
        }
        default: {
            break;
        }
    }
    return KON_2_KN(value);
}

KN MakeSymbol(KonReader* reader, KonTokenKind event)
{
    KonSymbol* value = KN_NEW_CONST_OBJ(reader->kstate, KonSymbol, KN_T_SYMBOL);
    if (event == KN_TOKEN_SYM_MARCRO) {
        value->type = KN_SYM_MARCRO;
    }
    else if (event == KN_TOKEN_SYM_CELL_SEG_END) {
        value->type = KN_SYM_CELL_SEG_END;
    }
    
    else if (event == KN_TOKEN_SYM_WORD) {
        // if in quote or quasiquote scope, convert word to symbol
        // if (reader->wordAsIdentifier) {
        //     value->type = KN_SYM_IDENTIFIER;
        // }
        // else {
            value->type = KN_SYM_WORD;
        // }
    }
    else if (event == KN_TOKEN_SYM_VARIABLE) {
        value->type = KN_SYM_VARIABLE;
    }
    else if (event == KN_TOKEN_SYM_IDENTIFIER) {
        value->type = KN_SYM_IDENTIFIER;
    }
    else if (event == KN_TOKEN_SYM_STRING) {
        value->type = KN_SYM_STRING;
    }


    
    value->data = utf8dup(KxStringBuffer_Cstr(reader->tokenizer->content));

    return KON_2_KN(value);
}

// num token to kon number
KN MakeNumber(KonReader* reader)
{
    bool isPositive = reader->tokenizer->numIsPositive;
    const char* numStrBeforeDot = KxStringBuffer_Cstr(reader->tokenizer->numBeforeDot);
    const char* numStrAfterDot = KxStringBuffer_Cstr(reader->tokenizer->numAfterDot);
    const char* numStrAfterPower = KxStringBuffer_Cstr(reader->tokenizer->numAfterPower);
    
    KN value = KN_ZERO;
    
    if (isPositive
        && (numStrAfterDot == NULL || strcmp(numStrAfterDot, "") == 0)
        && (numStrAfterPower == NULL || strcmp(numStrAfterPower, "") == 0)
    ) {
        // TODO add judgment, if out of range
        // 32 bit system : use 4*8-1 bits;
        // 64 bit system : use 8*8-1 bits;
        int numBeforeDot = atoi(numStrBeforeDot);
        value = KN_MAKE_FIXNUM(numBeforeDot);
    }
    else {
        // TODO add exp suport
        double numBeforeDot = 0;
        while (*numStrBeforeDot != '\0' && *numStrBeforeDot >= '0' && *numStrBeforeDot <= '9') {
            numBeforeDot = numBeforeDot * 10 + (*numStrBeforeDot - '0');
            numStrBeforeDot++;
        }

        // decimal part like 0.xxx
        double numAfterDot = 0;
        double lastDigit = 0.1;
        while (numStrAfterDot != NULL && *numStrAfterDot >= '0' && *numStrAfterDot <= '9') {
            numAfterDot = numAfterDot + lastDigit * (*numStrAfterDot - '0');
            lastDigit = lastDigit * 0.1;
            numStrAfterDot++;
        }
        double num = (numBeforeDot + numAfterDot) * (isPositive ? 1 : -1);
        value = KN_MakeFlonum(reader->kstate, num);
    }
    return value;
}

KN MakeRawString(KonReader* reader)
{
    KN value = KN_MakeString(reader->kstate, KxStringBuffer_Cstr(reader->tokenizer->content));
    return value;
}

KN MakeString(KonReader* reader)
{
    // replace excape chars
    const char* origin = KxStringBuffer_Cstr(reader->tokenizer->content);
    char* index = (char*)origin;
    
    KxStringBuffer* sb = KxStringBuffer_New();
    while (*index != '\0') {
        if (*index == '\\' && *(index + 1) != '\0') {
            if (*(index + 1) == 'n') {
                KxStringBuffer_NAppendChar(sb, '\n', 1);
                index += 2;
                continue;
            }
            else if (*(index + 1) == 'r') {
                KxStringBuffer_NAppendChar(sb, '\r', 1);
                index += 2;
                continue;
            }
            else if (*(index + 1) == 't') {
                KxStringBuffer_NAppendChar(sb, '\t', 1);
                index += 2;
                continue;
            }
        }
        KxStringBuffer_NAppendChar(sb, *index, 1);
        index += 1;
    }
    KonString* value = KN_NEW_CONST_OBJ(reader->kstate, KonString, KN_T_STRING);
    value->string = sb;

    return KON_2_KN(value);
}

KN MakeLiteral(KonReader* reader, KonTokenKind event)
{
    KN value = (KN)KN_UKN;

    if (event == KN_TOKEN_KEYWORD_NIL) {
        value = (KN)KN_NIL;
    }
    else if (event == KN_TOKEN_KEYWORD_UNDEF) {
        value = (KN)KN_UNDEF;
    }
    else if (event == KN_TOKEN_KEYWORD_UKN) {
        value = (KN)KN_UKN;
    }
    else if (event == KN_TOKEN_KEYWORD_TRUE) {
        value = (KN)KN_TRUE;
    }
    else if (event == KN_TOKEN_KEYWORD_FALSE) {
        value = (KN)KN_FALSE;
    }
    else if (event == KN_TOKEN_LITERAL_NUMBER) {
        value = MakeNumber(reader);
    }
    else if (event == KN_TOKEN_LITERAL_STRING) {
        value = MakeString(reader);
    }
    else if (event == KN_TOKEN_LITERAL_RAW_STRING) {
        value = MakeRawString(reader);
    }
    else if (event == KN_TOKEN_CHAR) {
        const char* content = KxStringBuffer_Cstr(reader->tokenizer->content);
        value = KN_BOX_CHAR(content[0]);
    }

    return value;
}

void AddValueToTopBuilder(KonReader* reader, KN value)
{
    KonReaderState currentState = StateStackTop(reader->stateStack);
    KonReaderState newState = currentState;

    if (BuilderStackLength(reader->builderStack) == 0) {
        // parse finish
        reader->root = value;
        return;
    }
    KonBuilder* topBuilder = BuilderStackTop(reader->builderStack);
    
    
    KonBuilderType builderType = topBuilder->type;
    KN_DEBUG("AddValueToTopBuilder top builder type %d", (int)builderType);
    if (builderType == KN_BUILDER_VECTOR) {
        VectorBuilderAddItem(topBuilder, value);
    }
    else if (builderType == KN_BUILDER_LIST) {
        ListBuilderAddItem(topBuilder, value);
    }
    else if (builderType == KN_BUILDER_TABLE) {
        TableBuilderAddValue(topBuilder, value);
    }
    else if (builderType == KN_BUILDER_BLOCK) {
        BlockBuilderAddItem(topBuilder, value);
    }
    else if (builderType == KN_BUILDER_PARAM) {
        ParamBuilderAddValue(topBuilder, value);
    }
    else if (builderType == KN_BUILDER_KV_PAIR) {
        if (currentState == KN_READER_PARSE_TABLE_PAIR_KEY) {
            assert(KN_IS_SYMBOL(value));
            KonSymbolType symbolType = KN_FIELD(value, Symbol, type);
            assert(symbolType != KN_SYM_VARIABLE && symbolType != KN_SYM_MARCRO);
            // table tag key should not be NULL
            char* tableKey = KN_UNBOX_SYMBOL(value);
            assert(tableKey);
            KvPairSetKey(topBuilder, tableKey);

            StateStackSetTopValue(
                reader->stateStack,
                KN_READER_PARSE_TABLE_PAIR_EQ
            );
        }
        else if (currentState == KN_READER_PARSE_MAP_PAIR_KEY) {
            assert(KN_IS_SYMBOL(value));
            KonSymbolType symbolType = KN_FIELD(value, Symbol, type);
            assert(symbolType != KN_SYM_VARIABLE && symbolType != KN_SYM_MARCRO);
            // table tag key should not be NULL
            char* tableKey = KN_UNBOX_SYMBOL(value);
            assert(tableKey);
            KvPairSetKey(topBuilder, tableKey);

            StateStackSetTopValue(
                reader->stateStack,
                KN_READER_PARSE_MAP_PAIR_EQ_OR_TAG
            );
        }
        // meet value, end key val pair (:a 1) (:a = 1)
        else if (currentState == KN_READER_PARSE_TABLE_PAIR_EQ
            || currentState == KN_READER_PARSE_TABLE_PAIR_VAL
        ) {
            KvPairSetValue(topBuilder, value);
            KonBuilder* pairBuilder = BuilderStackPop(reader->builderStack);
            KonBuilder* nextBuilder = BuilderStackTop(reader->builderStack);
            if (nextBuilder->type == KN_BUILDER_TABLE) {
                TableBuilderAddPair(nextBuilder, pairBuilder);
                StateStackPop(reader->stateStack);
            }
            else if (nextBuilder->type == KN_BUILDER_PARAM) {
                ParamBuilderAddPair(nextBuilder, pairBuilder);
                StateStackPop(reader->stateStack);
            }
            
        }
        else if (currentState == KN_READER_PARSE_MAP_PAIR_VAL || currentState == KN_READER_PARSE_MAP_PAIR_EQ_OR_TAG) {
            KvPairSetValue(topBuilder, value);
            KonBuilder* pairBuilder = BuilderStackPop(reader->builderStack);
            KonBuilder* mapBuilder = BuilderStackTop(reader->builderStack);

            // in cell mode, reset to wait map pair|param|block|nextcore state
            if (mapBuilder->type == KN_BUILDER_CELL) {
                CellBuilderAddPair(reader->kstate, mapBuilder, pairBuilder);
                StateStackPop(reader->stateStack);

                StateStackSetTopValue(
                    reader->stateStack,
                    KN_READER_PARSE_CELL_MAP_PAIR
                );
            }
            else {
                MapBuilderAddPair(reader->kstate, mapBuilder, pairBuilder);
                StateStackSetTopValue(
                    reader->stateStack,
                    KN_READER_PARSE_MAP_PAIR_KEY
                );
            }
        }
        else {
            // TODO exception
        }
    }
    else if (builderType == KN_BUILDER_CELL) {
        // create new cell section in CellBuilder
        // 1 core is set, meet next core
        // 2 table is set, meet next core or table
        // 3 list is set, meet next core or table or list
        if (KN_IS_TABLE(value)) {
            CellBuilderSetTable(topBuilder, value);
        }
        else if (KN_IS_PAIR(value)) {
            CellBuilderSetList(topBuilder, value);
        }
        else if (KN_IS_VECTOR(value)) {
            CellBuilderSetVector(topBuilder, value);
        }
        else if (KN_IS_SUFFIX(value)) {
            CellBuilderSetSuffix(topBuilder, value);
        }
        // treat nil a core value
        else if (value.asU64 == KNBOX_NIL) {
            CellBuilderSetCore(topBuilder, value);
        }
        else {
            CellBuilderSetCore(topBuilder, value);
        }
    }
    // when in wrapper builders, should exit this builder
    // after set wrapper inner value
    else if (builderType == KN_BUILDER_QUOTE
        || builderType == KN_BUILDER_QUASIQUOTE
        || builderType == KN_BUILDER_UNQUOTE
        || builderType == KN_BUILDER_PREFIX
        || builderType == KN_BUILDER_SUFFIX
        || builderType == KN_BUILDER_TXT_MARCRO
        || builderType == KN_BUILDER_OBJ_BUILDER
    ) {
        WrapperSetInner(reader->kstate, topBuilder, value);
        ExitTopBuilder(reader);
    }
}

void ExitTopBuilder(KonReader* reader)
{
    
    KonReaderState currentState = StateStackTop(reader->stateStack);
    KonReaderState newState = currentState;

    KonBuilder* topBuilder = BuilderStackTop(reader->builderStack);
    KonBuilderType builderType = topBuilder->type;

    KN_DEBUG("ExitTopBuilder builder type %s", BuilderTypeToCStr(builderType));

    KN value;
    if (builderType == KN_BUILDER_VECTOR) {
        value = MakeVectorByBuilder(reader->kstate, topBuilder);
    }
    else if (builderType == KN_BUILDER_LIST) {
        value = MakeListByBuilder(reader->kstate, topBuilder);
    }
    else if (builderType == KN_BUILDER_BLOCK) {
        value = MakeBlockByBuilder(reader->kstate, topBuilder);

        // a exceptional case, when in a cell, {abc #[]} set #[] to cell list
        // and {abc #nil;} set #nil; to 2nd cell core
        KonBuilder* nextTopBuilder = (KonBuilder*)reader->builderStack->top->next->data;
        if (nextTopBuilder->type == KN_BUILDER_CELL) {
            // resume last container builder
            BuilderStackPop(reader->builderStack);
            // resume last state
            StateStackPop(reader->stateStack);
            CellBuilderSetList(nextTopBuilder, value);

            return;
        }

    }
    else if (builderType == KN_BUILDER_PARAM) {
        value = MakeParamByBuilder(reader->kstate, topBuilder);
    }
    else if (builderType == KN_BUILDER_TABLE) {
        value = MakeTableByBuilder(reader->kstate, topBuilder);
    }
    else if (builderType == KN_BUILDER_MAP) {
        value = MakeMapByBuilder(reader->kstate, topBuilder);
    }
    else if (builderType == KN_BUILDER_CELL) {
        value = MakeCellByBuilder(reader->kstate, topBuilder);
    }
    else if (builderType == KN_BUILDER_QUOTE
        || builderType == KN_BUILDER_QUASIQUOTE
        || builderType == KN_BUILDER_UNQUOTE
        || builderType == KN_BUILDER_PREFIX
        || builderType == KN_BUILDER_SUFFIX
        || builderType == KN_BUILDER_TXT_MARCRO
        || builderType == KN_BUILDER_OBJ_BUILDER
    ) {
        value = MakeWrapperByBuilder(reader->kstate, topBuilder);
        if (builderType == KN_BUILDER_QUOTE || builderType == KN_BUILDER_QUASIQUOTE) {
            // close word to identifier mode ( abc to abc)
            reader->wordAsIdentifier = false;
        }
    }

    // resume last container builder
    BuilderStackPop(reader->builderStack);
    // resume last state
    StateStackPop(reader->stateStack);

    AddValueToTopBuilder(reader, value);    
}

void ExitAllStackBuilders()
{
    KN_DEBUG("ExitAllStackBuilders");
    // KonBuilder* newTopBuilder = BuilderStackTop(reader->builderStack);
}

KN KSON_Parse(KonReader* reader)
{
    int initState = KN_READER_ROOT;
    StateStackPush(reader->stateStack, initState);
    KN_DEBUG("start");

    int event = KN_TOKEN_NONE;
    while ((event = KSON_TokenizerNext(reader->tokenizer)) && event != KN_TOKEN_EOF) {
        KSON_TokenToString(reader->tokenizer);
        if (IsSkipToken(event)) {
            continue;
        }

        KonReaderState currentState = StateStackTop(reader->stateStack);

        if (IsContainerStartToken(event) || IsWrapperToken(event)) {
            // 1 create builder
            // 2 push new builder to builder stack
            // 3 save current state
            // 3 update top state

            // modify cell state, before create new builder
            if (currentState == KN_READER_PARSE_CELL_CORE
            ) {
                StateStackSetTopValue(reader->stateStack, KN_READER_PARSE_CELL_INNER_CONTAINER);
            }
            // {a :b ()} 
            else if (currentState == KN_READER_PARSE_MAP_PAIR_EQ_OR_TAG
            ) {
                AddValueToTopBuilder(reader, KN_UKN);
                // StateStackPop(reader->stateStack);
                // StateStackSetTopValue(reader->stateStack, KN_READER_PARSE_CELL_INNER_CONTAINER);
            }

            KonBuilder* builder;
            if (event == KN_TOKEN_LIST_START) {
                StateStackPush(reader->stateStack, KN_READER_PARSE_LIST);
                builder = CreateListBuilder();
            }
            else if (event == KN_TOKEN_VECTOR_START) {
                StateStackPush(reader->stateStack, KN_READER_PARSE_VECTOR);
                KonBuilder* builder2 = CreateVectorBuilder();
                BuilderStackPush(reader->builderStack, builder2);
                continue;
            }
            else if (event == KN_TOKEN_TABLE_START) {
                StateStackPush(reader->stateStack, KN_READER_PARSE_TABLE);
                builder = CreateTableBuilder();
            }
            else if (event == KN_TOKEN_CELL_START) {
                StateStackPush(reader->stateStack, KN_READER_PARSE_CELL_CORE);
                builder = CreateCellBuilder();
            }
            // wrapper types
            else {
                if (event == KN_TOKEN_QUOTE) {
                    StateStackPush(reader->stateStack, KN_READER_PARSE_QUOTE);
                    builder = CreateWrapperBuilder(KN_BUILDER_QUOTE, reader->tokenizer, reader->kstate);

                    // open word to identifier mode ( abc to $abc)
                    reader->wordAsIdentifier = true;
    
                }
                else if (event == KN_TOKEN_QUASI) {
                    StateStackPush(reader->stateStack, KN_READER_PARSE_QUASIQUOTE);
                    builder = CreateWrapperBuilder(KN_BUILDER_QUASIQUOTE, reader->tokenizer, reader->kstate);

                    // open word to identifier mode ( abc to $abc)
                    reader->wordAsIdentifier = true;
                }
                else if (event == KN_TOKEN_UNQUOTE_REPLACE
                    || event == KN_TOKEN_UNQUOTE_KV
                    || event == KN_TOKEN_UNQUOTE_SEQ
                ) {
                    StateStackPush(reader->stateStack, KN_READER_PARSE_UNQUOTE);
                    builder = CreateWrapperBuilder(KN_BUILDER_UNQUOTE, reader->tokenizer, reader->kstate);
                }
                else if (event == KN_TOKEN_PREFIX_WRAPPER) {
                    StateStackPush(reader->stateStack, KN_READER_PARSE_PREFIX_WRAPPER);
                    builder = CreateWrapperBuilder(KN_BUILDER_PREFIX, reader->tokenizer, reader->kstate);
                }
                else if (event == KN_TOKEN_SUFFIX_WRAPPER) {
                    StateStackPush(reader->stateStack, KN_READER_PARSE_SUFFIX_WRAPPER);
                    builder = CreateWrapperBuilder(KN_BUILDER_SUFFIX, reader->tokenizer, reader->kstate);
                }
                else if (event == KN_TOKEN_TXT_MARCRO) {
                    StateStackPush(reader->stateStack, KN_READER_PARSE_TXT_MARCRO);
                    builder = CreateWrapperBuilder(KN_BUILDER_TXT_MARCRO, reader->tokenizer, reader->kstate);
                }
                else if (event == KN_TOKEN_OBJ_BUILDER) {
                    StateStackPush(reader->stateStack, KN_READER_PARSE_OBJ_BUILDER);
                    builder = CreateWrapperBuilder(KN_BUILDER_OBJ_BUILDER, reader->tokenizer, reader->kstate);
                }
            }
            BuilderStackPush(reader->builderStack, builder);
            continue;
        }

        else if (event == KN_TOKEN_KV_PAIR_TAG) {
            if (currentState == KN_READER_PARSE_TABLE
                || currentState == KN_READER_PARSE_PARAM
            ) {
                StateStackPush(reader->stateStack, KN_READER_PARSE_TABLE_PAIR_KEY);
                KonBuilder* builder = CreateKvPairBuilder();
                BuilderStackPush(reader->builderStack, builder);
            }
            // #{:}
            else if (currentState == KN_READER_PARSE_MAP_PAIR_KEY) {
                KonBuilder* builder = CreateKvPairBuilder();
                BuilderStackPush(reader->builderStack, builder);
            }
            else if (currentState == KN_READER_PARSE_CELL_CORE
                || currentState == KN_READER_PARSE_CELL_MAP_PAIR
            ) {
                StateStackPush(reader->stateStack, KN_READER_PARSE_MAP_PAIR_KEY);
                KonBuilder* builder = CreateKvPairBuilder();
                BuilderStackPush(reader->builderStack, builder);
            }
            // (: : )
            else if (currentState == KN_READER_PARSE_TABLE_PAIR_KEY) {
                // ignore do nothing
            }
            // (:a = :b 1) bad case, at the ':' before b
            else if (currentState == KN_READER_PARSE_TABLE_PAIR_EQ) {
                // TODO throw exception
            }
            // (:a :b 1) bad case, at the ':' before b
            else if (currentState == KN_READER_PARSE_TABLE_PAIR_VAL) {
                // TODO throw exception
            }
            // {a :b :c = :d} current at the ':' before c and d
            // set pair value to true, and add to top val
            else if (currentState == KN_READER_PARSE_MAP_PAIR_EQ_OR_TAG
                || currentState == KN_READER_PARSE_MAP_PAIR_VAL
            ) {
                AddValueToTopBuilder(reader, KN_UKN);
                StateStackPush(reader->stateStack, KN_READER_PARSE_MAP_PAIR_KEY);
                KonBuilder* builder = CreateKvPairBuilder();
                BuilderStackPush(reader->builderStack, builder);
            }
            // {:a}
            else {
            }
            continue;
        }

        // = parse cell map key values
        else if (event == KN_TOKEN_EQUAL) {
            if (currentState == KN_READER_PARSE_MAP_PAIR_EQ_OR_TAG) {
                StateStackSetTopValue(
                    reader->stateStack,
                    KN_READER_PARSE_MAP_PAIR_VAL
                );
                continue;
            }
            else if (currentState == KN_READER_PARSE_TABLE_PAIR_EQ) {
                StateStackSetTopValue(
                    reader->stateStack,
                    KN_READER_PARSE_TABLE_PAIR_VAL
                );
                continue;
            }
         }
        else if (event == KN_TOKEN_EQUAL
            && currentState == KN_READER_PARSE_MAP_PAIR_EQ_OR_TAG
        ) {
            StateStackSetTopValue(
                reader->stateStack,
                KN_READER_PARSE_MAP_PAIR_VAL
            );
            continue;
        }

        // don't need to create new builder
        KonBuilder* topBuilder = BuilderStackTop(reader->builderStack);
        if (IsSyntaxToken(event)) {
            // syntax markers like: % . | ;
            // don't need update state
            KN marker = MakeSyntaxMarker(reader->kstate, event);
            AddValueToTopBuilder(reader, marker);
        }
        else if (event == KN_TOKEN_SYM_MARCRO) {
            // prefix marcro eg abc!
            // top builder should be a list
            // don't need update state
            KN symbol = MakeSymbol(reader, event);
            AddValueToTopBuilder(reader, symbol);
        }
        else if (event == KN_TOKEN_SYM_CELL_SEG_END) {
            // TODO
        }
        else if (IsContainerEndToken(event)) {
            // an exceptional case: treat a '>' as a symbol
            // if not in a vector builder
            if (event == KN_TOKEN_VECTOR_END
                && topBuilder->type != KN_BUILDER_VECTOR) {
                KN symbol = MakeSymbol(reader, KN_TOKEN_SYM_WORD);
                AddValueToTopBuilder(reader, symbol);
            }
            // the state after parsed kv pair key
            else if (event == KN_TOKEN_CELL_END) {
                if (currentState == KN_READER_PARSE_MAP_PAIR_EQ_OR_TAG
                    || currentState == KN_READER_PARSE_MAP_PAIR_VAL
                ) {
                    AddValueToTopBuilder(reader, KN_UKN);
                }
                ExitTopBuilder(reader);
            }
            else {
                ExitTopBuilder(reader);
            }
            
            continue;
        }
        
        else if (event == KN_TOKEN_SYM_WORD
            || event == KN_TOKEN_SYM_VARIABLE
            || event == KN_TOKEN_SYM_IDENTIFIER
            || event == KN_TOKEN_SYM_STRING
        ) {
            KN symbol = MakeSymbol(reader, event);
            AddValueToTopBuilder(reader, symbol);
        }
        else if (event == KN_TOKEN_SYM_VARIABLE) {
            KN value = MakeSymbol(reader, event);
            AddValueToTopBuilder(reader, value);
        }
        else if (IsLiteralToken(event)) {
            KN value = MakeLiteral(reader, event);
            AddValueToTopBuilder(reader, value);
        }
    }
    KSON_TokenToString(reader->tokenizer);  // should be EOF
    // TODO elegant exit
    if (StateStackTop(reader->stateStack) != KN_READER_ROOT) {
//        ExitAllStackBuilders(reader);
    }
    return reader->root;
}

void KSON_ReaderCloseStream(KonReader* reader)
{
    KSON_TokenizerEnd(reader->tokenizer);

    KxStringBuffer_Clear(reader->readStream);
}

void KSON_ReaderExit(KonReader* reader)
{
    BuilderStackDestroy(reader->builderStack);
    StateStackDestroy(reader->stateStack);
    KSON_TokenizerExit(reader->tokenizer);
}
