#include "value_builder.h"
#include <tbox/tbox.h>

#define BUILDER_VECTOR_GROW_SIZE 20
// TODO
static tb_void_t builder_vector_item_ptr_free(tb_element_ref_t element, tb_pointer_t buff)
{
}

const char* BuilderTypeToCStr(KonBuilderType type)
{
    switch (type) {
        case KON_BUILDER_VECTOR: {
            return "KON_BUILDER_VECTOR";
        }
        case KON_BUILDER_LIST: {
            return "KON_BUILDER_LIST";
        }
        case KON_BUILDER_TABLE: {
            return "KON_BUILDER_TABLE";
        }
        case KON_BUILDER_TABLE_PAIR: {
            return "KON_BUILDER_TABLE_PAIR";
        }
        case KON_BUILDER_CELL: {
            return "KON_BUILDER_CELL";
        }
        case KON_BUILDER_QUOTE: {
            return "KON_BUILDER_QUOTE";
        }
        case KON_BUILDER_QUASIQUOTE: {
            return "KON_BUILDER_QUASIQUOTE";
        }
        case KON_BUILDER_EXPAND: {
            return "KON_BUILDER_EXPAND";
        }
        case KON_BUILDER_UNQUOTE: {
            return "KON_BUILDER_UNQUOTE";
        }
        default: {
            return "";
        }
    }
}

KonBuilder* CreateVectorBuilder()
{
    KonBuilder* builder = (KonBuilder*)malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = KON_BUILDER_VECTOR;
    builder->Vector = KxVector_Init();
    return builder;
}

void VectorBuilderAddItem(KonBuilder* builder, KN item)
{
    KxVector_Push(builder->Vector, item);
}

KN MakeVectorByBuilder(KonState* kstate, KonBuilder* builder)
{
    KonVector* value = KON_ALLOC_TYPE_TAG(kstate, KonVector, KON_T_VECTOR);
    value->Vector = builder->Vector;
    free(builder);
    return value;
}

KonBuilder* CreateListBuilder()
{
    KonBuilder* builder = (KonBuilder*)malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = KON_BUILDER_LIST;
    builder->List = KxVector_Init();
    return builder;
}

void ListBuilderAddItem(KonBuilder* builder, KN item)
{
    KxVector_Push(builder->List, item);
}

KN MakeListByBuilder(KonState* kstate, KonBuilder* builder)
{
    KN pair = KON_NIL;
    
    KonVector* list = builder->List;

    // reverse add
    int len = KxVector_Length(list);
    for (int i = len - 1; i >= 0; i--) {
        KN item = KxVector_AtIndex(list, i);
        pair = KON_CONS(kstate, item, pair);
    }

    return pair;
}

KonBuilder* CreateTableBuilder()
{
    KonBuilder* builder = (KonBuilder*)malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = KON_BUILDER_TABLE;
    builder->Table = KxHashTable_Init(10);;
    return builder;
}

void TableBuilderAddPair(KonBuilder* builder, KonBuilder* pair)
{
    char* key = KxStringBuffer_Cstr(pair->TablePair.Key);
    
    KxHashTable_PutKv(builder->Table, key, pair->TablePair.Value);
    KON_DEBUG("TableBuilderAddPair before free pair builder key %s", key);
    free(pair);
}

KN MakeTableByBuilder(KonState* kstate, KonBuilder* builder)
{
    KonTable* value = KON_ALLOC_TYPE_TAG(kstate, KonTable, KON_T_TABLE);
    value->Table = builder->Table;
    free(builder);
    return value;
}

KonBuilder* CreateTablePairBuilder()
{
    KonBuilder* builder = (KonBuilder*)malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = KON_BUILDER_TABLE_PAIR;
    builder->TablePair.Key = KxStringBuffer_New();
    builder->TablePair.Value = KON_NULL;
    return builder;
}

void TablePairSetKey(KonBuilder* builder, char* key)
{
    assert(key);
    KxStringBuffer_AppendCstr(builder->TablePair.Key, key);
}

void TablePairSetValue(KonBuilder* builder, KN value)
{
    assert(value);
    builder->TablePair.Value = value;
}

void TablePairDestroy(KonBuilder* builder)
{
    free(builder);
}

KonBuilder* MakeTablePairBuilder(KonBuilder* builder, KN value)
{
    builder->TablePair.Value = value;
    return builder;
}

KonBuilder* CreateCellBuilder()
{
    KonBuilder* builder = (KonBuilder*)malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = KON_BUILDER_CELL;
    builder->Cell.Core = KON_NULL;
    builder->Cell.Vector = KON_NULL;
    builder->Cell.Table = KON_NULL;
    builder->Cell.List = KON_NULL;
    return builder;
}

void CellBuilderSetName(KonBuilder* builder, KN name)
{
    KON_DEBUG("CellBuilderSetName");
    builder->Cell.Core = name;
}

void CellBuilderSetVector(KonBuilder* builder, KN vector)
{
    builder->Cell.Vector = vector;
}

void CellBuilderSetList(KonBuilder* builder, KN list)
{
    builder->Cell.List = list;
}

void CellBuilderSetTable(KonBuilder* builder, KN table)
{
    builder->Cell.Table = table;
}

KN MakeCellByBuilder(KonState* kstate, KonBuilder* builder)
{
    KonCell* value = KON_ALLOC_TYPE_TAG(kstate, KonCell, KON_T_CELL);
    value->Core = builder->Cell.Core;
    value->Vector = builder->Cell.Vector;
    value->Table = builder->Cell.Table;
    value->List = builder->Cell.List;
    free(builder);
    return value;
}

