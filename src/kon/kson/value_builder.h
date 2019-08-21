#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "prefix.h"
#include "node.h"
#include "tokenizer.h"

typedef enum {
    KON_BUILDER_VECTOR = 1,
    KON_BUILDER_LIST,
    KON_BUILDER_TABLE,
    KON_BUILDER_KV_PAIR,
    KON_BUILDER_PARAM,
    KON_BUILDER_BLOCK,
    KON_BUILDER_CELL,

    KON_BUILDER_QUOTE,
    KON_BUILDER_QUASIQUOTE,
    KON_BUILDER_EXPAND,
    KON_BUILDER_UNQUOTE,
} KonBuilderType;

typedef struct {
    KN Core;
    KN Vector; // vector of KN
    KN Table;
    KxHashTable* Map;
    KN List; // vector of KN
} CellBuilderItem;

typedef struct {
    KonBuilderType Type;
    union {
        KxVector* Vector; // vector of KN
        KxVector* List; // vector of KN
        KxVector* Block;
        KxHashTable* Param;
        KxHashTable* Table;
        struct {
            KxStringBuffer* Key;
            KN Value;
        } KvPair;

        KxVector* Cell; // vector of CellBuilderItem*

        struct {
            KN Inner;
            KonTokenKind TokenKind;
        } Wrapper;
    };
} KonBuilder;

typedef struct _BuilderStackNode {
    KonBuilder* Data;
    struct _BuilderStackNode *Next;
} BuilderStackNode;

typedef struct _BuilderStack {
    long Length;
    BuilderStackNode* Top;
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

KonBuilder* CreateCellBuilder();
void CellBuilderSetCore(KonBuilder* builder, KN name);
void CellBuilderSetVector(KonBuilder* builder, KN vector);
void CellBuilderSetList(KonBuilder* builder, KN list);
void CellBuilderSetTable(KonBuilder* builder, KN table);
void CellBuilderAddPair(KonBuilder* builder, KonBuilder* pair);
KN MakeCellByBuilder(KonState* kstate, KonBuilder* builder);

KonBuilder* CreateWrapperBuilder(KonBuilderType type, KonTokenKind tokenKind);
void WrapperSetInner(KonState* kstate, KonBuilder* builder, KN inner);
KN MakeWrapperByBuilder(KonState* kstate, KonBuilder* builder);

BuilderStack* BuilderStackInit();
void BuilderStackDestroy(BuilderStack* stack);
void BuilderStackPush(BuilderStack* stack, KonBuilder* item);
KonBuilder* BuilderStackPop(BuilderStack* stack);
long BuilderStackLength(BuilderStack* stack);
KonBuilder* BuilderStackTop(BuilderStack* stack);
