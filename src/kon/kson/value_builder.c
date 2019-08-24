#include "value_builder.h"
#include "tbox/tbox.h"

const char* BuilderTypeToCStr(KonBuilderType type)
{
    switch (type) {
        case KN_BUILDER_VECTOR: {
            return "KN_BUILDER_VECTOR";
        }
        case KN_BUILDER_LIST: {
            return "KN_BUILDER_LIST";
        }
        case KN_BUILDER_TABLE: {
            return "KN_BUILDER_TABLE";
        }
        case KN_BUILDER_KV_PAIR: {
            return "KN_BUILDER_KV_PAIR";
        }
        case KN_BUILDER_CELL: {
            return "KN_BUILDER_CELL";
        }
        case KN_BUILDER_QUOTE: {
            return "KN_BUILDER_QUOTE";
        }
        case KN_BUILDER_QUASIQUOTE: {
            return "KN_BUILDER_QUASIQUOTE";
        }
        case KN_BUILDER_EXPAND: {
            return "KN_BUILDER_EXPAND";
        }
        case KN_BUILDER_UNQUOTE: {
            return "KN_BUILDER_UNQUOTE";
        }
        default: {
            return "";
        }
    }
}

KonBuilder* CreateVectorBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = KN_BUILDER_VECTOR;
    builder->Vector = KxVector_Init();
    return builder;
}

void VectorBuilderAddItem(KonBuilder* builder, KN item)
{
    KxVector_Push(builder->Vector, item);
}

KN MakeVectorByBuilder(KonState* kstate, KonBuilder* builder)
{
    KonVector* value = KN_ALLOC_TYPE_TAG(kstate, KonVector, KN_T_VECTOR);
    value->Vector = builder->Vector;
    tb_free(builder);
    return value;
}

KonBuilder* CreateListBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = KN_BUILDER_LIST;
    builder->List = KxVector_Init();
    return builder;
}

void ListBuilderAddItem(KonBuilder* builder, KN item)
{
    KxVector_Push(builder->List, item);
}

KN MakeListByBuilder(KonState* kstate, KonBuilder* builder)
{
    KN pair = KN_NIL;
    
    KonVector* list = builder->List;

    // reverse add
    int len = KxVector_Length(list);
    for (int i = len - 1; i >= 0; i--) {
        KN item = KxVector_AtIndex(list, i);
        pair = KN_CONS(kstate, item, pair);
    }

    return pair;
}


KonBuilder* CreateBlockBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = KN_BUILDER_BLOCK;
    builder->Block = KxVector_Init();
    return builder;
}

void BlockBuilderAddItem(KonBuilder* builder, KN item)
{
    KxVector_Push(builder->Block, item);
}

KN MakeBlockByBuilder(KonState* kstate, KonBuilder* builder)
{
    KN pair = KN_NIL;
    
    KonVector* list = builder->Block;

    // reverse add
    int len = KxVector_Length(list);
    for (int i = len - 1; i >= 0; i--) {
        KN item = KxVector_AtIndex(list, i);
        pair = KN_CONS(kstate, item, pair);
    }
    if (len > 0) {
        // change first element tag to BLOCK
        ((KonBase*)pair)->Tag = KN_T_BLOCK;
    }

    return pair;
}

KonBuilder* CreateParamBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = KN_BUILDER_PARAM;
    builder->Param = KxHashTable_Init(10);;
    return builder;
}

void ParamBuilderAddPair(KonBuilder* builder, KonBuilder* pair)
{
    char* key = KxStringBuffer_Cstr(pair->KvPair.Key);
    
    KxHashTable_PushKv(builder->Param, key, pair->KvPair.Value);
    KN_DEBUG("TableBuilderAddPair before free pair builder key %s", key);
    tb_free(pair);
}

void ParamBuilderAddValue(KonBuilder* builder, KN value)
{
    KxHashTable_PushVal(builder->Param, value);
}

KN MakeParamByBuilder(KonState* kstate, KonBuilder* builder)
{
    KonParam* value = KN_ALLOC_TYPE_TAG(kstate, KonTable, KN_T_PARAM);
    value->Table = builder->Param;
    tb_free(builder);
    return value;
}


KonBuilder* CreateTableBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = KN_BUILDER_TABLE;
    builder->Table = KxHashTable_Init(10);;
    return builder;
}

void TableBuilderAddPair(KonBuilder* builder, KonBuilder* pair)
{
    char* key = KxStringBuffer_Cstr(pair->KvPair.Key);
    
    KxHashTable_PushKv(builder->Table, key, pair->KvPair.Value);
    KN_DEBUG("TableBuilderAddPair before free pair builder key %s", key);
    tb_free(pair);
}

