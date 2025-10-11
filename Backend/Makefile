# Makefile for Budgeteer API
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
TARGET = BudgeteerAPI
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Default target
all: directories $(BIN_DIR)/$(TARGET)

# Create necessary directories
directories:
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"
	@if not exist "$(BIN_DIR)\SampleDataset" mkdir "$(BIN_DIR)\SampleDataset"
	@xcopy /E /I /Y "SampleDataset" "$(BIN_DIR)\SampleDataset" > nul

# Link object files to create executable
$(BIN_DIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo Build complete! Executable: $(BIN_DIR)/$(TARGET).exe

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	@if exist "$(OBJ_DIR)" rmdir /S /Q "$(OBJ_DIR)"
	@if exist "$(BIN_DIR)" rmdir /S /Q "$(BIN_DIR)"
	@echo Clean complete!

# Run the application
run: all
	@cd $(BIN_DIR) && $(TARGET).exe

# Rebuild
rebuild: clean all

.PHONY: all clean run rebuild directories
