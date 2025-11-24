#ifndef DATABASE_H
#define DATABASE_H

#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <pqxx/pqxx>

// Simple connection pool for pqxx::connection
class PGConnectionPool {
public:
    // conninfo: libpq connection string: "host=... port=... dbname=... user=... password=..."
    // pool_size: number of persistent connections
    PGConnectionPool(const std::string &conninfo, size_t pool_size);
    ~PGConnectionPool();

    // Acquire a connection. Blocks if none available.
    std::shared_ptr<pqxx::connection> acquire();

    // Returns connection to pool (done automatically when shared_ptr destructor runs).
    // We return shared_ptr with custom deleter that simply returns to pool - but here,
    // for simplicity we'll keep shared connections and reuse them.
private:
    std::string conninfo_;
    size_t pool_size_;
    std::vector<std::shared_ptr<pqxx::connection>> pool_;
    std::mutex mu_;
    std::condition_variable cv_;
    std::vector<bool> used_;
};

#endif // DATABASE_H
