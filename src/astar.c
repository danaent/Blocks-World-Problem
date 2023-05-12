#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <error.h>
#include <time.h>
#include "../include/pq.h"
#include "../include/stack.h"
#include "../include/states.h"

// Function declarations
void find_successors(State cur_state, State goal_state, PQ open_list);

void Astar(State init_state, State goal_state)
{
    clock_t start_time = clock();
    bool solved = false;

    // Initialize open and closed list
    PQ open_list = pq_init(state_compare_f, state_destroy);
    Stack closed_list = stack_init(state_destroy);

    // Evaluate initial state and add to open list
    state_evaluation_noparent(init_state, goal_state);
    pq_insert(open_list, init_state);

    // While there are still states to check
    while(!pq_empty(open_list))
    {
        // Get state with smaller evaluation function from open list and add to closed list
        State cur_state = pq_remove(open_list);
        stack_push(closed_list, cur_state);

        // If found goal state
        if (state_equal(cur_state, goal_state))
        {
            // Print path and break loop
            state_print_path(cur_state);

            printf("\nNumber of moves: %u\n", state_f(cur_state));
            printf("States explored: %u\n", (unsigned int) stack_size(closed_list));
            printf("Time elapsed: %.3lfs\n", ((double) (clock() - start_time)) / CLOCKS_PER_SEC);

            solved = true;
            break;
        }

        // Get successors and add to open list
        find_successors(cur_state, goal_state, open_list);

        // Free unnecessary memory
        state_free_positions(cur_state);
    }

    if (!solved)
        printf("Unable to solve problem\n");

    // Free memory for closed and open list
    pq_destroy(open_list);
    stack_destroy(closed_list);

    // Free memory for goal state
    state_destroy(goal_state);
}

// Finds successor states of current state and adds them to the open list
void find_successors(State cur_state, State goal_state, PQ open_list)
{
    char stacks = state_stacks(cur_state);

    // For every possible move
    for (char i = 0; i < stacks; i++)
    {
        for (char j = 0; j < stacks; j++)
        {
            // Generate successor state by moving a block
            State succ_state = state_move_block(cur_state, i, j, true);

            // If move is not possible
            if (succ_state == NULL)
                continue;

            // Evaluate f and add state to the open list
            state_evaluation_fromparent(succ_state, goal_state);
            pq_insert(open_list, succ_state);
        }
    }
}