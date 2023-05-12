#pragma once
#include <stdbool.h>
#include "item.h"

typedef struct stack *Stack;

// Initializes stack
Stack stack_init(void (*free)(item));

// Returns true if stack is empty
bool stack_empty(Stack S);

// Returns stack size
size_t stack_size(Stack S);

// Pushes an item at the top of the stack
void stack_push(Stack S, item new_item);

// Removes an item from the top of the stack
item stack_pop(Stack S);

// Frees memory for stack
void stack_destroy(Stack S);