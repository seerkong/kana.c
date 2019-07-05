#include "reader.h"
#include <tbox/tbox.h>


#define TB_VECTOR_GROW_SIZE (256)

void ExitTopBuilder(KonReader* reader);
void AddValueToTopBuilder(KonReader* reader, Kon* value);


KonReader* KSON_ReaderInit(Kon* kstate)
{
    // init reader
    KonReader* reader = (KonReader*)malloc(sizeof(KonReader));
    printf("[KSON_ReaderInit] malloc reader %x\n", reader);
    printf("before tb_assert_and_check_return_val\n");
    tb_assert_and_check_return_val(reader, tb_null);
    
    reader->Kstate = kstate;
    
    
    reader->Tokenizer = KSON_TokenizerInit(kstate);
    printf("after KSON_TokenizerInit\n");
    
    reader->BuilderStack = BuilderStackInit();
    reader->StateStack = StateStackInit();
    
    tb_assert_and_check_return_val(reader->BuilderStack, tb_null);
    tb_assert_and_check_return_val(reader->StateStack, tb_null);
    return reader;
}

bool KSON_ReaderOpenStream(KonReader* reader, tb_stream_ref_t stream, bool isOwnedStream)
{
    printf("IsOwnedStream %d reader %x\n", isOwnedStream, reader);
    reader->IsOwnedStream = isOwnedStream;
    if (stream) {
        bool openRes = KSON_TokenizerOpenStream(reader->Tokenizer, stream);
        if (!openRes) {
            return false;
        }
        reader->ReadStream = stream;
    }
    else {
        return false;
    }
    return true;
}

bool IsSkipToken(tb_size_t event)
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
bool IsContainerStartToken(tb_size_t event)
{
    if (event == KON_TOKEN_LIST_START
        || event == KON_TOKEN_VECTOR_START
        || event == KON_TOKEN_TABLE_START
        || event == KON_TOKEN_CELL_START
    ) {
        return true;
    }
    else {
        return false;
    }
}

bool IsContainerEndToken(tb_size_t event)
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
// $. $[]. $(). ${}.
// $e. $[]e. $()e. ${}e.
bool IsWrapperToken(tb_size_t event)
{
    if (event == KON_TOKEN_QUOTE_IDENTIFER
        || event == KON_TOKEN_QUOTE_SYMBOL
        || event == KON_TOKEN_QUOTE_VECTOR
        || event == KON_TOKEN_QUOTE_LIST
        || event == KON_TOKEN_QUOTE_TABLE
        || event == KON_TOKEN_QUOTE_CELL
        || event == KON_TOKEN_QUASI_VECTOR
        || event == KON_TOKEN_QUASI_LIST
        || event == KON_TOKEN_QUASI_TABLE
        || event == KON_TOKEN_QUASI_CELL
        || event == KON_TOKEN_EXPAND_REPLACE
        || event == KON_TOKEN_EXPAND_VECTOR
        || event == KON_TOKEN_EXPAND_TABLE
        || event == KON_TOKEN_EXPAND_LIST
        || event == KON_TOKEN_UNQUOTE_REPLACE
        || event == KON_TOKEN_UNQUOTE_VECTOR
        || event == KON_TOKEN_UNQUOTE_TABLE
        || event == KON_TOKEN_UNQUOTE_LIST
    ) {
        return true;
    }
    else {
        return false;
    }
}

bool IsLiteralToken(tb_size_t event)
{
    if (event == KON_TOKEN_KEYWORD_NIL
        || event == KON_TOKEN_KEYWORD_NULL
        || event == KON_TOKEN_KEYWORD_UKN
        || event == KON_TOKEN_KEYWORD_TRUE
        || event == KON_TOKEN_KEYWORD_FALSE
        || event == KON_TOKEN_KEYWORD_EITHER
        || event == KON_TOKEN_KEYWORD_BOTH
        || event == KON_TOKEN_LITERAL_NUMBER
        || event == KON_TOKEN_LITERAL_STRING
    ) {
        return true;
    }
    else {
        return false;
    }
}

// % . | ;
bool IsSyntaxToken(tb_size_t event)
{
    if (event == KON_TOKEN_APPLY
        || event == KON_TOKEN_EXEC_MSG
        || event == KON_TOKEN_PIPE
        || event == KON_TOKEN_CLAUSE_END
    ) {
        return true;
    }
    else {
        return false;
    }
}



