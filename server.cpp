#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <csignal>

#include "lru_cache.h"
#include "database.h"
#include "httplib.h"

int main(int argc, char* argv[]) {
   
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }

    int server_port = 0;
    try {
        server_port = std::stoi(argv[1]);
        if (server_port <= 0 || server_port > 65535)
            throw std::out_of_range("port out of range");
    } catch (...) {
        std::cerr << "Invalid port number: " << argv[1] << "\n";
        return 1;
    }

    const std::string listen_addr = "localhost";

    std::string conninfo = "postgresql://postgres:12345@localhost:5432/postgres";

    const size_t db_pool_size = 64;
    const size_t cache_capacity = 1000;

    try {
        PGConnectionPool dbpool(conninfo, db_pool_size);
        LRUCache cache(cache_capacity);

        httplib::Server svr;

        
        svr.Get("/health", [&](const httplib::Request&, httplib::Response& res){
            res.set_content("OK", "text/plain");
        });

       
        svr.Put(R"(/kv/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
            int key = std::stoi(req.matches[1]);
            std::string value = req.body;

            if (value.empty()) {
                res.status = 400;
                res.set_content("Empty value", "text/plain");
                return;
            }

            try {
                auto conn = dbpool.acquire();
                pqxx::work txn(*conn);

                txn.exec_params(
                    "INSERT INTO kv(key, value) VALUES($1, $2) "
                    "ON CONFLICT (key) DO UPDATE SET value = EXCLUDED.value",
                    key, value
                );

                txn.commit();
                cache.put(key, value);

                res.status = 200;
                res.set_content("OK", "text/plain");

            } catch (const std::exception &e) {
                res.status = 500;
                res.set_content(std::string("DB error: ") + e.what(), "text/plain");

            }
        });

       
        svr.Get(R"(/kv/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
            int key = std::stoi(req.matches[1]);

            auto cached = cache.get(key);
            if (cached) {
                res.status = 200;
                res.set_content(*cached, "text/plain");
                return;
            }

            try {
                auto conn = dbpool.acquire();
                pqxx::work txn(*conn);

                pqxx::result r = txn.exec_params("SELECT value FROM kv WHERE key = $1", key);

                if (r.empty()) {
                    res.status = 404;
                    res.set_content("Not found", "text/plain");
                    return;
                }

                std::string value = r[0][0].as<std::string>();
                cache.put(key, value);

                res.status = 200;
                res.set_content(value, "text/plain");

            } catch (const std::exception &e) {
                res.status = 500;
                res.set_content(std::string("DB error: ") + e.what(), "text/plain");

            }
        });

        // DELETE /kv/<key>
        svr.Delete(R"(/kv/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
            int key = std::stoi(req.matches[1]);

            try {
                auto conn = dbpool.acquire();
                pqxx::work txn(*conn);

                txn.exec_params("DELETE FROM kv WHERE key = $1", key);
                txn.commit();

                cache.remove(key);

                res.status = 200;
                res.set_content("Deleted", "text/plain");

            } catch (const std::exception &e) {
                res.status = 500;
                res.set_content(std::string("DB error: ") + e.what(), "text/plain");

            }
        });

        // Stats
        svr.Get("/stats", [&](const httplib::Request&, httplib::Response& res){
            std::ostringstream oss;
            oss << "cache_size=" << cache.size() << "\n";
            res.set_content(oss.str(), "text/plain");
        });

        // Start server
        std::cout << "Starting server on " << listen_addr << ":" << server_port << "...\n";
//         svr.new_task_queue = [] {
//     return new httplib::ThreadPool(100);
// };

        svr.listen(listen_addr.c_str(), server_port);

    } catch (const std::exception &e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
