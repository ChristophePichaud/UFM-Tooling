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

# Examples
EXAMPLE_SRC := $(EXAMPLE_DIR)/example_usage.cpp
EXAMPLE_BIN := $(BIN_DIR)/example_usage

LAYOUT_EXAMPLE_SRC := $(EXAMPLE_DIR)/layout_example.cpp
LAYOUT_EXAMPLE_BIN := $(BIN_DIR)/layout_example

# Targets
.PHONY: all clean library example examples

all: library examples

examples: example layout_example

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

layout_example: $(LAYOUT_EXAMPLE_BIN)

$(LAYOUT_EXAMPLE_BIN): $(LAYOUT_EXAMPLE_SRC) $(LIB_NAME)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< -L. -lufmtooling -o $@
	@echo "Layout example built: $@"

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_NAME)
	@echo "Clean complete"

# Dependencies
$(OBJECTS): $(wildcard $(INC_DIR)/*.h)
