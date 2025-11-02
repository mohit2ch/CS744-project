#include "httplib.h"
#include "LRU_cache.h"
#include <iostream>
#include <string>

LRU_Cache cache;
httplib::Server server;

void signalHandler(int){
    std::cout<<"Closing the server...\n";
    server.stop();
}

int main(int argc, char *argv[]){
    if(argc < 2){
        std::cout <<"Usage ./server <portno>"<<std::endl;
        return 1;
    }
    
    std::signal(SIGINT, signalHandler);

    
    int port = std::stoi(argv[1]);

    server.Get("/", [](const httplib::Request& req, httplib::Response& res){
        res.set_content("Hello client!", "text/plain");
    });

    server.Post("/create", [](const httplib::Request& req, httplib::Response& res){
        
        std::cout << "Create Request Received : "<<std::endl;
        
        auto key = req.get_param_value("key");
        auto value = req.get_param_value("value");
        
        std::cout<<"Key : "<<key<<std::endl;
        std::cout<<"Value : "<<value<<std::endl;
        
        if(cache.create(std::stoi(key), value) == 1)
            res.set_content("kv pair added succesfully", "text/plain");
        else 
            res.set_content("Entry for key already exists", "text/plain");
    });

    server.Post("/update", [](const httplib::Request& req, httplib::Response& res){
        
        std::cout<< "Update Request Received : "<<std::endl;
        
        auto key = req.get_param_value("key");
        auto value = req.get_param_value("value");
        
        std::cout<<"Key : "<<key<<std::endl;
        std::cout<<"Value : "<<value<<std::endl;
        
        if(cache.update(std::stoi(key), value) == 1)
            res.set_content("key value updated", "text/plain");
        else
            res.set_content("Entry for key doesn't exist", "text/plain");
    });

    server.Post("/delete", [](const httplib::Request& req, httplib::Response& res){
        
        std::cout<< "Delete Request Received : "<<std::endl;
        
        auto key = req.get_param_value("key");
        
        std::cout<<"Key : "<<key<<std::endl;
        
        if(cache.remove(std::stoi(key))==1)
            res.set_content("kv pair removed successfully", "text/plain");
        else
            res.set_content("Entry for given key doesn't exist", "text/plain");

    });
    
    server.Get("/read", [](const httplib::Request& req, httplib::Response& res){
        std::cout<< "Read Request Received : "<<std::endl;
        
        auto key = req.get_param_value("key");

        std::cout<< "Key : "<<key<<std::endl;
        auto value = cache.get(std::stoi(key));
        if(value=="-1")
            res.set_content("Entry for given key doesn't exist", "text/plain");
        else
            res.set_content("Key: "+key+" Value: "+value, "text/plain");
    });

    server.Get("/printcache", [](const httplib::Request& req, httplib::Response& res){
        cache.printCache();

        res.set_content("Cache printed", "text/plain"); 
    });

    std::cout << "Server started on localhost: "<<argv[1]<<std::endl;
    server.listen("localhost", port);
    return 0;
}