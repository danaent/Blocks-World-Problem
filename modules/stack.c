#include <stdlib.h>
#include <stdbool.h>
#include "../include/stack.h"

typedef struct node* Node;

typedef struct node
{
    item data;
    Node next;
} 
node;

struct stack
{
    Node head;
    size_t size;
    void (*free)(item);
};

Stack stack_init(void (*free)(item))
{
    Stack stack = malloc(sizeof(struct stack));

    stack->head = NULL;
    stack->size = 0;

    stack->free = free;
    return stack;
}

bool stack_empty(Stack stack)
{
    return (stack->size == 0);
}

size_t stack_size(Stack stack)
{
    return stack->size;
}

void stack_push(Stack stack, item new_item)
{
    // Allocate memory for  new node
    Node new_node = malloc(sizeof(node));

    // Add new node before head and change head
    new_node->data = new_item;
    new_node->next = stack->head;
    stack->head = new_node;

    stack->size++;
}

item stack_pop(Stack stack)
{
    // Save head node's item and head node in temporary variables
    item i = stack->head->data;
    Node temp_node = stack->head;
    
    // Change head and free memory for old head
    stack->head = stack->head->next;
    free(temp_node);
    stack->size--;

    // Return item
    return i;
}

void stack_destroy(Stack stack)
{
    // Free every node in the list
    for (unsigned int i = 0; i < stack->size; i++)
    {
        Node cur_node = stack->head;
        stack->head = stack->head->next;

        stack->free(cur_node->data);
        free(cur_node);
    }
    free(stack);
}