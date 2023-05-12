#include <stdlib.h>
#include <stdio.h>
#include "../include/pq.h"

#define parent(n) ((n - 1) / 2)
#define left_child(n) (2 * n + 1)
#define right_child(n) (2 * n + 2)

#define MIN_CAPACITY 1

struct priority_queue
{
    item *heap;
    size_t size;
    size_t capacity;
    int (*cmp)(item, item);
    void (*free)(item);
};

PQ pq_init(int (*cmp)(item, item), void (*free)(item))
{
    PQ pq = malloc(sizeof(struct priority_queue));
    pq->heap = malloc(sizeof(item));

    pq->size = 0;
    pq->capacity = MIN_CAPACITY;

    pq->cmp = cmp;
    pq->free = free;

    return pq;
}


bool pq_empty(PQ pq)
{
    return (pq->size == 0);
}

size_t pq_size(PQ pq)
{
    return pq->size;
}

void pq_insert(PQ pq, item new_item)
{
    // Double capacity to fit new item
    if (pq->size == pq->capacity)
    {
        pq->capacity *= 2;
        pq->heap = realloc(pq->heap, pq->capacity * sizeof(item));
    }

    size_t child = pq->size;
    size_t parent = parent(child);

    // While root hasn't been reached
    while (child && pq->cmp(new_item, pq->heap[parent]) < 0)
    {
        // Shift down items until new item can be placed
        pq->heap[child] = pq->heap[parent];
        child = parent;
        parent = parent(child);
    }

    pq->heap[child] = new_item;
    pq->size++;
}

item pq_remove(PQ pq)
{ 
    item max_item = pq->heap[0]; // Item that is returned
    item cur_item = pq->heap[--pq->size]; // Item that should shift position

    size_t current = 0; // Position item will be placed in
    size_t left = left_child(current); // Left and right children of that position
    size_t right = right_child(current);
    size_t new = 0; // Position current position may change to

    // While current node has at least a left child
    while (left < pq->size)
    {
        // Set new position as the largest of current node's children that is larger than current node
        if (pq->cmp(pq->heap[left], cur_item) < 0)
            new = left;

        if (right < pq->size && pq->cmp(pq->heap[right], cur_item) < 0)
        {
            if (new == current)
                new = right;
            else
                new = pq->cmp(pq->heap[right], pq->heap[left]) < 0 ? right : left;
        }

        // If current position hasn't changed, place shifted item here
        if (new == current)
            break;

        // Change item of current position to item of new position
        pq->heap[current] = pq->heap[new];

        // Change current position to new position and find its children
        current = new;
        left = left_child(current);
        right = right_child(current);
    }

     // Place shifted item and return max item
    pq->heap[current] = cur_item;
    return max_item;
}

void pq_destroy(PQ pq)
{
    if (pq->free)
        for (unsigned int i = 0; i < pq->size; i++)
            pq->free(pq->heap[i]);

    free(pq->heap);
    free(pq);
}
