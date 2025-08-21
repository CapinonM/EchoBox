# Variables
SERVER_DIR = server
CLIENT_DIR = client
BUILD_DIR = build
SERVER_BIN = $(SERVER_DIR)/target/release/server
CLIENT_BIN = $(BUILD_DIR)/client

# C++ Compiler
CXX = g++
CXXFLAGS = -I$(CLIENT_DIR)/include -Wall -std=c++17

# Client Sourse files
CLIENT_SRC = $(wildcard $(CLIENT_DIR)/src/*.cpp)
CLIENT_OBJ = $(CLIENT_SRC:$(CLIENT_DIR)/src/%.cpp=$(BUILD_DIR)/%.o)

# Default rule
all: server client

# Compile server (Rust)
server:
	@echo "Compilando el servidor (Rust)..."
	cd $(SERVER_DIR) && cargo build --release

# Compile client (C++)
client: $(CLIENT_BIN)

$(CLIENT_BIN): $(CLIENT_OBJ)
	@echo "Linkeando el cliente..."
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CLIENT_OBJ) -o $(CLIENT_BIN)

$(BUILD_DIR)/%.o: $(CLIENT_DIR)/src/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean generated files
clean:
	@echo "Limpiando..."
	cd $(SERVER_DIR) && cargo clean
	rm -rf $(BUILD_DIR)

# Run server
run-server: server
	@echo "Ejecutando el servidor..."
	./scripts/run_server.sh

# Run client
run-client: client
	@echo "Ejecutando el cliente..."
	./scripts/run_client.sh

help:
	@echo "Targets:"
	@echo "  all         - Compile client and server"
	@echo "  server      - Compile just server (Rust)"
	@echo "  client      - Compile just client (C++)"
	@echo "  run-server  - Run server"
	@echo "  run-client  - Run client"
	@echo "  clean       - Clean generated files"

# Phony targets (not real files)
.PHONY: all server client clean run-server run-client