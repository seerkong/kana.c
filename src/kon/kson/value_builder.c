#include "value_builder.h"
#include <tbox/tbox.h>

#define BUILDER_VECTOR_GROW_SIZE 20
// TODO
static tb_void_t builder_vector_item_ptr_free(tb_element_ref_t element, tb_pointer_t buff)
{
}

KonBuilder* CreateVectorBuilder()
{
    KonBuilder* builder = (KonBuilder*)malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = KON_BUILDER_VECTOR;
    builder->Vector = tb_vector_init(BUILDER_VECTOR_GROW_SIZE, tb_element_ptr(kon_vector_item_ptr_free, "ValueBuilderType"));
    return builder;
}

void VectorBuilderAddItem(KonBuilder* builder, Kon* item)
{
    tb_vector_insert_tail(builder->Vector, (tb_cpointer_t)item);
}

Kon* MakeVectorByBuilder(Kon* kstate, KonBuilder* builder)
{
//    Kon* value = kon_alloc_type(kstate, Vector, KON_VECTOR);
    Kon* value = KON_AllocTagged(kstate, sizeof(tb_vector_ref_t), KON_VECTOR);
    printf("MakeVectorByBuilder value->Value.Vector %x\n", value->Value.Vector);
    value->Value.Vector = builder->Vector;
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
    builder->List = tb_stack_init(BUILDER_VECTOR_GROW_SIZE, tb_element_ptr(builder_vector_item_ptr_free, "ValueBuilderType"));
    return builder;
}

void ListBuilderAddItem(KonBuilder* builder, Kon* item)
{
    tb_vector_insert_tail(builder->List, (tb_cpointer_t)item);
}

Kon* MakeListByBuilder(Kon* kstate, KonBuilder* builder)
{
    Kon* pair = KON_NIL;
    
    tb_vector_ref_t list = builder->List;

    // reverse add

    tb_size_t head = tb_iterator_head(list);
    tb_size_t itor = tb_iterator_tail(list);

    do {
        // the previous item
        itor = tb_iterator_prev(list, itor);
        
        Kon* item = (Kon*)tb_iterator_item(list, itor);
        if (item == NULL) {
            break;
        }
        printf("list builder item %d\n", (int)item);
        pair = kon_cons(kstate, item, pair);
        
    } while (itor != head);

    return pair;
}

KonBuilder* CreateTableBuilder()
{
    KonBuilder* builder = (KonBuilder*)malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = KON_BUILDER_TABLE;
    builder->Table = tb_hash_map_init(8, tb_element_str(tb_true), tb_element_ptr(kon_hash_item_ptr_free, "ValueBuilderType"));
    return builder;
}

void TableBuilderAddPair(KonBuilder* builder, KonBuilder* pair)
{
    // char* key = pair->TablePair.Key;
    char* key = tb_string_cstr(&(pair->TablePair.Key));
    
    tb_hash_map_insert(builder->Table, key, (tb_pointer_t)pair->TablePair.Value);
    printf("TableBuilderAddPair before free pair builder key %s\n", key);
    free(pair);
    // tb_hash_map_dump(builder->Table);
}

Kon* MakeTableByBuilder(Kon* kstate, KonBuilder* builder)
{
    Kon* value = kon_alloc_type(kstate, Table, KON_TABLE);
    value->Value.Table = builder->Table;
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
    tb_string_init(&(builder->TablePair.Key));
    builder->TablePair.Value = KON_NULL;
    return builder;
}

void TablePairSetKey(KonBuilder* builder, char* key)
{
    assert(key);
    // builder->TablePair.Key = key;
    tb_string_cstrcat(&(builder->TablePair.Key), key);
}

void TablePairSetValue(KonBuilder* builder, Kon* value)
{
    assert(value);
    builder->TablePair.Value = value;
}

void TablePairDestroy(KonBuilder* builder)
{
    free(builder);
}

KonBuilder* MakeTablePairBuilder(KonBuilder* builder, Kon* value)
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
    builder->Cell.Name = KON_NULL;
    builder->Cell.Vector = KON_NULL;
    builder->Cell.Table = KON_NULL;
    builder->Cell.List = KON_NULL;
    return builder;
}

void CellBuilderSetName(KonBuilder* builder, Kon* name)
{
    printf("CellBuilderSetName\n");
    builder->Cell.Name = name;
}

void CellBuilderSetVector(KonBuilder* builder, Kon* vector)
{
    builder->Cell.Vector = vector;
}

void CellBuilderSetList(KonBuilder* builder, Kon* list)
{
    builder->Cell.List = list;
}

void CellBuilderSetTable(KonBuilder* builder, Kon* table)
{
    builder->Cell.Table = table;
}

