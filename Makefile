# Compiler
CXX = g++

# Base compiler flags
CXXFLAGS_BASE = -I./src -Wall -std=c++2a 

# Release-specific flags
CXXFLAGS_RELEASE = $(CXXFLAGS_BASE) -DNDEBUG -O2

# Test-specific flags
CXXFLAGS_TEST = $(CXXFLAGS_BASE) -pg -O1

# Debug-specific flags
CXXFLAGS_DEBUG = $(CXXFLAGS_BASE) -g -fsanitize=address -DDEBUG -O1

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files
SRC = $(wildcard $(SRC_DIR)/*.cpp)
HEADERS = $(wildcard $(SRC_DIR)/*.h)

# Object files for release and debug builds
OBJ_RELEASE = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/release/%.o,$(SRC))
OBJ_TEST = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/test/%.o,$(SRC))
OBJ_DEBUG = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/debug/%.o,$(SRC))

# Target binaries
TARGET_RELEASE = $(BIN_DIR)/csc2525-project
TARGET_TEST = $(BIN_DIR)/csc2525-project-test
TARGET_DEBUG = $(BIN_DIR)/csc2525-project-debug

# Create necessary directories
$(shell mkdir -p $(BUILD_DIR)/release $(BUILD_DIR)/test $(BUILD_DIR)/debug $(BIN_DIR))

# Default target
all: release test debug

# Release target
release: $(TARGET_RELEASE)

# Test target
test: $(TARGET_TEST)

# Debug target
debug: $(TARGET_DEBUG)

# Linking the release binary
$(TARGET_RELEASE): $(OBJ_RELEASE)
	$(CXX) $(CXXFLAGS_RELEASE) -o $@ $^

# Linking the test binary
$(TARGET_TEST): $(OBJ_TEST)
	$(CXX) $(CXXFLAGS_TEST) -o $@ $^

# Linking the debug binary
$(TARGET_DEBUG): $(OBJ_DEBUG)
	$(CXX) $(CXXFLAGS_DEBUG) -o $@ $^

# Compiling release object files
$(BUILD_DIR)/release/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_RELEASE) -c $< -o $@

# Compiling test object files
$(BUILD_DIR)/test/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_TEST) -c $< -o $@

# Compiling debug object files
$(BUILD_DIR)/debug/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_DEBUG) -c $< -o $@

# Clean target to remove generated files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all release test debug clean