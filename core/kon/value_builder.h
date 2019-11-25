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
        KnVector* vector; // vector of KN
        KnVector* list; // vector of KN
        KnVector* block;
        KnHashTable* param;
        KnHashTable* table;
        KnHashTable* map;
        struct {
            KxStringBuffer* key;
            KN value;
        } kvPair;

        KnVector* cell; // vector of CellBuilderItem*

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
KN MakeVectorByBuilder(Kana* kana, KonBuilder* builder);

KonBuilder* CreateListBuilder();
void ListBuilderAddItem(KonBuilder* builder, KN item);
KN MakeListByBuilder(Kana* kana, KonBuilder* builder);

KonBuilder* CreateBlockBuilder();
void BlockBuilderAddItem(KonBuilder* builder, KN item);
KN MakeBlockByBuilder(Kana* kana, KonBuilder* builder);

KonBuilder* CreateParamBuilder();
// add pair and destroy pair
void ParamBuilderAddPair(KonBuilder* builder, KonBuilder* pair);
void ParamBuilderAddValue(KonBuilder* builder, KN value);
KN MakeParamByBuilder(Kana* kana, KonBuilder* builder);


KonBuilder* CreateTableBuilder();
// add pair and destroy pair
void TableBuilderAddPair(KonBuilder* builder, KonBuilder* pair);
void TableBuilderAddValue(KonBuilder* builder, KN value);
KN MakeTableByBuilder(Kana* kana, KonBuilder* builder);

KonBuilder* CreateKvPairBuilder();
void KvPairSetKey(KonBuilder* builder, char* key);
void KvPairSetValue(KonBuilder* builder, KN value);
void KvPairDestroy(KonBuilder* builder);

KonBuilder* CreateMapBuilder();
void MapBuilderAddPair(Kana* kana, KonBuilder* builder, KonBuilder* pair);
KN MakeMapByBuilder(Kana* kana, KonBuilder* builder);

KonBuilder* CreateCellBuilder();
void CellBuilderSetCore(KonBuilder* builder, KN name);
void CellBuilderSetList(KonBuilder* builder, KN list);
void CellBuilderSetTable(KonBuilder* builder, KN table);
void CellBuilderAddPair(Kana* kana, KonBuilder* builder, KonBuilder* pair);
KN MakeCellByBuilder(Kana* kana, KonBuilder* builder);

KonBuilder* CreateWrapperBuilder(KonBuilderType type, KonTokenizer* tokenizer, Kana* kana);
void WrapperSetInner(Kana* kana, KonBuilder* builder, KN inner);
KN MakeWrapperByBuilder(Kana* kana, KonBuilder* builder);

BuilderStack* BuilderStackInit();
void BuilderStackDestroy(BuilderStack* stack);
void BuilderStackPush(BuilderStack* stack, KonBuilder* item);
KonBuilder* BuilderStackPop(BuilderStack* stack);
long BuilderStackLength(BuilderStack* stack);
KonBuilder* BuilderStackTop(BuilderStack* stack);
