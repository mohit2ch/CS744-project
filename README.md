# Installation and Running

1. Clone the repository:
   git clone https://github.com/mohit2ch/CS744-project.git
   cd CS744-project

2. Build the project:
   make

3. Configure PostgreSQL:
   - Create database and table.
   - Update connection info inside source files if needed.

4. Run the server:
   ```./server 5000```

5. Run the client:
   ```./client <host> <port>```

6. Run the load generator:

  ```./loadgen --host <host> --port <port> --threads <N> --duration ```<sec>

# File Structure

- **server.cpp**: Implements HTTP KV server.
- **client.cpp**: Simple interactive test client.
- **load_generator.cpp**: Multi-threaded load generator.
- **database.cpp**: PostgreSQL connection pool.
- **database.h**: Database interface.
- **lru_cache.cpp**: LRU cache implementation.
- **lru_cache.h**: Header for cache.
- **httplib.h**: HTTP server/client library.
- **Makefile**: Build rules.