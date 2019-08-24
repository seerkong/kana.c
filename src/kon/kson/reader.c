#include "reader.h"

void ExitTopBuilder(KonReader* reader);
void AddValueToTopBuilder(KonReader* reader, KN value);


KonReader* KSON_ReaderInit(KonState* kstate)
{
    // init reader
    KonReader* reader = (KonReader*)tb_malloc(sizeof(KonReader));
    KN_DEBUG("malloc reader %x", reader);

    reader->Kstate = kstate;
    
    
    reader->Tokenizer = KSON_TokenizerInit(kstate);
    KN_DEBUG("after KSON_TokenizerInit");
    
    reader->BuilderStack = BuilderStackInit();
    reader->StateStack = StateStackInit();
    reader->WordAsIdentifier = false;

    return reader;
}

bool KSON_ReaderFromFile(KonReader* reader, const char* sourceFilePath)
{
    KxStringBuffer* sb = KN_ReadFileContent(sourceFilePath);
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
        || event == KN_TOKEN_PARAM_START
        || event == KN_TOKEN_BLOCK_START
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
bool IsWrapperToken(int event)
{
    if (event == KN_TOKEN_QUOTE_LIST
        || event == KN_TOKEN_QUOTE_CELL
        || event == KN_TOKEN_QUASI_LIST
        || event == KN_TOKEN_QUASI_CELL
        || event == KN_TOKEN_EXPAND_REPLACE
        || event == KN_TOKEN_EXPAND_KV
        || event == KN_TOKEN_EXPAND_SEQ
        || event == KN_TOKEN_UNQUOTE_REPLACE
        || event == KN_TOKEN_UNQUOTE_KV
        || event == KN_TOKEN_UNQUOTE_SEQ
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
        || event == KN_TOKEN_KEYWORD_EITHER
        || event == KN_TOKEN_KEYWORD_BOTH
        || event == KN_TOKEN_LITERAL_NUMBER
        || event == KN_TOKEN_LITERAL_STRING
        || event == KN_TOKEN_LITERAL_RAW_STRING
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
        || event == KN_TOKEN_GET_SLOT
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
    KonSyntaxMarker* value = KN_ALLOC_TYPE_TAG(kstate, KonSyntaxMarker, KN_T_SYNTAX_MARKER);
    switch (tokenKind) {
        case KN_TOKEN_APPLY: {
            value->Type = KN_SYNTAX_MARKER_APPLY;
            break;
        }
        case KN_TOKEN_EQUAL: {
            value->Type = KN_SYNTAX_MARKER_EQUAL;
            break;
        }
        case KN_TOKEN_ASSIGN: {
            value->Type = KN_SYNTAX_MARKER_ASSIGN;
            break;
        }
        case KN_TOKEN_CLAUSE_END: {
            value->Type = KN_SYNTAX_MARKER_CLAUSE_END;
            break;
        }
        case KN_TOKEN_PROC_PIPE: {
            value->Type = KN_SYNTAX_MARKER_PROC_PIPE;
            break;
        }
        case KN_TOKEN_MSG_SIGNAL: {
            value->Type = KN_SYNTAX_MARKER_MSG_SIGNAL;
            break;
        }
        case KN_TOKEN_GET_SLOT: {
            value->Type = KN_SYNTAX_MARKER_GET_SLOT;
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
    KonSymbol* value = KN_ALLOC_TYPE_TAG(reader->Kstate, KonSymbol, KN_T_SYMBOL);
    if (event == KN_TOKEN_SYM_PREFIX_WORD) {
        value->Type = KN_SYM_PREFIX_WORD;
    }
    else if (event == KN_TOKEN_SYM_SUFFIX_WORD) {
        value->Type = KN_SYM_SUFFIX_WORD;
    }
    
    else if (event == KN_TOKEN_SYM_WORD) {
        // if in quote or quasiquote scope, convert word to symbol
        // if (reader->WordAsIdentifier) {
        //     value->Type = KN_SYM_IDENTIFIER;
        // }
        // else {
            value->Type = KN_SYM_WORD;
        // }
    }
    else if (event == KN_TOKEN_SYM_VARIABLE) {
        value->Type = KN_SYM_VARIABLE;
    }
    else if (event == KN_TOKEN_SYM_IDENTIFIER) {
        value->Type = KN_SYM_IDENTIFIER;
    }
    else if (event == KN_TOKEN_SYM_STRING) {
        value->Type = KN_SYM_STRING;
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
        value = KN_MakeFlonum(reader->Kstate, num);
    }
    return value;
}

KN MakeString(KonReader* reader)
{
    KN value = KN_MakeString(reader->Kstate, KxStringBuffer_Cstr(reader->Tokenizer->Content));
    return value;
}

KN MakeLiteral(KonReader* reader, KonTokenKind event)
{
    KN value = KN_UKN;

    if (event == KN_TOKEN_KEYWORD_NIL) {
        value = KN_NIL;
    }
    else if (event == KN_TOKEN_KEYWORD_UNDEF) {
        value = KN_UNDEF;
    }
    else if (event == KN_TOKEN_KEYWORD_UKN) {
        value = KN_UKN;
    }
    else if (event == KN_TOKEN_KEYWORD_TRUE) {
        value = KN_TRUE;
    }
    else if (event == KN_TOKEN_KEYWORD_FALSE) {
        value = KN_FALSE;
    }
    else if (event == KN_TOKEN_KEYWORD_EITHER) {
        // TODO
    }
    else if (event == KN_TOKEN_KEYWORD_BOTH) {
        // TODO
    }
    else if (event == KN_TOKEN_LITERAL_NUMBER) {
        value = MakeNumber(reader);
    }
    else if (event == KN_TOKEN_LITERAL_STRING) {
        value = MakeString(reader);
    }
    else if (event == KN_TOKEN_LITERAL_RAW_STRING) {
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
    // KN_DEBUG("AddValueToTopBuilder builder type %d", (int)builderType);
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
            KonSymbolType symbolType = KN_FIELD(value, KonSymbol, Type);
            assert(symbolType != KN_SYM_VARIABLE && symbolType != KN_SYM_PREFIX_WORD);
            // table tag key should not be NULL
            char* tableKey = KN_UNBOX_SYMBOL(value);
            assert(tableKey);
            KvPairSetKey(topBuilder, tableKey);

            StateStackSetTopValue(
                reader->StateStack,
                KN_READER_PARSE_TABLE_PAIR_EQ
            );
        }
        else if (currentState == KN_READER_PARSE_MAP_PAIR_KEY) {
            assert(KN_IS_SYMBOL(value));
            KonSymbolType symbolType = KN_FIELD(value, KonSymbol, Type);
            assert(symbolType != KN_SYM_VARIABLE && symbolType != KN_SYM_PREFIX_WORD);
            // table tag key should not be NULL
            char* tableKey = KN_UNBOX_SYMBOL(value);
            assert(tableKey);
            KvPairSetKey(topBuilder, tableKey);

            StateStackSetTopValue(
                reader->StateStack,
                KN_READER_PARSE_MAP_PAIR_EQ_OR_TAG
            );
        }
        // meet value, end key val pair (:a 1) (:a = 1)
        else if (currentState == KN_READER_PARSE_TABLE_PAIR_EQ
            || currentState == KN_READER_PARSE_TABLE_PAIR_VAL
        ) {
            KvPairSetValue(topBuilder, value);
            KonBuilder* pairBuilder = BuilderStackPop(reader->BuilderStack);
            KonBuilder* nextBuilder = BuilderStackTop(reader->BuilderStack);
            if (nextBuilder->Type == KN_BUILDER_TABLE) {
                TableBuilderAddPair(nextBuilder, pairBuilder);
                StateStackPop(reader->StateStack);
            }
            else if (nextBuilder->Type == KN_BUILDER_PARAM) {
                ParamBuilderAddPair(nextBuilder, pairBuilder);
                StateStackPop(reader->StateStack);
            }
            
        }
        else if (currentState == KN_READER_PARSE_MAP_PAIR_VAL || currentState == KN_READER_PARSE_MAP_PAIR_EQ_OR_TAG) {
            KvPairSetValue(topBuilder, value);
            KonBuilder* pairBuilder = BuilderStackPop(reader->BuilderStack);
            KonBuilder* mapBuilder = BuilderStackTop(reader->BuilderStack);

            CellBuilderAddPair(mapBuilder, pairBuilder);
            StateStackPop(reader->StateStack);

            StateStackSetTopValue(
                reader->StateStack,
                KN_READER_PARSE_CELL_MAP
            );
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
        if (KN_IS_VECTOR(value)) {
            CellBuilderSetVector(topBuilder, value);
        }
        else if (KN_IS_PARAM(value)) {
            CellBuilderSetTable(topBuilder, value);
        }
        else if (KN_IS_BLOCK(value)) {
            CellBuilderSetList(topBuilder, value);
        }
        // #nil; a core value
        else if (value == KN_NIL) {
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
        || builderType == KN_BUILDER_EXPAND
        || builderType == KN_BUILDER_UNQUOTE
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

    KN_DEBUG("ExitTopBuilder builder type %s", BuilderTypeToCStr(builderType));

    KN value;
    if (builderType == KN_BUILDER_VECTOR) {
        value = MakeVectorByBuilder(reader->Kstate, topBuilder);
    }
    else if (builderType == KN_BUILDER_LIST) {
        value = MakeListByBuilder(reader->Kstate, topBuilder);
    }
    else if (builderType == KN_BUILDER_BLOCK) {
        value = MakeBlockByBuilder(reader->Kstate, topBuilder);

        // a exceptional case, when in a cell, {abc #[]} set #[] to cell list
        // and {abc #nil;} set #nil; to 2nd cell core
        KonBuilder* nextTopBuilder = (KonBuilder*)reader->BuilderStack->Top->Next->Data;
        if (nextTopBuilder->Type == KN_BUILDER_CELL) {
            // resume last container builder
            BuilderStackPop(reader->BuilderStack);
            // resume last state
            StateStackPop(reader->StateStack);
            CellBuilderSetList(nextTopBuilder, value);

            return;
        }

    }
    else if (builderType == KN_BUILDER_PARAM) {
        value = MakeParamByBuilder(reader->Kstate, topBuilder);
    }
    else if (builderType == KN_BUILDER_TABLE) {
        value = MakeTableByBuilder(reader->Kstate, topBuilder);
    }
    else if (builderType == KN_BUILDER_CELL) {
        value = MakeCellByBuilder(reader->Kstate, topBuilder);
    }
    else if (builderType == KN_BUILDER_QUOTE
        || builderType == KN_BUILDER_QUASIQUOTE
        || builderType == KN_BUILDER_EXPAND
        || builderType == KN_BUILDER_UNQUOTE
    ) {
        value = MakeWrapperByBuilder(reader->Kstate, topBuilder);
        if (builderType == KN_BUILDER_QUOTE || builderType == KN_BUILDER_QUASIQUOTE) {
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
    KN_DEBUG("ExitAllStackBuilders");
    // KonBuilder* newTopBuilder = BuilderStackTop(reader->BuilderStack);
}

KN KSON_Parse(KonReader* reader)
{
    int initState = KN_READER_ROOT;
    StateStackPush(reader->StateStack, initState);
    KN_DEBUG("start");

    int event = KN_TOKEN_NONE;
    while ((event = KSON_TokenizerNext(reader->Tokenizer)) && event != KN_TOKEN_EOF) {
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

            // modify cell state, before create new builder
            if (currentState == KN_READER_PARSE_CELL_CORE
            ) {
                StateStackSetTopValue(reader->StateStack, KN_READER_PARSE_CELL_INNER_CONTAINER);
            }
            // {a :b ()} 
            else if (currentState == KN_READER_PARSE_MAP_PAIR_EQ_OR_TAG
            ) {
                AddValueToTopBuilder(reader, KN_UKN);
                // StateStackPop(reader->StateStack);
                // StateStackSetTopValue(reader->StateStack, KN_READER_PARSE_CELL_INNER_CONTAINER);
            }

            KonBuilder* builder;
            if (event == KN_TOKEN_LIST_START) {
                StateStackPush(reader->StateStack, KN_READER_PARSE_LIST);
                builder = CreateListBuilder();
            }
            else if (event == KN_TOKEN_BLOCK_START) {
                StateStackPush(reader->StateStack, KN_READER_PARSE_BLOCK);
                builder = CreateBlockBuilder();
            }
            else if (event == KN_TOKEN_VECTOR_START) {
                StateStackPush(reader->StateStack, KN_READER_PARSE_VECTOR);
                KonBuilder* builder2 = CreateVectorBuilder();
                BuilderStackPush(reader->BuilderStack, builder2);
                continue;
            }
            else if (event == KN_TOKEN_TABLE_START) {
                StateStackPush(reader->StateStack, KN_READER_PARSE_TABLE);
                builder = CreateTableBuilder();
            }
            else if (event == KN_TOKEN_PARAM_START) {
                StateStackPush(reader->StateStack, KN_READER_PARSE_PARAM);
                builder = CreateParamBuilder();
            }
            else if (event == KN_TOKEN_CELL_START) {
                StateStackPush(reader->StateStack, KN_READER_PARSE_CELL_CORE);
                builder = CreateCellBuilder();
            }
            else {
                // wrapper types
                if (event == KN_TOKEN_QUOTE_LIST
                    || event == KN_TOKEN_QUOTE_CELL
                ) {
                    StateStackPush(reader->StateStack, KN_READER_PARSE_QUOTE);
                    builder = CreateWrapperBuilder(KN_BUILDER_QUOTE, event);

                    // open word to identifier mode ( abc to $abc)
                    reader->WordAsIdentifier = true;
    
                }
                else if (event == KN_TOKEN_QUASI_LIST
                    || event == KN_TOKEN_QUASI_CELL
                ) {
                    StateStackPush(reader->StateStack, KN_READER_PARSE_QUASIQUOTE);
                    builder = CreateWrapperBuilder(KN_BUILDER_QUASIQUOTE, event);

                    // open word to identifier mode ( abc to $abc)
                    reader->WordAsIdentifier = true;
                }
                else if (event == KN_TOKEN_UNQUOTE_REPLACE
                    || event == KN_TOKEN_UNQUOTE_KV
                    || event == KN_TOKEN_UNQUOTE_SEQ
                ) {
                    StateStackPush(reader->StateStack, KN_READER_PARSE_UNQUOTE);
                    builder = CreateWrapperBuilder(KN_BUILDER_UNQUOTE, event);
                }
                else if (event == KN_TOKEN_EXPAND_REPLACE) {
                    StateStackPush(reader->StateStack, KN_READER_PARSE_EXPAND_REPLACE);
                    builder = CreateWrapperBuilder(KN_BUILDER_EXPAND, event);
                }
                else if (event == KN_TOKEN_EXPAND_KV) {
                    StateStackPush(reader->StateStack, KN_READER_PARSE_EXPAND_KV);
                    builder = CreateWrapperBuilder(KN_BUILDER_EXPAND, event);
                }
                else if (event == KN_TOKEN_EXPAND_SEQ) {
                    StateStackPush(reader->StateStack, KN_READER_PARSE_EXPAND_SEQ);
                    builder = CreateWrapperBuilder(KN_BUILDER_EXPAND, event);
                }
            }
            BuilderStackPush(reader->BuilderStack, builder);
            continue;
        }

        else if (event == KN_TOKEN_TABLE_TAG) {
            if (currentState == KN_READER_PARSE_TABLE
                || currentState == KN_READER_PARSE_PARAM
            ) {
                StateStackPush(reader->StateStack, KN_READER_PARSE_TABLE_PAIR_KEY);
                KonBuilder* builder = CreateKvPairBuilder();
                BuilderStackPush(reader->BuilderStack, builder);
            }
            else if (currentState == KN_READER_PARSE_CELL_CORE
                || currentState == KN_READER_PARSE_CELL_MAP
            ) {
                StateStackPush(reader->StateStack, KN_READER_PARSE_MAP_PAIR_KEY);
                KonBuilder* builder = CreateKvPairBuilder();
                BuilderStackPush(reader->BuilderStack, builder);
            }
            // (: : )
            else if (currentState == KN_READER_PARSE_TABLE_PAIR_KEY
                || currentState == KN_READER_PARSE_MAP_PAIR_KEY
            ) {
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
                StateStackPush(reader->StateStack, KN_READER_PARSE_MAP_PAIR_KEY);
                KonBuilder* builder = CreateKvPairBuilder();
                BuilderStackPush(reader->BuilderStack, builder);
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
                    reader->StateStack,
                    KN_READER_PARSE_MAP_PAIR_VAL
                );
                continue;
            }
            else if (currentState == KN_READER_PARSE_TABLE_PAIR_EQ) {
                StateStackSetTopValue(
                    reader->StateStack,
                    KN_READER_PARSE_TABLE_PAIR_VAL
                );
                continue;
            }
         }
        else if (event == KN_TOKEN_EQUAL
            && currentState == KN_READER_PARSE_MAP_PAIR_EQ_OR_TAG
        ) {
            StateStackSetTopValue(
                reader->StateStack,
                KN_READER_PARSE_MAP_PAIR_VAL
            );
            continue;
        }

        // don't need to create new builder
        KonBuilder* topBuilder = BuilderStackTop(reader->BuilderStack);
        if (IsSyntaxToken(event)) {
            // syntax markers like: % . | ;
            // don't need update state
            KN marker = MakeSyntaxMarker(reader->Kstate, event);
            AddValueToTopBuilder(reader, marker);
        }
        else if (event == KN_TOKEN_SYM_PREFIX_WORD) {
            // prefix marcro eg !abc
            // top builder should be a list
            // don't need update state
            KN symbol = MakeSymbol(reader, event);
            AddValueToTopBuilder(reader, symbol);
        }
        else if (event == KN_TOKEN_SYM_SUFFIX_WORD) {
            // suffix marcro eg ^abc
            // TODO
        }
        else if (IsContainerEndToken(event)) {
            // an exceptional case: treat a '>' as a symbol
            // if not in a vector builder
            if (event == KN_TOKEN_VECTOR_END
                && topBuilder->Type != KN_BUILDER_VECTOR) {
                KN symbol = MakeSymbol(reader, KN_TOKEN_SYM_WORD);
                AddValueToTopBuilder(reader, symbol);
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
    KSON_TokenToString(reader->Tokenizer);  // should be EOF
    // TODO elegant exit
    if (StateStackTop(reader->StateStack) != KN_READER_ROOT) {
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
