# Makefile for UFM-Tooling library
# For g++ or clang++ on Windows (MinGW) or Linux

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS := 

# Directories
SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj
BIN_DIR := bin
EXAMPLE_DIR := examples

# Source files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Library name
LIB_NAME := libufmtooling.a

# Example
EXAMPLE_SRC := $(EXAMPLE_DIR)/example_usage.cpp
EXAMPLE_BIN := $(BIN_DIR)/example_usage

# Targets
.PHONY: all clean library example

all: library example

library: $(LIB_NAME)

$(LIB_NAME): $(OBJECTS)
	@mkdir -p $(dir $@)
	ar rcs $@ $^
	@echo "Library built: $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

example: $(EXAMPLE_BIN)

$(EXAMPLE_BIN): $(EXAMPLE_SRC) $(LIB_NAME)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< -L. -lufmtooling -o $@
	@echo "Example built: $@"

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_NAME)
	@echo "Clean complete"

# Dependencies
$(OBJECTS): $(wildcard $(INC_DIR)/*.h)
