#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#include "../include/states.h"

#ifndef HEURISTIC
    #define HEURISTIC 3
#endif

#if HEURISTIC == 1
    #define HEURISTIC_FUNC State_heuristic1
    #define HEURISTIC_FUNC_PARENT State_heuristic1_fromparent
#elif HEURISTIC == 2
    #define HEURISTIC_FUNC State_heuristic2
    #define HEURISTIC_FUNC_PARENT State_heuristic2_fromparent
#else
    #define HEURISTIC_FUNC State_heuristic3
    #define HEURISTIC_FUNC_PARENT State_heuristic3_fromparent
#endif

// Struct for each block
typedef struct block
{
    bl_int top; // Block on top of this block
    bl_int bottom; // Block under this block
    bl_int must_move; // Minimum amount of times this block must be moved to reach goal state
}
block;

// Struct for each state
typedef struct state
{
    block *positions; // Block positions
    bl_int *tops, *bottoms, blocks_count, stacks_count, past_action[3]; // Top and bottom blocks for each stack, number of blocks, number of stacks, action that lead to this state from parent state
    unsigned int f, h, g; // Evaluation functions
    State parent_state;
}
state;

// For past_action table
enum action {BLOCK, FROM_BLOCK, TO_BLOCK};

// For must_move variable
enum move {NO, ONCE, TWICE};

// Declarations for heuristic functions
void State_heuristic1(State cur_state, State goal_state);
void State_heuristic2(State cur_state, State goal_state);
void State_heuristic3(State cur_state, State goal_state);
void State_heuristic1_fromparent(State cur_state, State goal_state);
void State_heuristic2_fromparent(State cur_state, State goal_state);
void State_heuristic3_fromparent(State cur_state, State goal_state);


State state_create(void)
{
    State new_state = malloc(sizeof(state));
    assert(new_state != NULL);

    new_state->positions = NULL;
    new_state->tops = NULL;
    new_state->bottoms = NULL;
    new_state->stacks_count = 0;
    new_state->f = 0;
    new_state->h = 0;
    new_state->g = 0;
    new_state->parent_state = NULL;

    return new_state;
}

bool state_get(State blocks_state, bl_int blocks_total)
{
    bl_int cur_block, prev_block = TABLE;

    // Set number of blocks
    blocks_state->blocks_count = blocks_total;

    // Allocate memory for block positions
    blocks_state->positions = malloc(blocks_total * sizeof(block));
    assert(blocks_state->positions != NULL);

    // Table for whether user has already placed this block
    bool *isPlaced = calloc(blocks_total, sizeof(bool));
    assert(isPlaced != NULL);

    // For each block
    for (bl_int i = 0; i < blocks_total; i++)
    {
        // Get block number
        if (scanf("%hhi", &cur_block) != 1)
        {
            free(isPlaced);
            return false;
        }

        // Check if in range
        if (cur_block >= blocks_total || cur_block < 0)
        {
            free(isPlaced);
            return false;
        }

        // Check if block has already been placed
        if (isPlaced[cur_block])
        {
            free(isPlaced);
            return false;
        }
        else
            isPlaced[cur_block] = true;
        
        // Set bottom block of current block to previous block
        blocks_state->positions[cur_block].bottom = prev_block;

        // Set top block of previous block to current block
        if (prev_block != TABLE)
            blocks_state->positions[prev_block].top = cur_block;

        // New stack if new line is read
        int ch = getchar();
        while(isspace(ch))
        {
            if (ch == '\n')
            {   // Set top to sky and increase number of stacks
                blocks_state->positions[cur_block].top = SKY;
                blocks_state->stacks_count++;

                prev_block = TABLE;
                break;
            }
            ch = getchar();
        }

        // If new line is not read
        if (ch != '\n')
        {
            prev_block = cur_block;
            ungetc(ch, stdin);
        }
    }

    // Set final block's top to sky
    blocks_state->positions[cur_block].top = SKY;

    // Allocate memory for tops and bottoms
    blocks_state->tops = malloc(blocks_state->stacks_count * sizeof(int));
    assert(blocks_state->tops != NULL);

    blocks_state->bottoms = malloc(blocks_state->stacks_count * sizeof(int));
    assert(blocks_state->bottoms != NULL);

    // Find top and bottom blocks and initialize must move variable
    bl_int top_index = 0, bottom_index = 0;
    for (bl_int i = 0; i < blocks_total; i++)
    {   
        blocks_state->positions[i].must_move = NO;

        if (blocks_state->positions[i].top == SKY)
            blocks_state->tops[top_index++] = i;

        if (blocks_state->positions[i].bottom == TABLE)
            blocks_state->bottoms[bottom_index++] = i;
    }

    free(isPlaced);
    return true;
}

