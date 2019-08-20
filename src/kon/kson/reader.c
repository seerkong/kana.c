#include "reader.h"

void ExitTopBuilder(KonReader* reader);
void AddValueToTopBuilder(KonReader* reader, KN value);


KonReader* KSON_ReaderInit(KonState* kstate)
{
    // init reader
    KonReader* reader = (KonReader*)tb_malloc(sizeof(KonReader));
    KON_DEBUG("malloc reader %x", reader);

    reader->Kstate = kstate;
    
    
    reader->Tokenizer = KSON_TokenizerInit(kstate);
    KON_DEBUG("after KSON_TokenizerInit");
    
    reader->BuilderStack = BuilderStackInit();
    reader->StateStack = StateStackInit();
    reader->WordAsIdentifier = false;

    return reader;
}

bool KSON_ReaderFromFile(KonReader* reader, const char* sourceFilePath)
{
    KxStringBuffer* sb = KON_ReadFileContent(sourceFilePath);
    // prepend '[' at head, append ']' at tail
    KxStringBuffer_NPrependCstr(sb, "[\n", 1);
    KxStringBuffer_AppendCstr(sb, "\n]");
    bool res = KSON_ReaderFromCstr(reader, KxStringBuffer_Cstr(sb));
    KxStringBuffer_Clear(sb);
    return res;
}

bool KSON_ReaderFromCstr(KonReader* reader, const char* sourceCstr)
{
    reader->ReadStream = KxStringBuffer_New();
    KxStringBuffer_AppendCstr(reader->ReadStream, sourceCstr);
    KSON_TokenizerBegin(reader->Tokenizer, reader->ReadStream);
    return true;
}