void TableBuilderAddValue(KonBuilder* builder, KN value)
{
    KxHashTable_PushVal(builder->Table, value);
}

KN MakeTableByBuilder(KonState* kstate, KonBuilder* builder)
{
    KonTable* value = KN_ALLOC_TYPE_TAG(kstate, KonTable, KN_T_TABLE);
    value->Table = builder->Table;
    tb_free(builder);
    return value;
}


KonBuilder* CreateKvPairBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = KN_BUILDER_KV_PAIR;
    builder->KvPair.Key = KxStringBuffer_New();
    builder->KvPair.Value = KN_UNDEF;
    return builder;
}

void KvPairSetKey(KonBuilder* builder, char* key)
{
    assert(key);
    KxStringBuffer_AppendCstr(builder->KvPair.Key, key);
}

void KvPairSetValue(KonBuilder* builder, KN value)
{
    assert(value);
    builder->KvPair.Value = value;
}

void KvPairDestroy(KonBuilder* builder)
{
    tb_free(builder);
}

KonBuilder* MakeKvPairBuilder(KonBuilder* builder, KN value)
{
    builder->KvPair.Value = value;
    return builder;
}

CellBuilderItem* CreateCellBuilderItem()
{
    CellBuilderItem* cellItem = (CellBuilderItem*)tb_malloc(sizeof(CellBuilderItem));
    
    cellItem->Core = KN_UNDEF;
    cellItem->Vector = KN_UNDEF;
    cellItem->Table = KN_UNDEF;
    cellItem->List = KN_UNDEF;
    cellItem->Map = KxHashTable_Init(4);
    return cellItem;
}

KonBuilder* CreateCellBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }

    builder->Type = KN_BUILDER_CELL;
    builder->Cell = KxVector_Init();

    CellBuilderItem* cellItem = CreateCellBuilderItem();

    KxVector_Push(builder->Cell, cellItem);

    return builder;
}

// create new cell section in CellBuilder
// 1 core is set, meet next core
// 2 table is set, meet next core or table
// 3 list is set, meet next core or table or list

void CellBuilderSetCore(KonBuilder* builder, KN name)
{
    KN_DEBUG("CellBuilderSetCore");
    CellBuilderItem* cellItem = (CellBuilderItem*)KxVector_Tail(builder->Cell);
    if (cellItem->Core != KN_UNDEF
        || cellItem->Table != KN_UNDEF
        || cellItem->List != KN_UNDEF
    ) {
        CellBuilderItem* newCellItem = CreateCellBuilderItem();
        KxVector_Push(builder->Cell, newCellItem);
        cellItem = newCellItem;
    }
    cellItem->Core = name;
}

void CellBuilderSetVector(KonBuilder* builder, KN vector)
{
    CellBuilderItem* cellItem = (CellBuilderItem*)KxVector_Tail(builder->Cell);

    cellItem->Vector = vector;
}

void CellBuilderSetList(KonBuilder* builder, KN list)
{
    CellBuilderItem* cellItem = (CellBuilderItem*)KxVector_Tail(builder->Cell);
    if (cellItem->List != KN_UNDEF) {
        CellBuilderItem* newCellItem = CreateCellBuilderItem();
        KxVector_Push(builder->Cell, newCellItem);
        cellItem = newCellItem;
    }
    cellItem->List = list;
}

void CellBuilderSetTable(KonBuilder* builder, KN table)
{
    CellBuilderItem* cellItem = (CellBuilderItem*)KxVector_Tail(builder->Cell);
    if (cellItem->Table != KN_UNDEF
        || cellItem->List != KN_UNDEF
    ) {
        CellBuilderItem* newCellItem = CreateCellBuilderItem();
        KxVector_Push(builder->Cell, newCellItem);
        cellItem = newCellItem;
    }
    cellItem->Table = table;
}

void CellBuilderAddPair(KonBuilder* builder, KonBuilder* pair)
{
    CellBuilderItem* cellItem = (CellBuilderItem*)KxVector_Tail(builder->Cell);

    char* key = KxStringBuffer_Cstr(pair->KvPair.Key);
    
    KxHashTable_PutKv(cellItem->Map, key, pair->KvPair.Value);
    KN_DEBUG("CellBuilderAddPair before free pair builder key %s", key);
    tb_free(pair);
}

