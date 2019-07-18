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
    // s4 parse table key, need identifier, symbol('') or $abc
    KON_READER_PARSE_TABLE_KEY,
    // s5 parse table value or next kv pair
    KON_READER_PARSE_TABLE_VAL_OR_NEXT,
    // s6 parse cell, need tag, identifier, symbol('') or $abc, or [, <, (
    KON_READER_PARSE_CELL_TAG,
    // s7 parse cell inner array, table, or list
    KON_READER_PARSE_CELL_INNER_CONTAINER,
    // // s8 parse cell table
    // KON_READER_PARSE_CELL_INNER_TABLE,
    // // s9 parse cell arr
    // KON_READER_PARSE_CELL_INNER_VECTOR,
    // // s10 parse cell list
    // KON_READER_PARSE_CELL_INNER_LIST,

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
