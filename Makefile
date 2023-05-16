# Default values
HEURISTIC ?= 3
TEST ?= 3

# Compiler settings
CC = gcc
CFLAGS = -DHEURISTIC=$(HEURISTIC)
LDFLAGS =

# Directories
SRC_DIR = src
MODULES_DIR = modules
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = tests

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
MODULES_FILES = $(wildcard $(MODULES_DIR)/*.c)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES)) \
            $(patsubst $(MODULES_DIR)/%.c, $(OBJ_DIR)/%.o, $(MODULES_FILES))

# Executable program
EXEC_NAME = blocks
EXEC = $(BIN_DIR)/$(EXEC_NAME)

# OS
OS := LIN
ifeq ($(OS), WIN)
	CC = x86_64-w64-mingw32-$(CC)
	EXEC = $(BIN_DIR)/$(EXEC_NAME).exe
endif

# Targets
.PHONY:
	all clear help run

all: $(EXEC)

# Create the executable program
$(EXEC): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ -o $@

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(MODULES_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(EXEC)
	./$(EXEC)

# Run one of the tests
test: $(EXEC)
	./$(EXEC) < $(TEST_DIR)/test$(TEST).txt

# Clear files used by the program
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Use valgrind
help:
	valgrind --leak-check=full -v --show-leak-kinds=all --track-origins=yes ./$(EXEC)