KonCell* CreateNewKonCellNode(KonState* kstate, CellBuilderItem* cellItem)
{
    KonCell* value = KN_ALLOC_TYPE_TAG(kstate, KonCell, KN_T_CELL);
    value->Core = cellItem->Core;
    value->Vector = cellItem->Vector;
    value->Table = cellItem->Table;
    value->List = cellItem->List;
    value->Map = cellItem->Map;
    value->Next = KN_NIL;
    value->Prev = KN_NIL;
    return value;
}

KN MakeCellByBuilder(KonState* kstate, KonBuilder* builder)
{
    KonVector* cell = builder->Cell;

    KonCell* currentHead = KN_NIL;
    // reverse add
    int len = KxVector_Length(cell);
    for (int i = len - 1; i >= 0; i--) {
        CellBuilderItem* cellBuilderItem = (CellBuilderItem*)KxVector_AtIndex(cell, i);
        KonCell* newNode = CreateNewKonCellNode(kstate, cellBuilderItem);
        newNode->Next = currentHead;
        if (currentHead != KN_NIL) {
            currentHead->Prev = newNode;
        }
        currentHead = newNode;
    }


    tb_free(builder);
    return currentHead;
}

KonBuilder* CreateWrapperBuilder(KonBuilderType type, KonTokenKind tokenKind)
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->Type = type;
    builder->Wrapper.Inner = KN_UNDEF;
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
    KN result = KN_UNDEF;
    if (type == KN_BUILDER_QUOTE) {
        KonQuote* tmp = KN_ALLOC_TYPE_TAG(kstate, KonQuote, KN_T_QUOTE);
        tmp->Inner = inner;
        switch (tokenKind) {
            case KN_TOKEN_SYM_STRING: {
                tmp->Type = KN_SYM_STRING;
                break;
            }
            case KN_TOKEN_QUOTE_LIST: {
                tmp->Type = KN_QUOTE_LIST;
                break;
            }
            case KN_TOKEN_QUOTE_CELL: {
                tmp->Type = KN_QUOTE_CELL;
                break;
            }
        }
        result = tmp;
    }
    else if (type == KN_BUILDER_QUASIQUOTE) {
        KonQuasiquote* tmp = KN_ALLOC_TYPE_TAG(kstate, KonQuasiquote, KN_T_QUASIQUOTE);
        tmp->Inner = inner;
        switch (tokenKind) {
            case KN_TOKEN_QUASI_LIST: {
                tmp->Type = KN_QUASI_LIST;
                break;
            }
            case KN_TOKEN_QUASI_CELL: {
                tmp->Type = KN_QUASI_CELL;
                break;
            }
        }
        result = tmp;
    }
    else if (type == KN_BUILDER_EXPAND) {
        KonExpand* tmp = KN_ALLOC_TYPE_TAG(kstate, KonExpand, KN_T_EXPAND);
        tmp->Inner = inner;
        switch (tokenKind) {
            case KN_TOKEN_EXPAND_REPLACE: {
                tmp->Type = KN_EXPAND_REPLACE;
                break;
            }
            case KN_TOKEN_EXPAND_KV: {
                tmp->Type = KN_EXPAND_KV;
                break;
            }
            case KN_TOKEN_EXPAND_SEQ: {
                tmp->Type = KN_EXPAND_SEQ;
                break;
            }
        }
        result = tmp;
    }
    else if (type == KN_BUILDER_UNQUOTE) {
        KonUnquote* tmp = KN_ALLOC_TYPE_TAG(kstate, KonUnquote, KN_T_UNQUOTE);
        tmp->Inner = inner;
        switch (tokenKind) {
            case KN_TOKEN_UNQUOTE_REPLACE: {
                tmp->Type = KN_UNQUOTE_REPLACE;
                break;
            }
            case KN_TOKEN_UNQUOTE_KV: {
                tmp->Type = KN_UNQUOTE_KV;
                break;
            }
            case KN_TOKEN_UNQUOTE_SEQ: {
                tmp->Type = KN_UNQUOTE_SEQ;
                break;
            }
        }
        result = tmp;
    }
    tb_free(builder);
    return result;
}

// builder stack
BuilderStack* BuilderStackInit()
{
    BuilderStack* stack = (BuilderStack*)tb_malloc(sizeof(BuilderStack));
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
        tb_free(oldTop);
        top = top->Next;
    }
    tb_free(stack);
}

void BuilderStackPush(BuilderStack* stack, KonBuilder* item)
{
    assert(stack);
    BuilderStackNode* oldTop = stack->Top;
    BuilderStackNode* newTop = (BuilderStackNode*)tb_malloc(sizeof(BuilderStackNode));
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

    tb_free(top);
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