bool IsSkipToken(int event)
{
    if (event == KON_TOKEN_NONE
        || event == KON_TOKEN_WHITESPACE
        || event == KON_TOKEN_COMMENT_SINGLE_LINE
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
    if (event == KON_TOKEN_LIST_START
        || event == KON_TOKEN_VECTOR_START
        || event == KON_TOKEN_TABLE_START
        || event == KON_TOKEN_PARAM_START
        || event == KON_TOKEN_BLOCK_START
        || event == KON_TOKEN_CELL_START
    ) {
        return true;
    }
    else {
        return false;
    }
}

bool IsContainerEndToken(int event)
{
    if (event == KON_TOKEN_LIST_END
        || event == KON_TOKEN_VECTOR_END
        || event == KON_TOKEN_TABLE_END
        || event == KON_TOKEN_CELL_END
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
bool IsWrapperToken(int event)
{
    if (event == KON_TOKEN_QUOTE_LIST
        || event == KON_TOKEN_QUOTE_CELL
        || event == KON_TOKEN_QUASI_LIST
        || event == KON_TOKEN_QUASI_CELL
        || event == KON_TOKEN_EXPAND_REPLACE
        || event == KON_TOKEN_EXPAND_KV
        || event == KON_TOKEN_EXPAND_SEQ
        || event == KON_TOKEN_UNQUOTE_REPLACE
        || event == KON_TOKEN_UNQUOTE_KV
        || event == KON_TOKEN_UNQUOTE_SEQ
    ) {
        return true;
    }
    else {
        return false;
    }
}

bool IsLiteralToken(int event)
{
    if (event == KON_TOKEN_KEYWORD_NIL
        || event == KON_TOKEN_KEYWORD_UNDEF
        || event == KON_TOKEN_KEYWORD_UKN
        || event == KON_TOKEN_KEYWORD_TRUE
        || event == KON_TOKEN_KEYWORD_FALSE
        || event == KON_TOKEN_KEYWORD_EITHER
        || event == KON_TOKEN_KEYWORD_BOTH
        || event == KON_TOKEN_LITERAL_NUMBER
        || event == KON_TOKEN_LITERAL_STRING
        || event == KON_TOKEN_LITERAL_RAW_STRING
    ) {
        return true;
    }
    else {
        return false;
    }
}

// % . | ;
bool IsSyntaxToken(int event)
{
    if (event == KON_TOKEN_APPLY
        || event == KON_TOKEN_EQUAL
        || event == KON_TOKEN_PROC_PIPE
        || event == KON_TOKEN_MSG_SIGNAL
        || event == KON_TOKEN_GET_SLOT
        || event == KON_TOKEN_CLAUSE_END
    ) {
        return true;
    }
    else {
        return false;
    }
}



KN MakeSyntaxMarker(KonState* kstate, KonTokenKind tokenKind)
{
    KonSyntaxMarker* value = KON_ALLOC_TYPE_TAG(kstate, KonSyntaxMarker, KON_T_SYNTAX_MARKER);
    switch (tokenKind) {
        case KON_TOKEN_APPLY: {
            value->Type = KON_SYNTAX_MARKER_APPLY;
            break;
        }
        case KON_TOKEN_EQUAL: {
            value->Type = KON_SYNTAX_MARKER_EQUAL;
            break;
        }
        case KON_TOKEN_CLAUSE_END: {
            value->Type = KON_SYNTAX_MARKER_CLAUSE_END;
            break;
        }
        case KON_TOKEN_PROC_PIPE: {
            value->Type = KON_SYNTAX_MARKER_PROC_PIPE;
            break;
        }
        case KON_TOKEN_MSG_SIGNAL: {
            value->Type = KON_SYNTAX_MARKER_MSG_SIGNAL;
            break;
        }
        case KON_TOKEN_GET_SLOT: {
            value->Type = KON_SYNTAX_MARKER_GET_SLOT;
            break;
        }
        default: {
            break;
        }
    }
    return value;
}

KN MakeSymbol(KonReader* reader, KonTokenKind event)
{
    KonSymbol* value = KON_ALLOC_TYPE_TAG(reader->Kstate, KonSymbol, KON_T_SYMBOL);
    if (event == KON_TOKEN_SYM_PREFIX_WORD) {
        value->Type = KON_SYM_PREFIX_WORD;
    }
    else if (event == KON_TOKEN_SYM_SUFFIX_WORD) {
        value->Type = KON_SYM_SUFFIX_WORD;
    }
    
    else if (event == KON_TOKEN_SYM_WORD) {
        // if in quote or quasiquote scope, convert word to symbol
        // if (reader->WordAsIdentifier) {
        //     value->Type = KON_SYM_IDENTIFIER;
        // }
        // else {
            value->Type = KON_SYM_WORD;
        // }
    }
    else if (event == KON_TOKEN_SYM_VARIABLE) {
        value->Type = KON_SYM_VARIABLE;
    }
    else if (event == KON_TOKEN_SYM_IDENTIFIER) {
        value->Type = KON_SYM_IDENTIFIER;
    }
    else if (event == KON_TOKEN_SYM_STRING) {
        value->Type = KON_SYM_STRING;
    }


    
    value->Data = utf8dup(KxStringBuffer_Cstr(reader->Tokenizer->Content));

    return value;
}

// num token to kon number
KN MakeNumber(KonReader* reader)
{
    bool isPositive = reader->Tokenizer->NumIsPositive;
    const char* numStrBeforeDot = KxStringBuffer_Cstr(reader->Tokenizer->NumBeforeDot);
    const char* numStrAfterDot = KxStringBuffer_Cstr(reader->Tokenizer->NumAfterDot);
    const char* numStrAfterPower = KxStringBuffer_Cstr(reader->Tokenizer->NumAfterPower);
    
    KN value = KON_ZERO;
    
    if (isPositive
        && (numStrAfterDot == NULL || strcmp(numStrAfterDot, "") == 0)
        && (numStrAfterPower == NULL || strcmp(numStrAfterPower, "") == 0)
    ) {
        // TODO add judgment, if out of range
        // 32 bit system : use 4*8-1 bits;
        // 64 bit system : use 8*8-1 bits;
        int numBeforeDot = atoi(numStrBeforeDot);
        value = KON_MAKE_FIXNUM(numBeforeDot);
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
        value = KON_MakeFlonum(reader->Kstate, num);
    }
    return value;
}

KN MakeString(KonReader* reader)
{
    KN value = KON_MakeString(reader->Kstate, KxStringBuffer_Cstr(reader->Tokenizer->Content));
    return value;
}

KN MakeLiteral(KonReader* reader, KonTokenKind event)
{
    KN value = KON_UKN;

    if (event == KON_TOKEN_KEYWORD_NIL) {
        value = KON_NIL;
    }
    else if (event == KON_TOKEN_KEYWORD_UNDEF) {
        value = KON_UNDEF;
    }
    else if (event == KON_TOKEN_KEYWORD_UKN) {
        value = KON_UKN;
    }
    else if (event == KON_TOKEN_KEYWORD_TRUE) {
        value = KON_TRUE;
    }
    else if (event == KON_TOKEN_KEYWORD_FALSE) {
        value = KON_FALSE;
    }
    else if (event == KON_TOKEN_KEYWORD_EITHER) {
        // TODO
    }
    else if (event == KON_TOKEN_KEYWORD_BOTH) {
        // TODO
    }
    else if (event == KON_TOKEN_LITERAL_NUMBER) {
        value = MakeNumber(reader);
    }
    else if (event == KON_TOKEN_LITERAL_STRING) {
        value = MakeString(reader);
    }
    else if (event == KON_TOKEN_LITERAL_RAW_STRING) {
        value = MakeString(reader);
    }
    
    return value;
}

void AddValueToTopBuilder(KonReader* reader, KN value)
{
    KonReaderState currentState = StateStackTop(reader->StateStack);
    KonReaderState newState = currentState;

    if (BuilderStackLength(reader->BuilderStack) == 0) {
        // parse finish
        reader->Root = value;
        return;
    }
    KonBuilder* topBuilder = BuilderStackTop(reader->BuilderStack);
    
    
    KonBuilderType builderType = topBuilder->Type;
    // KON_DEBUG("AddValueToTopBuilder builder type %d", (int)builderType);
    if (builderType == KON_BUILDER_VECTOR) {
        VectorBuilderAddItem(topBuilder, value);
    }
    else if (builderType == KON_BUILDER_LIST) {
        ListBuilderAddItem(topBuilder, value);
    }
    else if (builderType == KON_BUILDER_TABLE) {
        TableBuilderAddValue(topBuilder, value);
    }
    else if (builderType == KON_BUILDER_BLOCK) {
        BlockBuilderAddItem(topBuilder, value);
    }
    else if (builderType == KON_BUILDER_PARAM) {
        ParamBuilderAddValue(topBuilder, value);
    }
    else if (builderType == KON_BUILDER_KV_PAIR) {
        if (currentState == KON_READER_PARSE_TABLE_PAIR_KEY) {
            assert(KON_IS_SYMBOL(value));
            KonSymbolType symbolType = KON_FIELD(value, KonSymbol, Type);
            assert(symbolType != KON_SYM_VARIABLE && symbolType != KON_SYM_PREFIX_WORD);
            // table tag key should not be NULL
            char* tableKey = KON_UNBOX_SYMBOL(value);
            assert(tableKey);
            KvPairSetKey(topBuilder, tableKey);

            StateStackSetTopValue(
                reader->StateStack,
                KON_READER_PARSE_TABLE_PAIR_VAL
            );
        }
        else if (currentState == KON_READER_PARSE_MAP_PAIR_KEY) {
            assert(KON_IS_SYMBOL(value));
            KonSymbolType symbolType = KON_FIELD(value, KonSymbol, Type);
            assert(symbolType != KON_SYM_VARIABLE && symbolType != KON_SYM_PREFIX_WORD);
            // table tag key should not be NULL
            char* tableKey = KON_UNBOX_SYMBOL(value);
            assert(tableKey);
            KvPairSetKey(topBuilder, tableKey);

            StateStackSetTopValue(
                reader->StateStack,
                KON_READER_PARSE_MAP_PAIR_VAL_OR_NEXT
            );
        }
        // meet value, end key val pair
        else if (currentState == KON_READER_PARSE_TABLE_PAIR_VAL) {
            KvPairSetValue(topBuilder, value);
            KonBuilder* pairBuilder = BuilderStackPop(reader->BuilderStack);
            KonBuilder* nextBuilder = BuilderStackTop(reader->BuilderStack);
            if (nextBuilder->Type == KON_BUILDER_TABLE) {
                TableBuilderAddPair(nextBuilder, pairBuilder);
                StateStackPop(reader->StateStack);
            }
            else if (nextBuilder->Type == KON_BUILDER_PARAM) {
                ParamBuilderAddPair(nextBuilder, pairBuilder);
                StateStackPop(reader->StateStack);
            }
            
        }
        else if (currentState == KON_READER_PARSE_MAP_PAIR_VAL_OR_NEXT) {
            KvPairSetValue(topBuilder, value);
            KonBuilder* pairBuilder = BuilderStackPop(reader->BuilderStack);
            KonBuilder* mapBuilder = BuilderStackTop(reader->BuilderStack);

            CellBuilderAddPair(mapBuilder, pairBuilder);
            StateStackPop(reader->StateStack);

            StateStackSetTopValue(
                reader->StateStack,
                KON_READER_PARSE_CELL_MAP
            );
        }
        else {
            // TODO exception
        }
    }
    else if (builderType == KON_BUILDER_CELL) {
        // TODO if cell next section
        // 1 core is set, meet next core
        // 2 table is set, meet next core or table
        // 3 list is set, meet next core or table or list
        if (KON_IS_VECTOR(value)) {
            CellBuilderSetVector(topBuilder, value);
        }
        else if (KON_IS_PARAM(value)) {
            CellBuilderSetTable(topBuilder, value);
        }
        else if (KON_IsBlock(value)) {
            CellBuilderSetList(topBuilder, value);
        }
        else {
            CellBuilderSetCore(topBuilder, value);
        }
        
    }
    // when in wrapper builders, should exit this builder
    // after set wrapper inner value
    else if (builderType == KON_BUILDER_QUOTE
        || builderType == KON_BUILDER_QUASIQUOTE
        || builderType == KON_BUILDER_EXPAND
        || builderType == KON_BUILDER_UNQUOTE
    ) {
        WrapperSetInner(reader->Kstate, topBuilder, value);
        ExitTopBuilder(reader);
    }
}

void ExitTopBuilder(KonReader* reader)
{
    
    KonReaderState currentState = StateStackTop(reader->StateStack);
    KonReaderState newState = currentState;

    KonBuilder* topBuilder = BuilderStackTop(reader->BuilderStack);
    KonBuilderType builderType = topBuilder->Type;

    KON_DEBUG("ExitTopBuilder builder type %s", BuilderTypeToCStr(builderType));

    KN value;
    if (builderType == KON_BUILDER_VECTOR) {
        value = MakeVectorByBuilder(reader->Kstate, topBuilder);
    }
    else if (builderType == KON_BUILDER_LIST) {
        value = MakeListByBuilder(reader->Kstate, topBuilder);
    }
    else if (builderType == KON_BUILDER_BLOCK) {
        value = MakeBlockByBuilder(reader->Kstate, topBuilder);
    }
    else if (builderType == KON_BUILDER_PARAM) {
        value = MakeParamByBuilder(reader->Kstate, topBuilder);
    }
    else if (builderType == KON_BUILDER_TABLE) {
        value = MakeTableByBuilder(reader->Kstate, topBuilder);
    }
    else if (builderType == KON_BUILDER_CELL) {
        value = MakeCellByBuilder(reader->Kstate, topBuilder);
    }
    else if (builderType == KON_BUILDER_QUOTE
        || builderType == KON_BUILDER_QUASIQUOTE
        || builderType == KON_BUILDER_EXPAND
        || builderType == KON_BUILDER_UNQUOTE
    ) {
        value = MakeWrapperByBuilder(reader->Kstate, topBuilder);
        if (builderType == KON_BUILDER_QUOTE || builderType == KON_BUILDER_QUASIQUOTE) {
            // close word to identifier mode ( abc to abc)
            reader->WordAsIdentifier = false;
        }
    }

    // resume last container builder
    BuilderStackPop(reader->BuilderStack);
    // resume last state
    StateStackPop(reader->StateStack);

    AddValueToTopBuilder(reader, value);
}

void ExitAllStackBuilders()
{
    KON_DEBUG("ExitAllStackBuilders");
    // KonBuilder* newTopBuilder = BuilderStackTop(reader->BuilderStack);
}

KN KSON_Parse(KonReader* reader)
{
    int initState = KON_READER_ROOT;
    StateStackPush(reader->StateStack, initState);
    KON_DEBUG("start");

    int event = KON_TOKEN_NONE;
    while ((event = KSON_TokenizerNext(reader->Tokenizer)) && event != KON_TOKEN_EOF) {
        KSON_TokenToString(reader->Tokenizer);
        if (IsSkipToken(event)) {
            continue;
        }

        KonReaderState currentState = StateStackTop(reader->StateStack);

        if (IsContainerStartToken(event) || IsWrapperToken(event)) {
            // 1 create builder
            // 2 push new builder to builder stack
            // 3 save current state
            // 3 update top state

            // 如当前是cell类型，修改当前状态后再push到state stack
            if (IsContainerStartToken(event)
                && currentState == KON_READER_PARSE_CELL_CORE
            ) {
                StateStackSetTopValue(reader->StateStack, KON_READER_PARSE_CELL_INNER_CONTAINER);
            }

            KonBuilder* builder;
            if (event == KON_TOKEN_LIST_START) {
                StateStackPush(reader->StateStack, KON_READER_PARSE_LIST);
                builder = CreateListBuilder();
            }
            else if (event == KON_TOKEN_BLOCK_START) {
                StateStackPush(reader->StateStack, KON_READER_PARSE_BLOCK);
                builder = CreateBlockBuilder();
            }
            else if (event == KON_TOKEN_VECTOR_START) {
                StateStackPush(reader->StateStack, KON_READER_PARSE_VECTOR);
                KonBuilder* builder2 = CreateVectorBuilder();
                BuilderStackPush(reader->BuilderStack, builder2);
                continue;
            }
            else if (event == KON_TOKEN_TABLE_START) {
                StateStackPush(reader->StateStack, KON_READER_PARSE_TABLE);
                builder = CreateTableBuilder();
            }
            else if (event == KON_TOKEN_PARAM_START) {
                StateStackPush(reader->StateStack, KON_READER_PARSE_PARAM);
                builder = CreateParamBuilder();
            }
            else if (event == KON_TOKEN_CELL_START) {
                StateStackPush(reader->StateStack, KON_READER_PARSE_CELL_CORE);
                builder = CreateCellBuilder();
            }
            else {
                // wrapper types
                if (event == KON_TOKEN_QUOTE_LIST
                    || event == KON_TOKEN_QUOTE_CELL
                ) {
                    StateStackPush(reader->StateStack, KON_READER_PARSE_QUOTE);
                    builder = CreateWrapperBuilder(KON_BUILDER_QUOTE, event);

                    // open word to identifier mode ( abc to $abc)
                    reader->WordAsIdentifier = true;
    
                }
                else if (event == KON_TOKEN_QUASI_LIST
                    || event == KON_TOKEN_QUASI_CELL
                ) {
                    StateStackPush(reader->StateStack, KON_READER_PARSE_QUASIQUOTE);
                    builder = CreateWrapperBuilder(KON_BUILDER_QUASIQUOTE, event);

                    // open word to identifier mode ( abc to $abc)
                    reader->WordAsIdentifier = true;
                }
                else if (event == KON_TOKEN_UNQUOTE_REPLACE
                    || event == KON_TOKEN_UNQUOTE_KV
                    || event == KON_TOKEN_UNQUOTE_SEQ
                ) {
                    StateStackPush(reader->StateStack, KON_READER_PARSE_UNQUOTE);
                    builder = CreateWrapperBuilder(KON_BUILDER_UNQUOTE, event);
                }
                else if (event == KON_TOKEN_EXPAND_REPLACE) {
                    StateStackPush(reader->StateStack, KON_READER_PARSE_EXPAND_REPLACE);
                    builder = CreateWrapperBuilder(KON_BUILDER_EXPAND, event);
                }
                else if (event == KON_TOKEN_EXPAND_KV) {
                    StateStackPush(reader->StateStack, KON_READER_PARSE_EXPAND_KV);
                    builder = CreateWrapperBuilder(KON_BUILDER_EXPAND, event);
                }
                else if (event == KON_TOKEN_EXPAND_SEQ) {
                    StateStackPush(reader->StateStack, KON_READER_PARSE_EXPAND_SEQ);
                    builder = CreateWrapperBuilder(KON_BUILDER_EXPAND, event);
                }
            }
            BuilderStackPush(reader->BuilderStack, builder);
            continue;
        }

        else if (event == KON_TOKEN_TABLE_TAG) {
            if (currentState == KON_READER_PARSE_TABLE
                || currentState == KON_READER_PARSE_PARAM
            ) {
                StateStackPush(reader->StateStack, KON_READER_PARSE_TABLE_PAIR_KEY);
                KonBuilder* builder = CreateKvPairBuilder();
                BuilderStackPush(reader->BuilderStack, builder);
            }
            else if (currentState == KON_READER_PARSE_CELL_CORE
                || currentState == KON_READER_PARSE_CELL_MAP
            ) {
                StateStackPush(reader->StateStack, KON_READER_PARSE_MAP_PAIR_KEY);
                KonBuilder* builder = CreateKvPairBuilder();
                BuilderStackPush(reader->BuilderStack, builder);
            }
            // (: : )
            else if (currentState == KON_READER_PARSE_TABLE_PAIR_KEY
                || currentState == KON_READER_PARSE_MAP_PAIR_KEY
            ) {
                // ignore do nothing
            }
            // (:a :b 1) bad case, like the ':' before b
            else if (currentState == KON_READER_PARSE_TABLE_PAIR_VAL) {
                // TODO throw exception
            }
            // {a :b :c} current like the ':' before c
            // set pair value to true, and add to top val
            else if (currentState == KON_READER_PARSE_MAP_PAIR_VAL_OR_NEXT) {
                AddValueToTopBuilder(reader, KON_TRUE);
                StateStackPush(reader->StateStack, KON_READER_PARSE_MAP_PAIR_KEY);
                KonBuilder* builder = CreateKvPairBuilder();
                BuilderStackPush(reader->BuilderStack, builder);
            }
            // {:a}
            else {
            }
            continue;
        }

        // don't need to create new builder
        KonBuilder* topBuilder = BuilderStackTop(reader->BuilderStack);
        if (IsSyntaxToken(event)) {
            // syntax markers like: % . | ; 
            // top builder should be a list
            // don't need update state
            assert(topBuilder && topBuilder->Type == KON_BUILDER_LIST);
            KN marker = MakeSyntaxMarker(reader->Kstate, event);
            AddValueToTopBuilder(reader, marker);
        }
        else if (event == KON_TOKEN_SYM_PREFIX_WORD) {
            // prefix marcro eg !abc
            // top builder should be a list
            // don't need update state
            assert(topBuilder && topBuilder->Type == KON_BUILDER_LIST);
            KN symbol = MakeSymbol(reader, event);
            AddValueToTopBuilder(reader, symbol);
        }
        else if (event == KON_TOKEN_SYM_SUFFIX_WORD) {
            // suffix marcro eg ^abc
            // TODO
        }
        else if (IsContainerEndToken(event)) {
            // an exceptional case: treat a '>' as a symbol
            // if not in a vector builder
            if (event == KON_TOKEN_VECTOR_END
                && topBuilder->Type != KON_BUILDER_VECTOR) {
                KN symbol = MakeSymbol(reader, KON_TOKEN_SYM_WORD);
                AddValueToTopBuilder(reader, symbol);
            }
            else {
                ExitTopBuilder(reader);
            }
            
            continue;
        }
        
        else if (event == KON_TOKEN_SYM_WORD
            || event == KON_TOKEN_SYM_VARIABLE
            || event == KON_TOKEN_SYM_IDENTIFIER
            || event == KON_TOKEN_SYM_STRING
        ) {
            KN symbol = MakeSymbol(reader, event);
            AddValueToTopBuilder(reader, symbol);
        }
        else if (event == KON_TOKEN_SYM_VARIABLE) {
            KN value = MakeSymbol(reader, event);
            AddValueToTopBuilder(reader, value);
        }
        else if (IsLiteralToken(event)) {
            KN value = MakeLiteral(reader, event);
            AddValueToTopBuilder(reader, value);
        }
    }
    KSON_TokenToString(reader->Tokenizer);  // should be EOF
    // TODO elegant exit
    if (StateStackTop(reader->StateStack) != KON_READER_ROOT) {
//        ExitAllStackBuilders(reader);
    }
    return reader->Root;
}

void KSON_ReaderCloseStream(KonReader* reader)
{
    KSON_TokenizerEnd(reader->Tokenizer);

    KxStringBuffer_Clear(reader->ReadStream);
}

void KSON_ReaderExit(KonReader* reader)
{
    BuilderStackDestroy(reader->BuilderStack);
    StateStackDestroy(reader->StateStack);
    KSON_TokenizerExit(reader->Tokenizer);
}
