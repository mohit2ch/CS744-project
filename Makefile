CXX = g++
CXXFLAGS = -std=c++17 -O2
LDFLAGS_SERVER = -lpqxx -lpq -pthread
LDFLAGS_CLIENT = -pthread

SERVER_SRC = server.cpp database.cpp lru_cache.cpp
CLIENT_SRC = client.cpp
LOADGEN_SRC = load_generator.cpp
all: server client loadgen

server: $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $(SERVER_SRC) -o server $(LDFLAGS_SERVER)

client: $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) $(CLIENT_SRC) -o client $(LDFLAGS_CLIENT)

loadgen:
	$(CXX) $(CXXFLAGS) $(LOADGEN_SRC) -o loadgen $(LDFLAGS_CLIENT)

clean:
	rm -f server client loadgen

.PHONY: all clean