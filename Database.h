#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <memory>
#include <pqxx/pqxx>
#include <mutex>

class Database {
private:
    std::unique_ptr<pqxx::connection> conn_;
    std::mutex mutex_;

public:
    Database(const std::string conn_string);
    ~Database();
    
    bool create_table();
    bool create_kvpair(int key, std::string value);
    bool read_kvpair(int key, std::string &value);
    bool update_kvpair(int key, std::string &value);
    bool delete_kvpair(int key);
    
};

#endif