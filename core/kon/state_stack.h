#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


// state
typedef enum
{
    //
    KN_READER_ROOT = 0,
    // parse arr, need value or arr end
    KN_READER_PARSE_VECTOR,
    // parse list, need value or list end
    KN_READER_PARSE_LIST,
    // parse table, need ':' or table end
    KN_READER_PARSE_TABLE,
    // parse table or map key, need identifier, symbol('') or $abc
    KN_READER_PARSE_TABLE_PAIR_KEY,
    KN_READER_PARSE_TABLE_PAIR_EQ,
    // parse table value or next kv pair
    KN_READER_PARSE_TABLE_PAIR_VAL,
    // parse cell, need core, eg: symbol, number str, wrapper, expr
    // or map key :a 1 :b
    // or param, block
    KN_READER_PARSE_CELL_CORE,
    // need map key :a 1 :b
    // or param, block
    // or next
    KN_READER_PARSE_CELL_MAP_PAIR,
    
    KN_READER_PARSE_MAP_PAIR_KEY,

    // need a '=' or ':'
    KN_READER_PARSE_MAP_PAIR_EQ_OR_TAG,
    // parse cell map value or next kv pair
    KN_READER_PARSE_MAP_PAIR_VAL,

    // parse cell inner array, table, or list
    KN_READER_PARSE_CELL_INNER_CONTAINER,

    // parse param table, inner state:KN_READER_PARSE_TABLE_PAIR_KEY KN_READER_PARSE_TABLE_PAIR_VAL
    KN_READER_PARSE_PARAM,
    // parse block list
    KN_READER_PARSE_BLOCK,

    KN_READER_PARSE_PREFIX_WRAPPER,
    KN_READER_PARSE_SUFFIX_WRAPPER,

    KN_READER_PARSE_TXT_MARCRO,
    KN_READER_PARSE_OBJ_BUILDER,

    // parse tpl expr var slot, need identifier or list
    // @. @~. @%.
    KN_READER_PARSE_UNQUOTE,
    // quasiquote container @[ @( @< @{ need [ ( < {
    KN_READER_PARSE_QUASIQUOTE,
    // quote, need identifier or [ ( < { .eg $abc $[ $( $< ${ 
    KN_READER_PARSE_QUOTE,
} KonReaderState;

typedef struct _StateStackNode {
    KonReaderState data;
    struct _StateStackNode *next;
} StateStackNode;

typedef struct _StateStack {
    long length;
    StateStackNode* top;
} StateStack;

StateStack* StateStackInit();
void StateStackDestroy(StateStack* stack);
void StateStackPush(StateStack* stack, KonReaderState item);
KonReaderState StateStackPop(StateStack* stack);
KonReaderState StateStackLength(StateStack* stack);
KonReaderState StateStackTop(StateStack* stack);
void StateStackSetTopValue(StateStack* stack, KonReaderState newData);
