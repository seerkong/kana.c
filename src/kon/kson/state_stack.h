#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


// state
typedef enum
{
    //
    KON_READER_ROOT = 0,
    // parse arr, need value or arr end
    KON_READER_PARSE_VECTOR,
    // parse list, need value or list end
    KON_READER_PARSE_LIST,
    // parse table, need ':' or table end
    KON_READER_PARSE_TABLE,
    // parse table or map key, need identifier, symbol('') or $abc
    KON_READER_PARSE_TABLE_PAIR_KEY,
    KON_READER_PARSE_TABLE_PAIR_EQ,
    // parse table value or next kv pair
    KON_READER_PARSE_TABLE_PAIR_VAL,
    // parse cell, need core, eg: symbol, number str, wrapper, expr
    // or map key :a 1 :b
    // or param, block
    KON_READER_PARSE_CELL_CORE,
    // need map key :a 1 :b
    // or param, block
    // or next
    KON_READER_PARSE_CELL_MAP,
    KON_READER_PARSE_MAP_PAIR_KEY,

    // need a '=' or ':'
    KON_READER_PARSE_MAP_PAIR_EQ_OR_TAG,
    // parse cell map value or next kv pair
    KON_READER_PARSE_MAP_PAIR_VAL,

    // parse cell inner array, table, or list
    KON_READER_PARSE_CELL_INNER_CONTAINER,

    // parse param table, inner state:KON_READER_PARSE_TABLE_PAIR_KEY KON_READER_PARSE_TABLE_PAIR_VAL
    KON_READER_PARSE_PARAM,
    // parse block list
    KON_READER_PARSE_BLOCK,



    // parse tpl expand need identifier $.abc
    KON_READER_PARSE_EXPAND_REPLACE,
    // parse tpl expand to sequence, need arr,table or arr,table var $~.
    KON_READER_PARSE_EXPAND_SEQ,
    // parse tpl expand to k v pair, need table or table var $%.
    KON_READER_PARSE_EXPAND_KV,
    

    // parse tpl expr var slot, need identifier or list
    // @. @~. @%.
    KON_READER_PARSE_UNQUOTE,
    // quasiquote container @[ @( @< @{ need [ ( < {
    KON_READER_PARSE_QUASIQUOTE,
    // quote, need identifier or [ ( < { .eg $abc $[ $( $< ${ 
    KON_READER_PARSE_QUOTE,
} KonReaderState;

typedef struct _StateStackNode {
    KonReaderState Data;
    struct _StateStackNode *Next;
} StateStackNode;

typedef struct _StateStack {
    long Length;
    StateStackNode* Top;
} StateStack;

StateStack* StateStackInit();
void StateStackDestroy(StateStack* stack);
void StateStackPush(StateStack* stack, KonReaderState item);
KonReaderState StateStackPop(StateStack* stack);
KonReaderState StateStackLength(StateStack* stack);
KonReaderState StateStackTop(StateStack* stack);
void StateStackSetTopValue(StateStack* stack, KonReaderState newData);
