#include <stdio.h>
#include "../include/astar.h"

// Frees memory and prints error message
int exit_message(State init_state, State goal_state, char *message);

int main(void)
{
    int cubes_total;
    State init_state = state_create(), goal_state = state_create();

    // If heuristic does not exist
    if (!state_valid_heuristic())
        return exit_message(init_state, goal_state, "Available heuristics: 1, 2, 3\n");

    printf("Input the total number of blocks: ");

    if (scanf("%d", &cubes_total) != 1 || cubes_total < 1)
        return exit_message(init_state, goal_state, "Invalid number of blocks!\n");

    printf("\nEach of the %d blocks is assigned a number from 0 to %d. To describe a state, input the blocks in each stack from the bottom\n", cubes_total, cubes_total-1);
    printf("to the top, the way they would be placed on a table, and follow each stack with a new line. For example, the state:\n\n");

    printf(" 2\n");
    printf(" 1  4\n");
    printf(" 0  3\n");
    printf("\nwould be inputed as:\n\n");
    printf("0 1 2\n3 4\n\n");

    printf("Start by describing the initial state:\n");

    if (!state_get(init_state, cubes_total))
        return exit_message(init_state, goal_state, "Please follow the format!\n");

    printf("Now describe the goal state:\n");

    if (!state_get(goal_state, cubes_total))
        return exit_message(init_state, goal_state, "Please follow the format!\n");

    printf("\n");

    Astar(init_state, goal_state);

    return 0;
}

int exit_message(State init_state, State goal_state, char *message)
{
    state_destroy(init_state);
    state_destroy(goal_state);
    fprintf(stderr, "%s", message);
    return 1;
}