State state_move_block(State cur_state, bl_int from_stack, bl_int to_stack, bool skip_parent_state)
{
    // Find block that is moved and block it will be moved on top of
    bl_int moved_block = cur_state->tops[from_stack];
    bl_int on_block = (from_stack == to_stack ? TABLE : cur_state->tops[to_stack]); // If from and to stack are the same, move block to table

    // If block must be moved to the table and is already on the table
    if (on_block == TABLE && cur_state->positions[moved_block].bottom == TABLE)
        return NULL;

    // Do not produce state if it is parent state
    if (skip_parent_state && cur_state->parent_state != NULL && cur_state->past_action[BLOCK] == moved_block && cur_state->past_action[FROM_BLOCK] == on_block)
        return NULL;

    // Allocate memory for new state
    State new_state = malloc(sizeof(state));

    // Set past action table
    new_state->past_action[BLOCK] = moved_block; // Block that is moved
    new_state->past_action[FROM_BLOCK] = cur_state->positions[moved_block].bottom; // Block it is moved from
    new_state->past_action[TO_BLOCK] = on_block; // Block it is moved to

    // Allocate memory for new state's block positions and copy old positions to modify them
    new_state->positions = malloc(cur_state->blocks_count * sizeof(block));
    memcpy(new_state->positions, cur_state->positions, cur_state->blocks_count * sizeof(block));

    // Set new blocks count to old blocks count
    new_state->blocks_count = cur_state->blocks_count;

    // If block must be moved to the table
   if (on_block == TABLE)
   {
        // New stack is created
        new_state->stacks_count = cur_state->stacks_count + 1;

        new_state->tops = malloc(new_state->stacks_count * sizeof(int));
        assert(new_state->tops != NULL);

        new_state->bottoms = malloc(new_state->stacks_count * sizeof(int));
        assert(new_state->bottoms != NULL);

        // Copy old top blocks and add block that was below moved block
        memcpy(new_state->tops, cur_state->tops, cur_state->stacks_count * sizeof(int));
        new_state->tops[from_stack] = cur_state->positions[moved_block].bottom;
        new_state->tops[cur_state->stacks_count] = moved_block;

        // Copy old bottom blocks and add moved block
        memcpy(new_state->bottoms, cur_state->bottoms, cur_state->stacks_count * sizeof(int));
        new_state->bottoms[cur_state->stacks_count] = moved_block;

        // Set moved block bottom and top
        new_state->positions[moved_block].bottom = TABLE;
        new_state->positions[cur_state->positions[moved_block].bottom].top = SKY;
   }

   // If moved block is on the table and must be moved to a stack
   else
   {
        if (cur_state->positions[moved_block].bottom == TABLE)
        {
            // A stack is removed
            new_state->stacks_count = cur_state->stacks_count - 1;

            new_state->tops = malloc(new_state->stacks_count * sizeof(int));
            assert(new_state->tops != NULL);

            new_state->bottoms = malloc(new_state->stacks_count * sizeof(int));
            assert(new_state->bottoms != NULL);
            
            // Copy old top blocks and skip block that is now under moved block
            memcpy(new_state->tops, cur_state->tops, to_stack * sizeof(int));
            memcpy(new_state->tops + to_stack, cur_state->tops + to_stack + 1, (new_state->stacks_count - to_stack) * sizeof(int));

            // Copy old bottom blocks and skip moved block
            memcpy(new_state->bottoms, cur_state->bottoms, from_stack * sizeof(int));
            memcpy(new_state->bottoms + from_stack, cur_state->bottoms + from_stack + 1, (new_state->stacks_count - from_stack) * sizeof(int));

            // Set moved block bottom and top
            new_state->positions[moved_block].bottom = cur_state->tops[to_stack];
            new_state->positions[cur_state->tops[to_stack]].top = moved_block;

        }
        // If moved block must be moved from one stack to another
        else
        {
            // Number of stacks remains the same
            new_state->stacks_count = cur_state->stacks_count;

            new_state->tops = malloc(new_state->stacks_count * sizeof(int));
            assert(new_state->tops != NULL);

            new_state->bottoms = malloc(new_state->stacks_count * sizeof(int));
            assert(new_state->bottoms != NULL);

            // Copy old top blocks, remove block that is now under moved block and add block that used to be under moved block
            memcpy(new_state->tops, cur_state->tops, cur_state->stacks_count * sizeof(int));
            new_state->tops[from_stack] = cur_state->positions[moved_block].bottom;
            new_state->tops[to_stack] = moved_block;

            // Copy old bottom blocks
            memcpy(new_state->bottoms, cur_state->bottoms, cur_state->stacks_count * sizeof(int));

            // Set moved block bottom and top
            new_state->positions[moved_block].bottom = cur_state->tops[to_stack];
            new_state->positions[cur_state->tops[to_stack]].top = moved_block;
            new_state->positions[cur_state->positions[moved_block].bottom].top = SKY;
        }
   }

    // Set new state's parent state
    new_state->parent_state = cur_state;

    return new_state;
}

