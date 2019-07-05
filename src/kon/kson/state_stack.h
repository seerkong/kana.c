#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


typedef struct _StateStackNode {
    long Data;
    struct _StateStackNode *Next;
} StateStackNode;

typedef struct _StateStack {
    long Length;
    StateStackNode* Top;
} StateStack;

StateStack* StateStackInit();
void StateStackDestroy(StateStack* stack);
void StateStackPush(StateStack* stack, long item);
long StateStackPop(StateStack* stack);
long StateStackLength(StateStack* stack);
long StateStackTop(StateStack* stack);
void StateStackSetTopValue(StateStack* stack, long newData);
