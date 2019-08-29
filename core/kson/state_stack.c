#include "state_stack.h"
#include "../../lib/tbox/tbox.h"

StateStack* StateStackInit()
{
    StateStack* stack = (StateStack*)tb_malloc(sizeof(StateStack));
    if (stack == NULL) {
        return NULL;
    }
    stack->Length = 0;
    stack->Top = NULL;
    return stack;
}

void StateStackDestroy(StateStack* stack)
{
    // TODO iter node, free
    StateStackNode* top = stack->Top;
    while (top) {
        StateStackNode* oldTop = top;
        tb_free(oldTop);
        top = top->Next;
    }
    tb_free(stack);
}

void StateStackPush(StateStack* stack, KonReaderState item)
{
    assert(stack);
    StateStackNode* oldTop = stack->Top;
    StateStackNode* newTop = (StateStackNode*)tb_malloc(sizeof(StateStackNode));
    assert(newTop);
    newTop->Data = item;
    newTop->Next = oldTop;
    stack->Top = newTop;
    stack->Length = stack->Length + 1;
}

KonReaderState StateStackPop(StateStack* stack)
{
    assert(stack);
    StateStackNode* top = stack->Top;
    assert(top);
    StateStackNode* next = stack->Top->Next;

    long data = top->Data;
    stack->Top = next;
    stack->Length = stack->Length - 1;

    tb_free(top);
    return data;
}

KonReaderState StateStackLength(StateStack* stack)
{
    assert(stack);
    return stack->Length;
}

KonReaderState StateStackTop(StateStack* stack)
{
    assert(stack);
    StateStackNode* top = stack->Top;
    assert(top);
    return top->Data;
}

void StateStackSetTopValue(StateStack* stack, KonReaderState newData)
{
    assert(stack);
    StateStackNode* top = stack->Top;
    assert(top);
    top->Data = newData;
}
