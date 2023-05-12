# Blocks World Problem

The blocks world consits of a set of numbered blocks arranged on a table. Each block is either placed on top of another block, forming a stack, or on the table. The goal is to go from a given initial state to a given goal state by making the minimum number of moves possible. Only one block can be moved at a time and blocks can only be moved if there are no other blocks on top of them. Here the problem is solved using A* search.

## Heuristics

The following heuristics can be used:

- **Heuristic 1:** Returns the number of blocks placed on top of the wrong block
- **Heuristic 2:** Starting from the bottom of each stack, this heuristic finds the first block that is placed on the wrong block, adds every block that is stacked above it to the sum of incorrectly placed blocks and returns that sum.
- **Heuristic 3:** Same as 2, but adds 2 to the sum for each block that must be moved twice. A block must be moved twice if the block it must be placed on is somewhere beneath it in the same stack.

## Use

You can compile the program with ```make``` or compile and run with ```make run```. The heuristic function can be selected during compilation with ```make HEURISTIC=2``` or ```make run HEURISTIC=2```. If no heuristic is selected, the default is 3. If you want to change heuristic, you have to remove the executable file with ```make clean``` and recompile the program.

## Tests

There are five tests you can try out, each one with increasing difficulty. You may run ```make test TEST=3``` to run the test of your choice (default is 3).