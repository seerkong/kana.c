#include "value_builder.h"
#include "../../lib/tbox/tbox.h"

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
        case KN_BUILDER_MAP: {
            return "KN_BUILDER_MAP";
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
    builder->type = KN_BUILDER_VECTOR;
    builder->vector = KxVector_Init();
    return builder;
}

void VectorBuilderAddItem(KonBuilder* builder, KN item)
{
    KxVector_Push(builder->vector, item);
}

KN MakeVectorByBuilder(KonState* kstate, KonBuilder* builder)
{
    KonVector* value = KN_ALLOC_TYPE_TAG(kstate, KonVector, KN_T_VECTOR);
    value->vector = builder->vector;
    tb_free(builder);
    return value;
}

KonBuilder* CreateListBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->type = KN_BUILDER_LIST;
    builder->list = KxVector_Init();
    return builder;
}

void ListBuilderAddItem(KonBuilder* builder, KN item)
{
    KxVector_Push(builder->list, item);
}

KN MakeListByBuilder(KonState* kstate, KonBuilder* builder)
{
    KN pair = KN_NIL;
    
    KonVector* list = builder->list;

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
    builder->type = KN_BUILDER_BLOCK;
    builder->block = KxVector_Init();
    return builder;
}

void BlockBuilderAddItem(KonBuilder* builder, KN item)
{
    KxVector_Push(builder->block, item);
}

KN MakeBlockByBuilder(KonState* kstate, KonBuilder* builder)
{
    KN pair = KN_NIL;
    
    KonVector* list = builder->block;

    // reverse add
    int len = KxVector_Length(list);
    for (int i = len - 1; i >= 0; i--) {
        KN item = KxVector_AtIndex(list, i);
        pair = KN_CONS(kstate, item, pair);
    }
    if (len > 0) {
        // change first element tag to BLOCK
        ((KonBase*)pair)->tag = KN_T_BLOCK;
    }

    return pair;
}

KonBuilder* CreateParamBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->type = KN_BUILDER_PARAM;
    builder->param = KxHashTable_Init(10);;
    return builder;
}

void ParamBuilderAddPair(KonBuilder* builder, KonBuilder* pair)
{
    char* key = KxStringBuffer_Cstr(pair->kvPair.key);
    
    KxHashTable_PushKv(builder->param, key, pair->kvPair.value);
    KN_DEBUG("TableBuilderAddPair before free pair builder key %s", key);
    tb_free(pair);
}

void ParamBuilderAddValue(KonBuilder* builder, KN value)
{
    KxHashTable_PushVal(builder->param, value);
}

KN MakeParamByBuilder(KonState* kstate, KonBuilder* builder)
{
    KonParam* value = KN_ALLOC_TYPE_TAG(kstate, KonTable, KN_T_PARAM);
    value->table = builder->param;
    tb_free(builder);
    return value;
}


KonBuilder* CreateTableBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->type = KN_BUILDER_TABLE;
    builder->table = KxHashTable_Init(10);;
    return builder;
}

void TableBuilderAddPair(KonBuilder* builder, KonBuilder* pair)
{
    char* key = KxStringBuffer_Cstr(pair->kvPair.key);
    
    KxHashTable_PushKv(builder->table, key, pair->kvPair.value);
    KN_DEBUG("TableBuilderAddPair before free pair builder key %s", key);
    tb_free(pair);
}

void TableBuilderAddValue(KonBuilder* builder, KN value)
{
    KxHashTable_PushVal(builder->table, value);
}

KN MakeTableByBuilder(KonState* kstate, KonBuilder* builder)
{
    KonTable* value = KN_ALLOC_TYPE_TAG(kstate, KonTable, KN_T_TABLE);
    value->table = builder->table;
    tb_free(builder);
    return value;
}


KonBuilder* CreateKvPairBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->type = KN_BUILDER_KV_PAIR;
    builder->kvPair.key = KxStringBuffer_New();
    builder->kvPair.value = KN_UNDEF;
    return builder;
}

void KvPairSetKey(KonBuilder* builder, char* key)
{
    assert(key);
    KxStringBuffer_AppendCstr(builder->kvPair.key, key);
}

void KvPairSetValue(KonBuilder* builder, KN value)
{
    assert(value);
    builder->kvPair.value = value;
}

void KvPairDestroy(KonBuilder* builder)
{
    tb_free(builder);
}

KonBuilder* MakeKvPairBuilder(KonBuilder* builder, KN value)
{
    builder->kvPair.value = value;
    return builder;
}

KonBuilder* CreateMapBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->type = KN_BUILDER_MAP;
    builder->map = KxHashTable_Init(10);;
    return builder;
}