KonBuilder* CreateWrapperBuilder(KonBuilderType type, KonTokenKind tokenKind)
{
    KonBuilder* builder = (KonBuilder*)malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = type;
    builder->Wrapper.Inner = KON_NULL;
    builder->Wrapper.TokenKind = tokenKind;
    return builder;
}

void WrapperSetInner(KonState* kstate, KonBuilder* builder, KN inner)
{
    builder->Wrapper.Inner = inner;
}

KN MakeWrapperByBuilder(KonState* kstate, KonBuilder* builder)
{
    KN inner = builder->Wrapper.Inner;
    KonBuilderType type = builder->Type;
    KonTokenKind tokenKind = builder->Wrapper.TokenKind;
    KN result = KON_NULL;
    if (type == KON_BUILDER_QUOTE) {
        KonQuote* tmp = KON_ALLOC_TYPE_TAG(kstate, KonQuote, KON_T_QUOTE);
        tmp->Inner = inner;
        switch (tokenKind) {
            case KON_TOKEN_SYM_IDENTIFIER: {
                tmp->Type = KON_QUOTE_IDENTIFER;
                break;
            }
            case KON_TOKEN_SYM_STRING: {
                tmp->Type = KON_QUOTE_SYMBOL;
                break;
            }
            case KON_TOKEN_QUOTE_VECTOR: {
                tmp->Type = KON_QUOTE_VECTOR;
                break;
            }
            case KON_TOKEN_QUOTE_LIST: {
                tmp->Type = KON_QUOTE_LIST;
                break;
            }
            case KON_TOKEN_QUOTE_TABLE: {
                tmp->Type = KON_QUOTE_TABLE;
                break;
            }
            case KON_TOKEN_QUOTE_CELL: {
                tmp->Type = KON_QUOTE_CELL;
                break;
            }
        }
        result = tmp;
    }
    else if (type == KON_BUILDER_QUASIQUOTE) {
        KonQuasiquote* tmp = KON_ALLOC_TYPE_TAG(kstate, KonQuasiquote, KON_T_QUASIQUOTE);
        tmp->Inner = inner;
        switch (tokenKind) {
            case KON_TOKEN_QUASI_VECTOR: {
                tmp->Type = KON_QUASI_VECTOR;
                break;
            }
            case KON_TOKEN_QUASI_LIST: {
                tmp->Type = KON_QUASI_LIST;
                break;
            }
            case KON_TOKEN_QUASI_TABLE: {
                tmp->Type = KON_QUASI_TABLE;
                break;
            }
            case KON_TOKEN_QUASI_CELL: {
                tmp->Type = KON_QUASI_CELL;
                break;
            }
        }
        result = tmp;
    }
    else if (type == KON_BUILDER_EXPAND) {
        KonExpand* tmp = KON_ALLOC_TYPE_TAG(kstate, KonExpand, KON_T_EXPAND);
        tmp->Inner = inner;
        switch (tokenKind) {
            case KON_TOKEN_EXPAND_REPLACE: {
                tmp->Type = KON_EXPAND_REPLACE;
                break;
            }
            case KON_TOKEN_EXPAND_KV: {
                tmp->Type = KON_EXPAND_KV;
                break;
            }
            case KON_TOKEN_EXPAND_SEQ: {
                tmp->Type = KON_EXPAND_SEQ;
                break;
            }
        }
        result = tmp;
    }
    else if (type == KON_BUILDER_UNQUOTE) {
        KonUnquote* tmp = KON_ALLOC_TYPE_TAG(kstate, KonUnquote, KON_T_UNQUOTE);
        tmp->Inner = inner;
        switch (tokenKind) {
            case KON_TOKEN_UNQUOTE_REPLACE: {
                tmp->Type = KON_UNQUOTE_REPLACE;
                break;
            }
            case KON_TOKEN_UNQUOTE_KV: {
                tmp->Type = KON_UNQUOTE_KV;
                break;
            }
            case KON_TOKEN_UNQUOTE_SEQ: {
                tmp->Type = KON_UNQUOTE_SEQ;
                break;
            }
        }
        result = tmp;
    }
    free(builder);
    return result;
}

// builder stack
BuilderStack* BuilderStackInit()
{
    BuilderStack* stack = (BuilderStack*)malloc(sizeof(BuilderStack));
    if (stack == NULL) {
        return NULL;
    }
    stack->Length = 0;
    stack->Top = NULL;
    return stack;
}

void BuilderStackDestroy(BuilderStack* stack)
{
    // TODO iter node, free
    BuilderStackNode* top = stack->Top;
    while (top) {
        BuilderStackNode* oldTop = top;
        free(oldTop);
        top = top->Next;
    }
    free(stack);
}

void BuilderStackPush(BuilderStack* stack, KonBuilder* item)
{
    assert(stack);
    BuilderStackNode* oldTop = stack->Top;
    BuilderStackNode* newTop = (BuilderStackNode*)malloc(sizeof(BuilderStackNode));
    assert(newTop);
    newTop->Data = item;
    newTop->Next = oldTop;
    stack->Top = newTop;
    stack->Length = stack->Length + 1;
}

KonBuilder* BuilderStackPop(BuilderStack* stack)
{
    assert(stack);
    BuilderStackNode* top = stack->Top;
    assert(top);
    BuilderStackNode* next = stack->Top->Next;

    KonBuilder* data = top->Data;
    stack->Top = next;
    stack->Length = stack->Length - 1;

    free(top);
    return data;
}

long BuilderStackLength(BuilderStack* stack)
{
    assert(stack);
    return stack->Length;
}

KonBuilder* BuilderStackTop(BuilderStack* stack)
{
    assert(stack);
    BuilderStackNode* top = stack->Top;
    assert(top);
    return top->Data;
}


