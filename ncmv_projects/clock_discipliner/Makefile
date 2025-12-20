# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2
LDFLAGS :=
LIBS := -lrt -lc

# Directories
SRC_DIR := .
OBJ_DIR := obj
BIN_DIR := bin

# Source files
SOURCES := test.cpp
OBJECTS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Target executable
TARGET := $(BIN_DIR)/clock_discipliner_test

# Default target
.PHONY: all
all: $(TARGET)

# Create directories if they don't exist
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Build target executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)
	@echo "Built $(TARGET)"

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Dependencies
$(OBJ_DIR)/clock_discipliner.o: clock_discipliner.cpp clock_discipliner.h
$(OBJ_DIR)/test.o: test.cpp clock_discipliner.h

# Clean build artifacts
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Cleaned build artifacts"

# Run the test program
.PHONY: run
run: $(TARGET)
	@echo "Running test program..."
	@sudo $(TARGET)

# Build with debug symbols
.PHONY: debug
debug: CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -g -O0 -DDEBUG
debug: clean all

# Display help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all     - Build the project (default)"
	@echo "  clean   - Remove build artifacts"
	@echo "  run     - Build and run the test program (requires sudo)"
	@echo "  debug   - Build with debug symbols"
	@echo "  help    - Display this help message"