void MapBuilderAddPair(KonState* kstate, KonBuilder* builder, KonBuilder* pair)
{
    char* key = KxStringBuffer_Cstr(pair->kvPair.key);
    KxHashTable* unboxedMap = builder->map;
    KxHashTable_PutKv(unboxedMap, key, pair->kvPair.value);
    KN_DEBUG("MapBuilderAddPair before free pair builder key %s", key);
    tb_free(pair);
}

KN MakeMapByBuilder(KonState* kstate, KonBuilder* builder)
{
    KonMap* value = KN_ALLOC_TYPE_TAG(kstate, KonMap, KN_T_MAP);
    value->map = builder->map;
    tb_free(builder);
    return value;
}

CellBuilderItem* CreateCellBuilderItem()
{
    CellBuilderItem* cellItem = (CellBuilderItem*)tb_malloc(sizeof(CellBuilderItem));
    
    cellItem->core = KN_UNDEF;
    cellItem->table = KN_UNDEF;
    cellItem->list = KN_UNDEF;
    cellItem->map = KN_UNDEF;
    return cellItem;
}

KonBuilder* CreateCellBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }

    builder->type = KN_BUILDER_CELL;
    builder->cell = KxVector_Init();

    CellBuilderItem* cellItem = CreateCellBuilderItem();

    KxVector_Push(builder->cell, cellItem);

    return builder;
}

// create new cell section in CellBuilder
// 1 core is set, meet next core
// 2 table is set, meet next core or table
// 3 list is set, meet next core or table or list

void CellBuilderSetCore(KonBuilder* builder, KN name)
{
    KN_DEBUG("CellBuilderSetCore");
    CellBuilderItem* cellItem = (CellBuilderItem*)KxVector_Tail(builder->cell);
    if (cellItem->core != KN_UNDEF
        || cellItem->table != KN_UNDEF
        || cellItem->list != KN_UNDEF
    ) {
        CellBuilderItem* newCellItem = CreateCellBuilderItem();
        KxVector_Push(builder->cell, newCellItem);
        cellItem = newCellItem;
    }
    cellItem->core = name;
}

void CellBuilderSetList(KonBuilder* builder, KN list)
{
    CellBuilderItem* cellItem = (CellBuilderItem*)KxVector_Tail(builder->cell);
    if (cellItem->list != KN_UNDEF) {
        CellBuilderItem* newCellItem = CreateCellBuilderItem();
        KxVector_Push(builder->cell, newCellItem);
        cellItem = newCellItem;
    }
    cellItem->list = list;
}

void CellBuilderSetTable(KonBuilder* builder, KN table)
{
    CellBuilderItem* cellItem = (CellBuilderItem*)KxVector_Tail(builder->cell);
    if (cellItem->table != KN_UNDEF
        || cellItem->list != KN_UNDEF
    ) {
        CellBuilderItem* newCellItem = CreateCellBuilderItem();
        KxVector_Push(builder->cell, newCellItem);
        cellItem = newCellItem;
    }
    cellItem->table = table;
}

void CellBuilderAddPair(KonState* kstate, KonBuilder* builder, KonBuilder* pair)
{
    CellBuilderItem* cellItem = (CellBuilderItem*)KxVector_Tail(builder->cell);
    if (cellItem->map == KN_UNDEF) {
        KonMap* newMap = KN_ALLOC_TYPE_TAG(kstate, KonMap, KN_T_MAP);
        newMap->map = KxHashTable_Init(4);
        cellItem->map = newMap;
    }

    char* key = KxStringBuffer_Cstr(pair->kvPair.key);
    KxHashTable* unboxedMap = CAST_Kon(Map, cellItem->map)->map;
    KxHashTable_PutKv(unboxedMap, key, pair->kvPair.value);
    KN_DEBUG("CellBuilderAddPair before free pair builder key %s", key);
    tb_free(pair);
}

KonCell* CreateNewKonCellNode(KonState* kstate, CellBuilderItem* cellItem)
{
    KonCell* value = KN_ALLOC_TYPE_TAG(kstate, KonCell, KN_T_CELL);
    value->core = cellItem->core;
    value->table = cellItem->table;
    value->list = cellItem->list;
    value->map = cellItem->map;
    value->next = KN_NIL;
    value->prev = KN_NIL;
    return value;
}