Kon* MakeSyntaxMarker(Kon* kstate, KonTokenKind tokenKind)
{
    Kon* value = KON_NULL;
    switch (tokenKind) {
        case KON_TOKEN_APPLY: {
            value = kon_alloc_type(kstate, SyntaxMarker, KON_SYNTAX_MARKER);
            value->Value.SyntaxMarker.Type = KON_SYNTAX_MARKER_APPLY;
            break;
        }
        case KON_TOKEN_EXEC_MSG: {
            value = kon_alloc_type(kstate, SyntaxMarker, KON_SYNTAX_MARKER);
            value->Value.SyntaxMarker.Type = KON_SYNTAX_MARKER_EXEC_MSG;
            break;
        }
        case KON_TOKEN_PIPE: {
            value = kon_alloc_type(kstate, SyntaxMarker, KON_SYNTAX_MARKER);
            value->Value.SyntaxMarker.Type = KON_SYNTAX_MARKER_PIPE;
            break;
        }
        case KON_TOKEN_CLAUSE_END: {
            value = kon_alloc_type(kstate, SyntaxMarker, KON_SYNTAX_MARKER);
            value->Value.SyntaxMarker.Type = KON_SYNTAX_MARKER_CLAUSE_END;
            break;
        }
        default: {
            break;
        }
    }
    return value;
}

Kon* MakeSymbol(KonReader* reader, KonTokenKind event)
{
    Kon* value = kon_alloc_type(reader->Kstate, Symbol, KON_SYMBOL);
    if (event == KON_TOKEN_SYM_FORM_WORD) {
        value->Value.Symbol.Type = KON_SYM_FORM_WORD;
    }
    else if (event == KON_TOKEN_SYM_VARIABLE) {
        value->Value.Symbol.Type = KON_SYM_VAR;
    }
    else if (event == KON_TOKEN_SYM_IDENTIFIER) {
        value->Value.Symbol.Type = KON_SYM_IDENTIFER;
    }
    else if (event == KON_TOKEN_SYM_STRING) {
        value->Value.Symbol.Type = KON_SYM_STRING;
    }
    
    tb_string_init(&(value->Value.Symbol.Data));
    tb_string_strcat(&value->Value.Symbol.Data, &reader->Tokenizer->Content);

    return value;
}

Kon* MakeSymFormWord(KonReader* reader, KonTokenKind event)
{
    Kon* value = kon_alloc_type(reader->Kstate, Symbol, KON_SYMBOL);
    value->Value.Symbol.Type = KON_SYM_IDENTIFER;
    tb_string_init(&(value->Value.Symbol.Data));
    tb_string_strcat(&value->Value.Symbol.Data, &reader->Tokenizer->Content);
    return value;
}