Kon* MakeCellByBuilder(Kon* kstate, KonBuilder* builder)
{
    Kon* value = kon_alloc_type(kstate, Cell, KON_CELL);
    value->Value.Cell.Name = builder->Cell.Name;
    value->Value.Cell.Vector = builder->Cell.Vector;
    value->Value.Cell.Table = builder->Cell.Table;
    value->Value.Cell.List = builder->Cell.List;
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

void WrapperSetInner(Kon* kstate, KonBuilder* builder, Kon* inner)
{
    builder->Wrapper.Inner = inner;
}

Kon* MakeWrapperByBuilder(Kon* kstate, KonBuilder* builder)
{
    Kon* inner = builder->Wrapper.Inner;
    KonBuilderType type = builder->Type;
    KonTokenKind tokenKind = builder->Wrapper.TokenKind;
    Kon* result = KON_NULL;
    if (type == KON_BUILDER_QUOTE) {
        result = KON_AllocTagged(kstate, sizeof(KonQuote), KON_QUOTE);
        result->Value.Quote.Inner = inner;
        switch (tokenKind) {
            case KON_TOKEN_QUOTE_IDENTIFER: {
                result->Value.Quote.Type = KON_QUOTE_IDENTIFER;
                break;
            }
            case KON_TOKEN_QUOTE_SYMBOL: {
                result->Value.Quote.Type = KON_QUOTE_SYMBOL;
                break;
            }
            case KON_TOKEN_QUOTE_VECTOR: {
                result->Value.Quote.Type = KON_QUOTE_VECTOR;
                break;
            }
            case KON_TOKEN_QUOTE_LIST: {
                result->Value.Quote.Type = KON_QUOTE_LIST;
                break;
            }
            case KON_TOKEN_QUOTE_TABLE: {
                result->Value.Quote.Type = KON_QUOTE_TABLE;
                break;
            }
            case KON_TOKEN_QUOTE_CELL: {
                result->Value.Quote.Type = KON_QUOTE_CELL;
                break;
            }
        }
    }
    else if (type == KON_BUILDER_QUASIQUOTE) {
        result = KON_AllocTagged(kstate, sizeof(KonQuasiquote), KON_QUASIQUOTE);
        result->Value.Quasiquote.Inner = inner;
        switch (tokenKind) {
            case KON_TOKEN_QUASI_VECTOR: {
                result->Value.Quasiquote.Type = KON_QUASI_VECTOR;
                break;
            }
            case KON_TOKEN_QUASI_LIST: {
                result->Value.Quasiquote.Type = KON_QUASI_LIST;
                break;
            }
            case KON_TOKEN_QUASI_TABLE: {
                result->Value.Quasiquote.Type = KON_QUASI_TABLE;
                break;
            }
            case KON_TOKEN_QUASI_CELL: {
                result->Value.Quasiquote.Type = KON_QUASI_CELL;
                break;
            }
        }
    }
    else if (type == KON_BUILDER_EXPAND) {
        result = KON_AllocTagged(kstate, sizeof(KonExpand), KON_EXPAND);
        result->Value.Expand.Inner = inner;
        switch (tokenKind) {
            case KON_TOKEN_EXPAND_REPLACE: {
                result->Value.Expand.Type = KON_EXPAND_REPLACE;
                break;
            }
            case KON_TOKEN_EXPAND_VECTOR: {
                result->Value.Expand.Type = KON_EXPAND_VECTOR;
                break;
            }
            case KON_TOKEN_EXPAND_TABLE: {
                result->Value.Expand.Type = KON_EXPAND_LIST;
                break;
            }
            case KON_TOKEN_EXPAND_LIST: {
                result->Value.Expand.Type = KON_EXPAND_TABLE;
                break;
            }
        }
    }
    else if (type == KON_BUILDER_UNQUOTE) {
        result = KON_AllocTagged(kstate, sizeof(KonUnquote), KON_UNQUOTE);
        result->Value.Unquote.Inner = inner;
        switch (tokenKind) {
            case KON_TOKEN_UNQUOTE_REPLACE: {
                result->Value.Unquote.Type = KON_UNQUOTE_REPLACE;
                break;
            }
            case KON_TOKEN_UNQUOTE_VECTOR: {
                result->Value.Unquote.Type = KON_UNQUOTE_VECTOR;
                break;
            }
            case KON_TOKEN_UNQUOTE_TABLE: {
                result->Value.Unquote.Type = KON_UNQUOTE_LIST;
                break;
            }
            case KON_TOKEN_UNQUOTE_LIST: {
                result->Value.Unquote.Type = KON_UNQUOTE_TABLE;
                break;
            }
        }
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


