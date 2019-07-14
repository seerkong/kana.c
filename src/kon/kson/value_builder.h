#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "prefix.h"
#include "node.h"
#include "tokenizer.h"

typedef enum {
    KON_BUILDER_VECTOR,
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
        tb_vector_ref_t Vector; // vector of Kon*
        tb_vector_ref_t List; // vector of Kon*

        tb_hash_map_ref_t Table;  // vector of TablePair*
        struct {
            tb_string_t Key;
            Kon* Value;
        } TablePair;

        struct {
            Kon* Name;
            Kon* Vector; // vector of Kon*
            Kon* Table;  // vector of TablePair*
            Kon* List; // vector of Kon*
        } Cell;

        struct {
            Kon* Inner;
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

KonBuilder* CreateVectorBuilder();
void VectorBuilderAddItem(KonBuilder* builder, Kon* item);
Kon* MakeVectorByBuilder(Kon* kstate, KonBuilder* builder);

KonBuilder* CreateListBuilder();
void ListBuilderAddItem(KonBuilder* builder, Kon* item);
Kon* MakeListByBuilder(Kon* kstate, KonBuilder* builder);

KonBuilder* CreateTableBuilder();
// add pair and destroy pair
void TableBuilderAddPair(KonBuilder* builder, KonBuilder* pair);
Kon* MakeTableByBuilder(Kon* kstate, KonBuilder* builder);

KonBuilder* CreateTablePairBuilder();
void TablePairSetKey(KonBuilder* builder, char* key);
void TablePairSetValue(KonBuilder* builder, Kon* value);
void TablePairDestroy(KonBuilder* builder);

KonBuilder* CreateCellBuilder();
void CellBuilderSetName(KonBuilder* builder, Kon* name);
void CellBuilderSetVector(KonBuilder* builder, Kon* vector);
void CellBuilderSetList(KonBuilder* builder, Kon* list);
void CellBuilderSetTable(KonBuilder* builder, Kon* table);
Kon* MakeCellByBuilder(Kon* kstate, KonBuilder* builder);

KonBuilder* CreateWrapperBuilder(KonBuilderType type, KonTokenKind tokenKind);
void WrapperSetInner(Kon* kstate, KonBuilder* builder, Kon* inner);
Kon* MakeWrapperByBuilder(Kon* kstate, KonBuilder* builder);

BuilderStack* BuilderStackInit();
void BuilderStackDestroy(BuilderStack* stack);
void BuilderStackPush(BuilderStack* stack, KonBuilder* item);
KonBuilder* BuilderStackPop(BuilderStack* stack);
long BuilderStackLength(BuilderStack* stack);
KonBuilder* BuilderStackTop(BuilderStack* stack);
