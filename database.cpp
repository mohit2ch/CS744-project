#include "database.h"
#include <stdexcept>

PGConnectionPool::PGConnectionPool(const std::string &conninfo, size_t pool_size)
    : conninfo_(conninfo), pool_size_(pool_size) {
    pool_.reserve(pool_size_);
    used_.assign(pool_size_, false);
    for (size_t i = 0; i < pool_size_; ++i) {
        try {
            auto conn = std::make_shared<pqxx::connection>(conninfo_);
            if (!conn->is_open()) {
                throw std::runtime_error("Failed to open database connection");
            }
            pool_.push_back(conn);
        } catch (const std::exception &e) {
           
            throw std::runtime_error(std::string("Failed to create DB connection: ") + e.what());
        }
    }
}

PGConnectionPool::~PGConnectionPool() {
    std::lock_guard<std::mutex> lock(mu_);
    for (auto &c : pool_) {
        try {
            if (c && c->is_open()) c->close();
        } catch (...) {}
    }
    pool_.clear();
}

std::shared_ptr<pqxx::connection> PGConnectionPool::acquire() {
    std::unique_lock<std::mutex> lock(mu_);
    cv_.wait(lock, [this]() {
        for (bool b : used_) if (!b) return true;
        return false;
    });
    for (size_t i = 0; i < pool_size_; ++i) {
        if (!used_[i]) {
            used_[i] = true;
            std::shared_ptr<pqxx::connection> conn = pool_[i];
            auto pool_ptr = this;
            std::shared_ptr<pqxx::connection> wrapper(conn.get(),
                [conn, pool_ptr, i](pqxx::connection *) {
                    std::lock_guard<std::mutex> lock(pool_ptr->mu_);
                    pool_ptr->used_[i] = false;
                    pool_ptr->cv_.notify_one();
                }
            );
            return wrapper;
        }
    }
    
    throw std::runtime_error("Failed to acquire DB connection");
}
