#include "state_stack.h"
#include "../../lib/tbox/tbox.h"

StateStack* StateStackInit()
{
    StateStack* stack = (StateStack*)tb_malloc(sizeof(StateStack));
    if (stack == NULL) {
        return NULL;
    }
    stack->length = 0;
    stack->top = NULL;
    return stack;
}

void StateStackDestroy(StateStack* stack)
{
    // TODO iter node, free
    StateStackNode* top = stack->top;
    while (top) {
        StateStackNode* oldTop = top;
        tb_free(oldTop);
        top = top->next;
    }
    tb_free(stack);
}

void StateStackPush(StateStack* stack, KonReaderState item)
{
    assert(stack);
    StateStackNode* oldTop = stack->top;
    StateStackNode* newTop = (StateStackNode*)tb_malloc(sizeof(StateStackNode));
    assert(newTop);
    newTop->data = item;
    newTop->next = oldTop;
    stack->top = newTop;
    stack->length = stack->length + 1;
}

KonReaderState StateStackPop(StateStack* stack)
{
    assert(stack);
    StateStackNode* top = stack->top;
    assert(top);
    StateStackNode* next = stack->top->next;

    long data = top->data;
    stack->top = next;
    stack->length = stack->length - 1;

    tb_free(top);
    return data;
}

KonReaderState StateStackLength(StateStack* stack)
{
    assert(stack);
    return stack->length;
}

KonReaderState StateStackTop(StateStack* stack)
{
    assert(stack);
    StateStackNode* top = stack->top;
    assert(top);
    return top->data;
}

void StateStackSetTopValue(StateStack* stack, KonReaderState newData)
{
    assert(stack);
    StateStackNode* top = stack->top;
    assert(top);
    top->data = newData;
}
