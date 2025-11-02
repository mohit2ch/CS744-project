#include "Database.h"
#include <iostream>

Database::Database(const std::string conn_string) {
    try{
        conn_ = std::make_unique<pqxx::connection>(conn_string);
        if(conn_->is_open()){
            std::cout << "Connected to PostgreSQL database successfully!" <<std::endl;
            create_table();
        } else {
            std::cerr << "Failed to connect to database!" <<std::endl;
        }
    } catch(const std::exception& e){
        std::cerr << "Database connection error: "<<e.what() <<std::endl;
    }
}

Database::~Database(){
    if(conn_ && conn_->is_open()){
        conn_->close();
    }
}

bool Database::create_table(){
    try{
        pqxx::work transaction(*conn_);
        transaction.exec_params("CREATE TABLE IF NOT EXISTS kv_table (key INT PRIMARY KEY, value TEXT NOT NULL)");
        transaction.commit();
        return true;
    } catch (const std::exception& e){
        std::cerr <<"Table creation error: "<<e.what()<<std::endl;
        return false;
    }
}

bool Database::create_kvpair(int key, std::string value){
    std::lock_guard<std::mutex> lock(mutex_);
    try{
        pqxx::work transaction(*conn_);
        auto result = transaction.exec_params("SELECT value FROM kv_table WHERE key = $1", key);
        
        if(!result.empty()) return false;
        
        transaction.exec_params("INSERT INTO kv_table (key, value) VALUES ($1, $2)",key, value);
        transaction.commit();
        return true;
    } catch(const std::exception& e) {
        std::cerr << "Error in kv_pair creation : "<<e.what()<<std::endl;
        return false;
    }
}

bool Database::read_kvpair(int key, std::string &value){
    std::lock_guard<std::mutex> lock(mutex_);
    try{
        pqxx::work transaction(*conn_);
        
        auto result = transaction.exec_params("SELECT value FROM kv_table WHERE key = $1", key);
        
        if(result.empty()) return false;
        value = result[0][0].as<std::string>();
        return true;
    } catch(const std::exception& e){
        std::cerr << "Error in reading kv_pair : "<<e.what()<<std::endl;
        return false;
    }
}

bool Database::update_kvpair(int key, std::string &value){
    std::lock_guard<std::mutex> lock(mutex_);
    try{
        pqxx::work transaction(*conn_);
        
        auto result = transaction.exec_params("SELECT value FROM kv_table WHERE key = $1", key);
        if(result.empty()) return false;

        transaction.exec_params("UPDATE kv_table SET value = $2 WHERE key = $1", key, value);
        transaction.commit();
        return true;
    } catch(const std::exception& e) {
        std::cerr << "Error in kv_pair update : "<<e.what() <<std::endl;
        return false;
    }
}

bool Database::delete_kvpair(int key){
    std::lock_guard<std::mutex> lock(mutex_);
    try{
        pqxx::work transaction(*conn_);
        
        auto result = transaction.exec_params("SELECT value FROM kv_table WHERE key = $1", key);
        if(result.empty()) return false;

        transaction.exec_params("DELETE FROM kv_table WHERE key=$1", key);
        transaction.commit();
        return true;
    } catch(const std::exception& e) {
        std::cerr << "Error in kv_pair delete : "<<e.what() << std::endl;
        return false;
    }
}