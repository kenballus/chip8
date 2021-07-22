CPP := g++
CPPFLAGS := -lSDL2 -Wall -g

OBJ_DIR := obj
_OBJ := emulator
OBJ := $(patsubst %, $(OBJ_DIR)/%, $(_OBJ).o)

SRC_DIR := src

MAIN := $(SRC_DIR)/main.cpp

BINARY_NAME := chip8

.DEFAULT_GOAL := $(BINARY_NAME)

# Compile the target
$(BINARY_NAME): $(OBJ) $(MAIN)
	$(CPP) $(CPPFLAGS) $^ -o $@

# Compile all the object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp
	mkdir -p $(OBJ_DIR)
	$(CPP) $(CPPFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o $(BINARY_NAME)
	rmdir --ignore-fail-on-non-empty $(OBJ_DIR)