bool state_equal(State state1, State state2)
{
    // If blocks count is not the same
    if (state1->blocks_count != state2->blocks_count || state1->stacks_count != state2->stacks_count)
        return false;

    // If states differ somewhere
    for (bl_int i = 0; i < state1->blocks_count; i++)
        if (state1->positions[i].top != state2->positions[i].top || state1->positions[i].bottom != state2->positions[i].bottom)
            return false;

    return true;
}

void state_print_path(State blocks_state)
{
    // Base case is the first state in the path
    if (blocks_state->parent_state == NULL)
        return;

    // Call function for parent state
    state_print_path(blocks_state->parent_state);

    // Print move
    printf("Move block %hhi to ", blocks_state->past_action[BLOCK]);
    
    if (blocks_state->past_action[TO_BLOCK] == TABLE)
        printf("table\n");
    else
        printf("block %hhi\n", blocks_state->past_action[TO_BLOCK]);
}

void state_destroy(void *void_state)
{
    State blocks_state = void_state;

    if (blocks_state->positions)
    {
        free(blocks_state->positions);
        free(blocks_state->tops);
        free(blocks_state->bottoms);
    }
    
    free(blocks_state);
}

void state_free_positions(void *void_state)
{
    State blocks_state = void_state;
    free(blocks_state->positions);
    free(blocks_state->tops);
    free(blocks_state->bottoms);
    blocks_state->positions = NULL;
}

void state_evaluation_noparent(State cur_state, State goal_state)
{
    cur_state->g = 0;
    HEURISTIC_FUNC(cur_state, goal_state);
    
    cur_state->f = cur_state->g + cur_state->h;
}

void state_evaluation_fromparent(State cur_state, State goal_state)
{
    cur_state->g = cur_state->parent_state->g + 1;
    HEURISTIC_FUNC_PARENT(cur_state, goal_state);

    cur_state->f = cur_state->g + cur_state->h;
}

int state_compare_f(void *void_state1, void *void_state2)
{
    State state1 = void_state1;
    State state2 = void_state2;
    return state1->f - state2->f;
}

unsigned int state_f(State blocks_state)
{
    return blocks_state->f;
}

bl_int state_stacks(State blocks_state)
{
    return blocks_state->stacks_count;
}

/************************************************************HEURISTIC FUNCTIONS************************************************************/

bool state_valid_heuristic()
{
    if (HEURISTIC != 1 && HEURISTIC != 2 && HEURISTIC != 3)
        return false;

    return true;
}

void State_heuristic1(State cur_state, State goal_state)
{
    unsigned int h = 0;

    // Add 1 for every block that isn't on the right block
    for (bl_int i = 0; i < cur_state->blocks_count; i++)
    {
        if (cur_state->positions[i].bottom != goal_state->positions[i].bottom)
            h += 1;
    }

    cur_state->h = h;
}

void State_heuristic1_fromparent(State cur_state, State goal_state)
{
    // Set h to same as parent state's
    unsigned int h = cur_state->parent_state->h;

    // Find block that moved to create current state
    bl_int moved_block = cur_state->past_action[BLOCK];

    // If moved block used to be on the right block and now isn't
    if (cur_state->parent_state->positions[moved_block].bottom == goal_state->positions[moved_block].bottom && cur_state->positions[moved_block].bottom != goal_state->positions[moved_block].bottom)
        h++;

    // If moved block wasn't on the right block but now is
    else if (cur_state->parent_state->positions[moved_block].bottom != goal_state->positions[moved_block].bottom && cur_state->positions[moved_block].bottom == goal_state->positions[moved_block].bottom)
        h--;

    cur_state->h = h;
}

void State_heuristic2(State cur_state, State goal_state)
{
    unsigned int h = 0;

    // For every stack
    for (bl_int i = 0; i < cur_state->stacks_count; i++)
    {
        bl_int cur_block = cur_state->bottoms[i];

        // Find the first block that is incorrectly placed
        while (cur_block != SKY)
        {
            if (cur_state->positions[cur_block].bottom != goal_state->positions[cur_block].bottom)
                break;

            cur_block = cur_state->positions[cur_block].top;
        }

        // Add 1 for every block above it
        while (cur_block != SKY)
        {
            h++;
            cur_state->positions[cur_block].must_move = ONCE; // Each of these blocks must move once
            cur_block = cur_state->positions[cur_block].top;
        }
    }

    cur_state->h = h;
}

