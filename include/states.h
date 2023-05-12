#pragma once
#include <stdbool.h>

#define TABLE -1
#define SKY -1

typedef struct state* State;
typedef char bl_int;

// Creates a blank state
State state_create(void);

// Reads data from standard input and creates a state
bool state_get(State blocks_state, bl_int blocks_total);

// Creates a new state by moving a block from one stack to another or on the table
State state_move_block(State cur_state, bl_int from_stack, bl_int to_stack, bool skip_parent_state);

// Compares two states and returns true if they're the same
bool state_equal(State state1, State state2);

// Prints moves that lead to a state
void state_print_path(State blocks_state);

// Frees memory for a state
void state_destroy(void *blocks_state);

// Frees unnencessary memory help by a state
void state_free_positions(void *blocks_state);

// Evaluates f, g and h
void state_evaluation_noparent(State cur_state, State goal_state);

// Evaluates f, g and h from a state's parent state
void state_evaluation_fromparent(State cur_state, State goal_state);

// Returns the difference between two states's fs
int state_compare_f(void *state1, void *state2);

// Returns a state's f function
unsigned int state_f(State blocks_state);

// Returns a state's number of stacks
bl_int state_stacks(State blocks_state);

// Checks if heuristic number exists
bool state_valid_heuristic();