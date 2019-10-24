#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "prefix.h"
#include "value.h"
#include "tokenizer.h"

typedef enum {
    KN_BUILDER_VECTOR = 1,
    KN_BUILDER_LIST,
    KN_BUILDER_TABLE,
    KN_BUILDER_KV_PAIR,
    KN_BUILDER_PARAM,
    KN_BUILDER_BLOCK,
    KN_BUILDER_MAP,
    KN_BUILDER_CELL,

    KN_BUILDER_QUOTE,
    KN_BUILDER_QUASIQUOTE,
    KN_BUILDER_UNQUOTE,

    KN_BUILDER_PREFIX,
    KN_BUILDER_SUFFIX,

    KN_BUILDER_TXT_MARCRO,
    KN_BUILDER_OBJ_BUILDER,
} KonBuilderType;

typedef struct {
    KN core;
    KN suffix;
    KN vector;
    KN table;
    KN map;
    KN list; // vector of KN
} CellBuilderItem;

typedef struct {
    KonBuilderType type;
    union {
        KxVector* vector; // vector of KN
        KxVector* list; // vector of KN
        KxVector* block;
        KxHashTable* param;
        KxHashTable* table;
        KxHashTable* map;
        struct {
            KxStringBuffer* key;
            KN value;
        } kvPair;

        KxVector* cell; // vector of CellBuilderItem*

        struct {
            KN name;
            KN inner;
            KonTokenKind tokenKind;
        } wrapper;
    };
} KonBuilder;

typedef struct _BuilderStackNode {
    KonBuilder* data;
    struct _BuilderStackNode *next;
} BuilderStackNode;

typedef struct _BuilderStack {
    long length;
    BuilderStackNode* top;
} BuilderStack;

// create xxx builder
// make xxx by builder

const char* BuilderTypeToCStr(KonBuilderType type);

KonBuilder* CreateVectorBuilder();
void VectorBuilderAddItem(KonBuilder* builder, KN item);
KN MakeVectorByBuilder(KonState* kstate, KonBuilder* builder);

KonBuilder* CreateListBuilder();
void ListBuilderAddItem(KonBuilder* builder, KN item);
KN MakeListByBuilder(KonState* kstate, KonBuilder* builder);

KonBuilder* CreateBlockBuilder();
void BlockBuilderAddItem(KonBuilder* builder, KN item);
KN MakeBlockByBuilder(KonState* kstate, KonBuilder* builder);

KonBuilder* CreateParamBuilder();
// add pair and destroy pair
void ParamBuilderAddPair(KonBuilder* builder, KonBuilder* pair);
void ParamBuilderAddValue(KonBuilder* builder, KN value);
KN MakeParamByBuilder(KonState* kstate, KonBuilder* builder);


KonBuilder* CreateTableBuilder();
// add pair and destroy pair
void TableBuilderAddPair(KonBuilder* builder, KonBuilder* pair);
void TableBuilderAddValue(KonBuilder* builder, KN value);
KN MakeTableByBuilder(KonState* kstate, KonBuilder* builder);

KonBuilder* CreateKvPairBuilder();
void KvPairSetKey(KonBuilder* builder, char* key);
void KvPairSetValue(KonBuilder* builder, KN value);
void KvPairDestroy(KonBuilder* builder);

KonBuilder* CreateMapBuilder();
void MapBuilderAddPair(KonState* kstate, KonBuilder* builder, KonBuilder* pair);
KN MakeMapByBuilder(KonState* kstate, KonBuilder* builder);

KonBuilder* CreateCellBuilder();
void CellBuilderSetCore(KonBuilder* builder, KN name);
void CellBuilderSetList(KonBuilder* builder, KN list);
void CellBuilderSetTable(KonBuilder* builder, KN table);
void CellBuilderAddPair(KonState* kstate, KonBuilder* builder, KonBuilder* pair);
KN MakeCellByBuilder(KonState* kstate, KonBuilder* builder);

KonBuilder* CreateWrapperBuilder(KonBuilderType type, KonTokenizer* tokenizer, KonState* kstate);
void WrapperSetInner(KonState* kstate, KonBuilder* builder, KN inner);
KN MakeWrapperByBuilder(KonState* kstate, KonBuilder* builder);

BuilderStack* BuilderStackInit();
void BuilderStackDestroy(BuilderStack* stack);
void BuilderStackPush(BuilderStack* stack, KonBuilder* item);
KonBuilder* BuilderStackPop(BuilderStack* stack);
long BuilderStackLength(BuilderStack* stack);
KonBuilder* BuilderStackTop(BuilderStack* stack);