void State_heuristic2_fromparent(State cur_state, State goal_state)
{
    // Set h to same as parent state's and find block that moved to create current state
    unsigned int h = cur_state->parent_state->h;
    bl_int moved_block = cur_state->past_action[BLOCK];

    // Change that block's must move variable and take it away from the heuristic total
    h -= cur_state->parent_state->positions[moved_block].must_move;
    cur_state->positions[moved_block].must_move = NO;

    // If block isn't placed on the right block
    if (cur_state->positions[moved_block].bottom != goal_state->positions[moved_block].bottom)
    {   
        h++;
        cur_state->positions[moved_block].must_move = ONCE;
    }

    // If block is placed on the right block and that block is not the table
    else if (cur_state->positions[moved_block].bottom != TABLE)
    {
        // If block below moved block must be moved, moved block must also be moved
        if (cur_state->positions[cur_state->positions[moved_block].bottom].must_move == ONCE)
        {
            h++;
            cur_state->positions[moved_block].must_move = ONCE;
        }
    }
    cur_state->h = h;
}

void State_heuristic3(State cur_state, State goal_state)
{
    unsigned int h = 0;

    // Array that shows if a specific block is in a stack or not
    bool *inStack = calloc(cur_state->blocks_count, sizeof(bool));
    assert(inStack != NULL);

    // For every stack
    for (bl_int i = 0; i < cur_state->stacks_count; i++)
    {
        bl_int cur_block = cur_state->bottoms[i];

        // Find the first block that is incorrectly placed
        while (cur_block != SKY)
        {
            if (cur_state->positions[cur_block].bottom != goal_state->positions[cur_block].bottom)
                break;

            // This block is in the stack
            inStack[cur_block] = true;
            cur_block = cur_state->positions[cur_block].top;
        }

        // For every block above that incorrectly placed block
        while (cur_block != SKY)
        {
            // If the block that should be below that block is under it in the same stack, that block must move twice, otherwise once
            if (goal_state->positions[cur_block].bottom != TABLE)
            {
                if (inStack[goal_state->positions[cur_block].bottom])
                {
                    h += 2;
                    cur_state->positions[cur_block].must_move = TWICE;
                }
                else
                {
                    h++;
                    cur_state->positions[cur_block].must_move = ONCE;
                }
            }
            else
            {
                h++;
                cur_state->positions[cur_block].must_move = ONCE;
            }

            inStack[cur_block] = true;
            cur_block = cur_state->positions[cur_block].top;
        }

        // Set all array cells to false for next stack
        memset(inStack, 0, cur_state->blocks_count * sizeof(bool));
    }

    free(inStack);
    cur_state->h = h;
}

void State_heuristic3_fromparent(State cur_state, State goal_state)
{
    // Set h to same as parent state's and find block that moved to create current state
    unsigned int h = cur_state->parent_state->h;
    bl_int moved_block = cur_state->past_action[BLOCK];

    // Change that block's must move variable and take it away from the heuristic total
    h -= cur_state->parent_state->positions[moved_block].must_move;
    cur_state->positions[moved_block].must_move = NO;

    // If moved block used to be on a stack with the block it must be placed on below it
    if (cur_state->parent_state->positions[moved_block].must_move == TWICE)
    {
        // It will either be placed on a different stack in the wrong placed or in the right place
        if (cur_state->positions[moved_block].bottom != goal_state->positions[moved_block].bottom)
        {
            h += 1;
            cur_state->positions[moved_block].must_move = ONCE;
        }
        cur_state->h = h;
        return;
    }

    // If moved block is not placed on the right block
    if (cur_state->positions[moved_block].bottom != goal_state->positions[moved_block].bottom)
    {
        bl_int bottom_block = cur_state->positions[moved_block].bottom;

        // Check if the stack it is on has the block it must be placed on below it
        while(bottom_block != TABLE)
        {
            if (bottom_block == goal_state->positions[moved_block].bottom)
            {
                h += 2;
                cur_state->positions[moved_block].must_move = TWICE;
                break;
            }
            bottom_block = cur_state->positions[bottom_block].bottom;
        }

        if (bottom_block == TABLE)
        {
            h += 1;
            cur_state->positions[moved_block].must_move = ONCE;
        }
    }
    // If block is placed on the right block but it must be moved
    else if (cur_state->positions[moved_block].bottom != TABLE && cur_state->positions[cur_state->positions[moved_block].bottom].must_move)
    {
        h += 2;
        cur_state->positions[moved_block].must_move = TWICE;
    }
    cur_state->h = h;
}