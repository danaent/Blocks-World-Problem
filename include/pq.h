#pragma once
#include <stdbool.h>
#include "../include/item.h"

#define INIT_SIZE 64

typedef struct priority_queue* PQ;

// Initializes PQ
PQ pq_init(int (*cmp)(item, item), void (*free)(item));

// Returns true if PQ is empty
bool pq_empty(PQ PQ);

// Returns PQ size
size_t pq_size(PQ PQ);

// Inserts an item into PQ
void pq_insert(PQ PQ, item new_item);

// Removes item with highest priority from PQ
item pq_remove(PQ PQ);

// Frees memory allocated for PQ
void pq_destroy(PQ PQ);