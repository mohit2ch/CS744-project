# DECS Project Report  
**CS 744 : DECS Project : Phase 1**  
**Title:** Key-Value Server With Database and LRU Cache Integration  


---

## System Overview  
This project implements a **client–server key–value storage system** that provides both fast in-memory access and persistent database storage.  
It efficiently handles frequent read and write operations using an **LRU (Least Recently Used)** cache for hot data and **PostgreSQL** for long-term persistence.

---

## System Architecture  

The architecture follows a **modular design** consisting of three core components:

### 1. Client  
- Provides the interface for users or external applications to send key–value operations (GET, PUT, DELETE) to the server.  
- Communicates over TCP sockets using a simple **request–response protocol**.  

### 2. Server Core  
- Listens for client connections, parses incoming commands, and coordinates data access between the cache and the database.  
- **Read Request:**  
  - Checks the LRU cache.  
  - If found → returns value immediately.  
  - If not found → fetches from PostgreSQL, updates cache, and responds to the client.  
- **Write Request:**  
  - Updates both cache and database to maintain consistency.  

### 3. LRU Cache Module  
- Implemented using a combination of a **hash map** and a **doubly-linked list**.  
- Enables **O(1)** access, insertion, and eviction operations.  
- When full, the **least recently used** item is evicted automatically.  

### 4. Database Layer (PostgreSQL)  
- Acts as the **persistent storage backend**.  
- All key–value pairs are stored in a table:  

  ```sql
  kv_table(key TEXT PRIMARY KEY, value TEXT)
    ```
- Ensures data durability across restarts and allows recovery from cache losses.

## Data Flow Summary
1. *Client* → Server: Sends request (e.g., PUT key value or GET key)
2. *Server* → Cache: Checks or updates the LRU cache
3. *Server* → Database: Syncs persistent data if necessary
4. *Server* → Client: Sends back the response

## Key Features
- Hybrid Storage: Combines in-memory speed with persistent reliability.
- Scalable and Modular: Components can be extended or replaced independently.
- Efficient Caching: LRU policy ensures frequently accessed data remains hot.
- Crash Recovery: Persistent PostgreSQL storage guarantees data integrity.

