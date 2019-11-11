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
    KxVector_Push(builder->vector, item.asU64);
}

KN MakeVectorByBuilder(Kana* kana, KonBuilder* builder)
{
    KonVector* value = KN_NEW_CONST_OBJ(kana, KonVector, KN_T_VECTOR);
    value->vector = builder->vector;
    tb_free(builder);

    return KON_2_KN(value);
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
    KxVector_Push(builder->list, item.asU64);
}

KN MakeListByBuilder(Kana* kana, KonBuilder* builder)
{
    KN pair = BOXED_KN_NIL;
    
    KxVector* list = builder->list;

    // reverse add
    int len = KxVector_Length(list);
    for (int i = len - 1; i >= 0; i--) {
        KN item = (KN)KxVector_AtIndex(list, i);
        pair = KN_CONS(kana, item, pair);
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
    KxVector_Push(builder->block, item.asU64);
}

KN MakeBlockByBuilder(Kana* kana, KonBuilder* builder)
{
    KN pair = BOXED_KN_NIL;
    
    KonVector* list = builder->block;

    // reverse add
    int len = KxVector_Length(list);
    for (int i = len - 1; i >= 0; i--) {
        KN item = (KN)KxVector_AtIndex(list, i);
        pair = KN_CONS(kana, item, pair);
    }
    if (len > 0) {
        // change first element tag to BLOCK
        KN_FIELD(pair, Base, tag) = KN_T_BLOCK;
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
    
    KxHashTable_PushKv(builder->param, key, pair->kvPair.value.asU64);
    KN_DEBUG("TableBuilderAddPair before free pair builder key %s", key);
    tb_free(pair);
}

void ParamBuilderAddValue(KonBuilder* builder, KN value)
{
    KxHashTable_PushVal(builder->param, value.asU64);
}

KN MakeParamByBuilder(Kana* kana, KonBuilder* builder)
{
    KonParam* value = KN_NEW_CONST_OBJ(kana, KonTable, KN_T_PARAM);
    value->table = builder->param;
    tb_free(builder);
    return KON_2_KN(value);
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
    
    KxHashTable_PushKv(builder->table, key, pair->kvPair.value.asU64);
    KN_DEBUG("TableBuilderAddPair before free pair builder key %s", key);
    tb_free(pair);
}

void TableBuilderAddValue(KonBuilder* builder, KN value)
{
    KxHashTable_PushVal(builder->table, value.asU64);
}

KN MakeTableByBuilder(Kana* kana, KonBuilder* builder)
{
    KonTable* value = KN_NEW_CONST_OBJ(kana, KonTable, KN_T_TABLE);
    value->table = builder->table;
    tb_free(builder);
    return KON_2_KN(value);
}


KonBuilder* CreateKvPairBuilder()
{
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->type = KN_BUILDER_KV_PAIR;
    builder->kvPair.key = KxStringBuffer_New();
    builder->kvPair.value = (KN)KN_UNDEF;
    return builder;
}

void KvPairSetKey(KonBuilder* builder, char* key)
{
    assert(key);
    KxStringBuffer_AppendCstr(builder->kvPair.key, key);
}

void KvPairSetValue(KonBuilder* builder, KN value)
{
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

void MapBuilderAddPair(Kana* kana, KonBuilder* builder, KonBuilder* pair)
{
    char* key = KxStringBuffer_Cstr(pair->kvPair.key);
    KxHashTable* unboxedMap = builder->map;
    KxHashTable_PutKv(unboxedMap, key, pair->kvPair.value.asU64);
    KN_DEBUG("MapBuilderAddPair before free pair builder key %s", key);
    tb_free(pair);
}

KN MakeMapByBuilder(Kana* kana, KonBuilder* builder)
{
    KonMap* value = KN_NEW_CONST_OBJ(kana, KonMap, KN_T_MAP);
    value->map = builder->map;
    tb_free(builder);
    return KON_2_KN(value);
}

CellBuilderItem* CreateCellBuilderItem()
{
    CellBuilderItem* cellItem = (CellBuilderItem*)tb_malloc(sizeof(CellBuilderItem));
    
    cellItem->core = (KN)KN_UNDEF;
    cellItem->table = (KN)KN_UNDEF;
    cellItem->list = (KN)KN_UNDEF;
    cellItem->map = (KN)KN_UNDEF;
    cellItem->vector = (KN)KN_UNDEF;
    cellItem->suffix = (KN)KN_UNDEF;
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
    if (cellItem->core.asU64 != KNBOX_UNDEF
        || cellItem->table.asU64 != KNBOX_UNDEF
        || cellItem->list.asU64 != KNBOX_UNDEF
        || cellItem->vector.asU64 != KNBOX_UNDEF
        || cellItem->suffix.asU64 != KNBOX_UNDEF
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
    if (cellItem->list.asU64 != KNBOX_UNDEF) {
        CellBuilderItem* newCellItem = CreateCellBuilderItem();
        KxVector_Push(builder->cell, newCellItem);
        cellItem = newCellItem;
    }
    cellItem->list = list;
}

void CellBuilderSetTable(KonBuilder* builder, KN table)
{
    CellBuilderItem* cellItem = (CellBuilderItem*)KxVector_Tail(builder->cell);
    if (cellItem->table.asU64 != KNBOX_UNDEF
        || cellItem->list.asU64 != KNBOX_UNDEF
    ) {
        CellBuilderItem* newCellItem = CreateCellBuilderItem();
        KxVector_Push(builder->cell, newCellItem);
        cellItem = newCellItem;
    }
    cellItem->table = table;
}

void CellBuilderSetVector(KonBuilder* builder, KN vector)
{
    CellBuilderItem* cellItem = (CellBuilderItem*)KxVector_Tail(builder->cell);
    if (cellItem->vector.asU64 != KNBOX_UNDEF
        || cellItem->vector.asU64 != KNBOX_UNDEF
    ) {
        CellBuilderItem* newCellItem = CreateCellBuilderItem();
        KxVector_Push(builder->cell, newCellItem);
        cellItem = newCellItem;
    }
    cellItem->vector = vector;
}


void CellBuilderSetSuffix(KonBuilder* builder, KN suffix)
{
    CellBuilderItem* cellItem = (CellBuilderItem*)KxVector_Tail(builder->cell);
    if (cellItem->suffix.asU64 != KNBOX_UNDEF
        || cellItem->suffix.asU64 != KNBOX_UNDEF
    ) {
        CellBuilderItem* newCellItem = CreateCellBuilderItem();
        KxVector_Push(builder->cell, newCellItem);
        cellItem = newCellItem;
    }
    cellItem->suffix = suffix;
}

void CellBuilderAddPair(Kana* kana, KonBuilder* builder, KonBuilder* pair)
{
    CellBuilderItem* cellItem = (CellBuilderItem*)KxVector_Tail(builder->cell);
    if (cellItem->map.asU64 == KNBOX_UNDEF) {
        KonMap* newMap = KN_NEW_CONST_OBJ(kana, KonMap, KN_T_MAP);
        newMap->map = KxHashTable_Init(4);
        cellItem->map = KON_2_KN(newMap);
    }

    char* key = KxStringBuffer_Cstr(pair->kvPair.key);
    KxHashTable* unboxedMap = KN_FIELD(cellItem->map, Map, map);
    KxHashTable_PutKv(unboxedMap, key, pair->kvPair.value.asU64);
    KN_DEBUG("CellBuilderAddPair before free pair builder key %s", key);
    tb_free(pair);
}

KonCell* CreateNewKonCellNode(Kana* kana, CellBuilderItem* cellItem)
{
    KonCell* value = KN_NEW_CONST_OBJ(kana, KonCell, KN_T_CELL);
    value->core = cellItem->core;
    value->table = KN_2_KON(cellItem->table, Table);
    value->list = KN_2_KON(cellItem->list, Pair);
    value->map = KN_2_KON(cellItem->map, Map);
    value->vector = KN_2_KON(cellItem->vector, Vector);
    value->suffix = KN_2_KON(cellItem->suffix, Suffix);
    value->next = KN_2_KON(KN_NIL, Cell);
    value->prev = KN_2_KON(KN_NIL, Cell);
    return value;
}

KN MakeCellByBuilder(Kana* kana, KonBuilder* builder)
{
    KonVector* cell = builder->cell;

    KonCell* currentHead = KN_2_KON(KN_NIL, Cell);
    // reverse add
    int len = KxVector_Length(cell);
    for (int i = len - 1; i >= 0; i--) {
        CellBuilderItem* cellBuilderItem = (CellBuilderItem*)KxVector_AtIndex(cell, i);
        KonCell* newNode = CreateNewKonCellNode(kana, cellBuilderItem);
        newNode->next = currentHead;
        if (currentHead != KNBOX_NIL) {
            currentHead->prev = newNode;
        }
        currentHead = newNode;
    }


    tb_free(builder);
    return KON_2_KN(currentHead);
}

KonBuilder* CreateWrapperBuilder(KonBuilderType type, KonTokenizer* tokenizer, Kana* kana)
{
    KonTokenKind tokenKind = tokenizer->tokenKind;
    KonBuilder* builder = (KonBuilder*)tb_malloc(sizeof(KonBuilder));
    if (builder == NULL) {
        return NULL;
    }
    builder->type = type;
    builder->wrapper.inner = KN_UNDEF;
    builder->wrapper.tokenKind = tokenKind;
    KN name;
    if (KxStringBuffer_Length(tokenizer->content) <= 5) {
        const char* nameCstr = KxStringBuffer_Cstr(tokenizer->content);
        name = KN_BOX_SHORT_STR(nameCstr);
    }
    else {
        KonString* value = KN_NEW_CONST_OBJ(kana, KonString, KN_T_STRING);
        value->string = KxStringBuffer_New();
        KxStringBuffer_AppendCstr(value->string, KxStringBuffer_Cstr(tokenizer->content));
        name = KON_2_KN(value);
    }
    
    builder->wrapper.name = name;
    return builder;
}

void WrapperSetInner(Kana* kana, KonBuilder* builder, KN inner)
{
    builder->wrapper.inner = inner;
}

KN MakeWrapperByBuilder(Kana* kana, KonBuilder* builder)
{
    KN inner = builder->wrapper.inner;
    KonBuilderType type = builder->type;
    KonTokenKind tokenKind = builder->wrapper.tokenKind;
    KN result = KN_UNDEF;
    if (type == KN_BUILDER_QUOTE) {
        KonQuote* tmp = KN_NEW_CONST_OBJ(kana, KonQuote, KN_T_QUOTE);
        tmp->inner = inner;
        if (KN_IS_CELL(inner)) {
            tmp->type = KN_QUOTE_CELL;
        }
        else if (KN_IsPairList(inner)) {
             tmp->type = KN_QUOTE_LIST;
        }
        else {
            // TODO
            tmp->type = KN_SYM_STRING;
        }
        tmp->name = builder->wrapper.name;
        result = KON_2_KN(tmp);
    }
    else if (type == KN_BUILDER_QUASIQUOTE) {
        KonQuasiquote* tmp = KN_NEW_CONST_OBJ(kana, KonQuasiquote, KN_T_QUASIQUOTE);
        tmp->inner = inner;
        if (KN_IS_CELL(inner)) {
            tmp->type = KN_QUASI_CELL;
        }
        else if (KN_IsPairList(inner)) {
             tmp->type = KN_QUASI_LIST;
        }
        else {
            // TODO
            tmp->type = KN_SYM_STRING;
        }
        tmp->name = builder->wrapper.name;
        result = KON_2_KN(tmp);
    }
    else if (type == KN_BUILDER_UNQUOTE) {
        KonUnquote* tmp = KN_NEW_CONST_OBJ(kana, KonUnquote, KN_T_UNQUOTE);
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
        result = KON_2_KN(tmp);
    }
    else if (type == KN_BUILDER_PREFIX) {
        KonPrefix* tmp = KN_NEW_CONST_OBJ(kana, KonPrefix, KN_T_PREFIX);
        tmp->inner = inner;
        result = KON_2_KN(tmp);
    }
    else if (type == KN_BUILDER_SUFFIX) {
        KonSuffix* tmp = KN_NEW_CONST_OBJ(kana, KonSuffix, KN_T_SUFFIX);
        tmp->inner = inner;
        result = KON_2_KN(tmp);
    }
    else if (type == KN_BUILDER_TXT_MARCRO) {
        KonTxtMarcro* tmp = KN_NEW_CONST_OBJ(kana, KonTxtMarcro, KN_T_TXT_MARCRO);
        tmp->inner = inner;
        tmp->name = builder->wrapper.name;
        result = KON_2_KN(tmp);
    }
    else if (type == KN_BUILDER_OBJ_BUILDER) {
        KonObjBuilder* tmp = KN_NEW_CONST_OBJ(kana, KonObjBuilder, KN_T_OBJ_BUILDER);
        tmp->inner = inner;
        tmp->name = builder->wrapper.name;
        result = KON_2_KN(tmp);
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