KN MakeCellByBuilder(KonState* kstate, KonBuilder* builder)
{
    KonVector* cell = builder->cell;

    KonCell* currentHead = KN_NIL;
    // reverse add
    int len = KxVector_Length(cell);
    for (int i = len - 1; i >= 0; i--) {
        CellBuilderItem* cellBuilderItem = (CellBuilderItem*)KxVector_AtIndex(cell, i);
        KonCell* newNode = CreateNewKonCellNode(kstate, cellBuilderItem);
        newNode->next = currentHead;
        if (currentHead != KN_NIL) {
            currentHead->prev = newNode;
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
    builder->type = type;
    builder->wrapper.inner = KN_UNDEF;
    builder->wrapper.tokenKind = tokenKind;
    return builder;
}

void WrapperSetInner(KonState* kstate, KonBuilder* builder, KN inner)
{
    builder->wrapper.inner = inner;
}

KN MakeWrapperByBuilder(KonState* kstate, KonBuilder* builder)
{
    KN inner = builder->wrapper.inner;
    KonBuilderType type = builder->type;
    KonTokenKind tokenKind = builder->wrapper.tokenKind;
    KN result = KN_UNDEF;
    if (type == KN_BUILDER_QUOTE) {
        KonQuote* tmp = KN_ALLOC_TYPE_TAG(kstate, KonQuote, KN_T_QUOTE);
        tmp->inner = inner;
        switch (tokenKind) {
            case KN_TOKEN_SYM_STRING: {
                tmp->type = KN_SYM_STRING;
                break;
            }
            case KN_TOKEN_QUOTE_LIST: {
                tmp->type = KN_QUOTE_LIST;
                break;
            }
            case KN_TOKEN_QUOTE_CELL: {
                tmp->type = KN_QUOTE_CELL;
                break;
            }
        }
        result = tmp;
    }
    else if (type == KN_BUILDER_QUASIQUOTE) {
        KonQuasiquote* tmp = KN_ALLOC_TYPE_TAG(kstate, KonQuasiquote, KN_T_QUASIQUOTE);
        tmp->inner = inner;
        switch (tokenKind) {
            case KN_TOKEN_QUASI_LIST: {
                tmp->type = KN_QUASI_LIST;
                break;
            }
            case KN_TOKEN_QUASI_CELL: {
                tmp->type = KN_QUASI_CELL;
                break;
            }
        }
        result = tmp;
    }
    else if (type == KN_BUILDER_EXPAND) {
        KonExpand* tmp = KN_ALLOC_TYPE_TAG(kstate, KonExpand, KN_T_EXPAND);
        tmp->inner = inner;
        switch (tokenKind) {
            case KN_TOKEN_EXPAND_REPLACE: {
                tmp->type = KN_EXPAND_REPLACE;
                break;
            }
            case KN_TOKEN_EXPAND_KV: {
                tmp->type = KN_EXPAND_KV;
                break;
            }
            case KN_TOKEN_EXPAND_SEQ: {
                tmp->type = KN_EXPAND_SEQ;
                break;
            }
        }
        result = tmp;
    }
    else if (type == KN_BUILDER_UNQUOTE) {
        KonUnquote* tmp = KN_ALLOC_TYPE_TAG(kstate, KonUnquote, KN_T_UNQUOTE);
        tmp->inner = inner;
        switch (tokenKind) {
            case KN_TOKEN_UNQUOTE_REPLACE: {
                tmp->type = KN_UNQUOTE_REPLACE;
                break;
            }
            case KN_TOKEN_UNQUOTE_KV: {
                tmp->type = KN_UNQUOTE_KV;
                break;
            }
            case KN_TOKEN_UNQUOTE_SEQ: {
                tmp->type = KN_UNQUOTE_SEQ;
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
    stack->length = 0;
    stack->top = NULL;
    return stack;
}

void BuilderStackDestroy(BuilderStack* stack)
{
    // TODO iter node, free
    BuilderStackNode* top = stack->top;
    while (top) {
        BuilderStackNode* oldTop = top;
        tb_free(oldTop);
        top = top->next;
    }
    tb_free(stack);
}

void BuilderStackPush(BuilderStack* stack, KonBuilder* item)
{
    assert(stack);
    BuilderStackNode* oldTop = stack->top;
    BuilderStackNode* newTop = (BuilderStackNode*)tb_malloc(sizeof(BuilderStackNode));
    assert(newTop);
    newTop->data = item;
    newTop->next = oldTop;
    stack->top = newTop;
    stack->length = stack->length + 1;
}

KonBuilder* BuilderStackPop(BuilderStack* stack)
{
    assert(stack);
    BuilderStackNode* top = stack->top;
    assert(top);
    BuilderStackNode* next = stack->top->next;

    KonBuilder* data = top->data;
    stack->top = next;
    stack->length = stack->length - 1;

    tb_free(top);
    return data;
}

long BuilderStackLength(BuilderStack* stack)
{
    assert(stack);
    return stack->length;
}

KonBuilder* BuilderStackTop(BuilderStack* stack)
{
    assert(stack);
    BuilderStackNode* top = stack->top;
    assert(top);
    return top->data;
}


