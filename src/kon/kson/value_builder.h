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
    KON_BUILDER_TABLE_PAIR,
    KON_BUILDER_CELL,

    KON_BUILDER_QUOTE,
    KON_BUILDER_QUASIQUOTE,
    KON_BUILDER_EXPAND,
    KON_BUILDER_UNQUOTE,
} KonBuilderType;

typedef struct {
    KonBuilderType Type;
    union {
        KxVector* Vector; // vector of KN
        KxVector* List; // vector of KN

        KxHashTable* Table;
        struct {
            KxStringBuffer* Key;
            KN Value;
        } TablePair;

        struct {
            KN Core;
            KN Vector; // vector of KN
            KN Table;  // vector of TablePair*
            KN List; // vector of KN
        } Cell;

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

KonBuilder* CreateTableBuilder();
// add pair and destroy pair
void TableBuilderAddPair(KonBuilder* builder, KonBuilder* pair);
void TableBuilderAddValue(KonBuilder* builder, KN value);
KN MakeTableByBuilder(KonState* kstate, KonBuilder* builder);

KonBuilder* CreateTablePairBuilder();
void TablePairSetKey(KonBuilder* builder, char* key);
void TablePairSetValue(KonBuilder* builder, KN value);
void TablePairDestroy(KonBuilder* builder);

KonBuilder* CreateCellBuilder();
void CellBuilderSetName(KonBuilder* builder, KN name);
void CellBuilderSetVector(KonBuilder* builder, KN vector);
void CellBuilderSetList(KonBuilder* builder, KN list);
void CellBuilderSetTable(KonBuilder* builder, KN table);
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
