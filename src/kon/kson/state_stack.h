#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


// state
typedef enum
{
    // s0
    KON_READER_ROOT = 0,
    // s1 parse arr, need value or arr end
    KON_READER_PARSE_VECTOR,
    // s2 parse list, need value or list end
    KON_READER_PARSE_LIST,
    // s3 parse table, need ':' or table end
    KON_READER_PARSE_TABLE,
    // s4 parse table or map key, need identifier, symbol('') or $abc
    KON_READER_PARSE_TABLE_PAIR_KEY,
    // s5 parse table value or next kv pair
    KON_READER_PARSE_TABLE_PAIR_VAL,
    // s6 parse cell, need core, eg: symbol, number str, wrapper, expr
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
    // s10 parse cell map value or next kv pair
    KON_READER_PARSE_MAP_PAIR_VAL,

    // s7 parse cell inner array, table, or list
    KON_READER_PARSE_CELL_INNER_CONTAINER,

    // s8 parse param table, inner state:KON_READER_PARSE_TABLE_PAIR_KEY KON_READER_PARSE_TABLE_PAIR_VAL
    KON_READER_PARSE_PARAM,
    // s9 parse block list
    KON_READER_PARSE_BLOCK,



    // s11 parse tpl expand need identifier $.abc
    KON_READER_PARSE_EXPAND_REPLACE,
    // s12 parse tpl expand to sequence, need arr,table or arr,table var $~.
    KON_READER_PARSE_EXPAND_SEQ,
    // s13 parse tpl expand to k v pair, need table or table var $%.
    KON_READER_PARSE_EXPAND_KV,
    

    // s15 parse tpl expr var slot, need identifier or list
    // @. @~. @%.
    KON_READER_PARSE_UNQUOTE,
    // s16 quasiquote container @[ @( @< @{ need [ ( < {
    KON_READER_PARSE_QUASIQUOTE,
    // s17 quote, need identifier or [ ( < { .eg $abc $[ $( $< ${ 
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