// num token to kon number
Kon* MakeNumber(KonReader* reader)
{
    bool isPositive = reader->Tokenizer->NumIsPositive;
    char* numStrBeforeDot = tb_string_cstr(&reader->Tokenizer->NumBeforeDot);
    char* numStrAfterDot = tb_string_cstr(&reader->Tokenizer->NumAfterDot);
    char* numStrAfterPower = tb_string_cstr(&reader->Tokenizer->NumAfterPower);
    
    Kon* value = KON_ZERO;
    
    if (isPositive
        && numStrAfterDot == NULL
        && numStrAfterPower == NULL
    ) {
        // TODO add judgment, if out of range
        // 32 bit system : use 4*8-1 bits;
        // 64 bit system : use 8*8-1 bits;
        int numBeforeDot = atoi(numStrBeforeDot);
        value = kon_make_fixnum(numBeforeDot);
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

Kon* MakeString(KonReader* reader)
{
    Kon* value = KON_MakeString(reader->Kstate, tb_string_cstr(&reader->Tokenizer->Content));
    return value;
}

Kon* MakeLiteral(KonReader* reader, KonTokenKind event)
{
    Kon* value = KON_UKN;

    if (event == KON_TOKEN_KEYWORD_NIL) {
        value = KON_NIL;
    }
    else if (event == KON_TOKEN_KEYWORD_NULL) {
        value = KON_NULL;
    }
    else if (event == KON_TOKEN_KEYWORD_UKN) {
        value = KON_UKN;
    }
    else if (event == KON_TOKEN_KEYWORD_TRUE) {
        value = KON_TRUE;
    }
    else if (event == KON_TOKEN_KEYWORD_FALSE) {
        value = KON_TRUE;
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

    
    return value;
}

void AddValueToTopBuilder(KonReader* reader, Kon* value)
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
//    kon_debug("AddValueToTopBuilder builder type %d", (int)builderType);
    if (builderType == KON_BUILDER_VECTOR) {
        VectorBuilderAddItem(topBuilder, value);
    }
    else if (builderType == KON_BUILDER_LIST) {
        ListBuilderAddItem(topBuilder, value);
    }
    else if (builderType == KON_BUILDER_TABLE_PAIR) {
        if (currentState == KON_READER_PARSE_TABLE_KEY) {
            assert(kon_is_symbol(value));
            KonSymbolType symbolType = kon_field(value, Symbol, KON_SYMBOL, Type);
            assert(symbolType != KON_SYM_VAR && symbolType != KON_SYM_FORM_WORD);
            // table tag key should not be NULL
            char* tableKey = tb_string_cstr(&value->Value.Symbol.Data);
            assert(tableKey);
            TablePairSetKey(topBuilder, tableKey);

            StateStackSetTopValue(
                reader->StateStack,
                KON_READER_PARSE_TABLE_VAL_OR_NEXT
            );
        }
        // if current in table pair builder, that means pair is parse finished，
        // should add this pair to parent table builder
        else {
            TablePairSetValue(topBuilder, value);
            KonBuilder* pairBuilder = BuilderStackPop(reader->BuilderStack);
            KonBuilder* tableBuilder = BuilderStackTop(reader->BuilderStack);
            TableBuilderAddPair(tableBuilder, pairBuilder);

            StateStackSetTopValue(
                reader->StateStack,
                KON_READER_PARSE_TABLE
            );
        }
    }
    else if (builderType == KON_BUILDER_CELL) {
        if (kon_is_vector(value)) {
            CellBuilderSetVector(topBuilder, value);
        }
        else if (kon_is_table(value)) {
            CellBuilderSetTable(topBuilder, value);
        }
        else if (KON_IsList(value)) {
            CellBuilderSetList(topBuilder, value);
        }
        else {
            assert(kon_is_symbol(value));
            KonSymbolType symbolType = kon_field(value, Symbol, KON_SYMBOL, Type);
            assert(symbolType != KON_SYM_FORM_WORD);
            CellBuilderSetSubj(topBuilder, value);
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

    printf("ExitTopBuilder builder type %d\n", builderType);

    Kon* value;
    if (builderType == KON_BUILDER_VECTOR) {
        value = MakeVectorByBuilder(reader->Kstate, topBuilder);
    }
    else if (builderType == KON_BUILDER_LIST) {
        value = MakeListByBuilder(reader->Kstate, topBuilder);
    }
    else if (builderType == KON_BUILDER_TABLE_PAIR) {
        // table pair parse incomplete, abandon
        TablePairDestroy(topBuilder);
        BuilderStackPop(reader->BuilderStack);
        topBuilder = BuilderStackTop(reader->BuilderStack);
        StateStackPop(reader->StateStack);

        value = MakeTableByBuilder(reader->Kstate, topBuilder);
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
    }

    // resume last container builder
    BuilderStackPop(reader->BuilderStack);
    // resume last state
    StateStackPop(reader->StateStack);

    AddValueToTopBuilder(reader, value);
}

void ExitAllStackBuilders()
{
    kon_debug("ExitAllStackBuilders");
    // KonBuilder* newTopBuilder = BuilderStackTop(reader->BuilderStack);
}

Kon* KSON_Parse(KonReader* reader)
{
    tb_size_t initState = KON_READER_ROOT;
    StateStackPush(reader->StateStack, initState);
    printf("[KSON_Parse]start\n");

    tb_size_t event = KON_TOKEN_NONE;
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
                && currentState == KON_READER_PARSE_CELL_TAG
            ) {
                StateStackSetTopValue(reader->StateStack, KON_READER_PARSE_CELL_INNER_CONTAINER);
            }

            StateStackPush(reader->StateStack, currentState);

            KonBuilder* builder;
            if (event == KON_TOKEN_LIST_START) {
                StateStackSetTopValue(reader->StateStack, KON_READER_PARSE_LIST);
                builder = CreateListBuilder();
            }
            else if (event == KON_TOKEN_VECTOR_START) {
                printf("event is KON_TOKEN_VECTOR_START\n");
                StateStackSetTopValue(reader->StateStack, KON_READER_PARSE_VECTOR);
                KonBuilder* builder2 = CreateVectorBuilder();
                printf("builder type %d\n", builder2->Type);
                BuilderStackPush(reader->BuilderStack, builder2);
                continue;
            }
            else if (event == KON_TOKEN_TABLE_START) {
                StateStackSetTopValue(reader->StateStack, KON_READER_PARSE_TABLE);
                builder = CreateTableBuilder();
            }
            else if (event == KON_TOKEN_CELL_START) {
                StateStackSetTopValue(
                    reader->StateStack,
                    KON_READER_PARSE_CELL_TAG
                );
                builder = CreateCellBuilder();
            }
            else {
                // wrapper types
                if (event == KON_TOKEN_QUOTE_IDENTIFER
                    || event == KON_TOKEN_QUOTE_SYMBOL
                    || event == KON_TOKEN_QUOTE_VECTOR
                    || event == KON_TOKEN_QUOTE_LIST
                    || event == KON_TOKEN_QUOTE_TABLE
                    || event == KON_TOKEN_QUOTE_CELL
                ) {
                    StateStackSetTopValue(
                        reader->StateStack,
                        KON_READER_PARSE_QUOTE
                    );
                    builder = CreateWrapperBuilder(KON_BUILDER_QUOTE, event);
                }
                else if (event == KON_TOKEN_QUASI_VECTOR
                    || event == KON_TOKEN_QUASI_LIST
                    || event == KON_TOKEN_QUASI_TABLE
                    || event == KON_TOKEN_QUASI_CELL
                ) {
                    StateStackSetTopValue(
                        reader->StateStack,
                        KON_READER_PARSE_QUASIQUOTE
                    );
                    builder = CreateWrapperBuilder(KON_BUILDER_QUASIQUOTE, event);
                }
                else if (event == KON_TOKEN_UNQUOTE_REPLACE
                    || event == KON_TOKEN_UNQUOTE_VECTOR
                    || event == KON_TOKEN_UNQUOTE_TABLE
                    || event == KON_TOKEN_UNQUOTE_LIST
                ) {
                    StateStackSetTopValue(
                        reader->StateStack,
                        KON_READER_PARSE_UNQUOTE
                    );
                    builder = CreateWrapperBuilder(KON_BUILDER_UNQUOTE, event);
                }
                else if (event == KON_TOKEN_EXPAND_REPLACE) {
                    StateStackSetTopValue(
                        reader->StateStack,
                        KON_READER_PARSE_EXPAND_REPLACE
                    );
                    builder = CreateWrapperBuilder(KON_BUILDER_EXPAND, event);
                }
                else if (event == KON_TOKEN_EXPAND_VECTOR) {
                    StateStackSetTopValue(
                        reader->StateStack,
                        KON_READER_PARSE_EXPAND_VECTOR
                    );
                    builder = CreateWrapperBuilder(KON_BUILDER_EXPAND, event);
                }
                else if (event == KON_TOKEN_EXPAND_TABLE) {
                    StateStackSetTopValue(
                        reader->StateStack,
                        KON_READER_PARSE_EXPAND_TABLE
                    );
                    builder = CreateWrapperBuilder(KON_BUILDER_EXPAND, event);
                }
                else if (event == KON_TOKEN_EXPAND_LIST) {
                    StateStackSetTopValue(
                        reader->StateStack,
                        KON_READER_PARSE_EXPAND_LIST
                    );
                    builder = CreateWrapperBuilder(KON_BUILDER_EXPAND, event);
                }
            }
            BuilderStackPush(reader->BuilderStack, builder);
            continue;
        }

        else if (event == KON_TOKEN_TABLE_TAG) {
            if (currentState == KON_READER_PARSE_TABLE_VAL_OR_NEXT) {
                // set default hash item value to TRUE
                AddValueToTopBuilder(reader, KON_TRUE);
            }
            StateStackSetTopValue(
                reader->StateStack,
                KON_READER_PARSE_TABLE_KEY
            );
            // k v pair builder
            KonBuilder* builder = CreateTablePairBuilder();
            BuilderStackPush(reader->BuilderStack, builder);
            continue;
        }

        // don't need to create new builder
        KonBuilder* topBuilder = BuilderStackTop(reader->BuilderStack);
        if (IsSyntaxToken(event)) {
            // syntax markers like: % . | ; 
            // top builder should be a list
            // don't need update state
            assert(topBuilder && topBuilder->Type == KON_BUILDER_LIST);
            Kon* marker = MakeSyntaxMarker(reader->Kstate, event);
            AddValueToTopBuilder(reader, marker);
        }
        else if (event == KON_TOKEN_SYM_FORM_WORD) {
            // prefix marcro eg !abc
            // top builder should be a list
            // don't need update state
            assert(topBuilder && topBuilder->Type == KON_BUILDER_LIST);
            Kon* symbol = MakeSymbol(reader, event);
            AddValueToTopBuilder(reader, symbol);
        }
        else if (IsContainerEndToken(event)) {
            ExitTopBuilder(reader);
            continue;
        }
        
        else if (event == KON_TOKEN_SYM_STRING
            || event == KON_TOKEN_SYM_IDENTIFIER
        ) {
            Kon* symbol = MakeSymbol(reader, event);
            AddValueToTopBuilder(reader, symbol);
        }
        else if (event == KON_TOKEN_SYM_VARIABLE) {
            Kon* value = MakeSymbol(reader, event);
            AddValueToTopBuilder(reader, value);
        }
        else if (IsLiteralToken(event)) {
            Kon* value = MakeLiteral(reader, event);
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
    KSON_TokenizerCloseStream(reader->Tokenizer);
    if (reader->IsOwnedStream) {
        tb_stream_exit(reader->ReadStream);
    }
}

void KSON_ReaderExit(KonReader* reader)
{
    BuilderStackDestroy(reader->BuilderStack);
    StateStackDestroy(reader->StateStack);
    KSON_TokenizerExit(reader->Tokenizer);
}
