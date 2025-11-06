#include "httplib.h"
#include <iostream>
#include <string>
#include <fstream>

void interactive_console(httplib::Client &client){
    if(auto res = client.Get("/")){
        std::cout << "Status: "<<res->status << std::endl;
        std::cout << "Body: "<<res->body << std::endl;
    } else {
        std::cout << "Error: "<<res.error()<<std::endl;
    }
    httplib::Params params;
    while(1){
        params.clear();
        std::cout << " > ";
        std::cin.clear();
        std::string line;
        std::getline(std::cin, line);
        
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        std::istringstream iss(line);
        std::string command, key, value;
        iss >> command >> key;
        if(command == "exit"){
            break;
        }
        else if(command == "print"){
            if(auto res = client.Get("/printcache")){
                std::cout <<"Get Response: "<<res->body<<std::endl;
            }
        }
        else if(command == "delete"){
            
            params.emplace("key", key);
            if(auto res = client.Post("/delete", params)) {
                std::cout << "POST Response: "<< res->body << std::endl; 
            } 
        } else if(command == "create"){
            getline(iss, value);
            if (!value.empty() && value[0] == ' ') value.erase(0, 1);
            
            params.emplace("key", key);
            params.emplace("value", value);
            
            if(auto res = client.Post("/create",params)) {
                std::cout << "POST Response: "<< res->body << std::endl;
            }
        } else if(command == "update"){
            getline(iss, value);
            if (!value.empty() && value[0] == ' ') value.erase(0, 1);
            
            params.emplace("key", key);
            params.emplace("value", value);
            
            if(auto res = client.Post("/update",params)) {
                std::cout << "POST Response: "<< res->body << std::endl;
            }
        } else if(command == "read"){
            params.emplace("key", key);

            if(auto res = client.Get("/read?key="+key)) {
                std::cout << "GET Response: "<< res->body << std::endl;
            }
        }
    }
}

void file_console(httplib::Client &client, std::string& filename){
    
    std::ifstream file(filename);
    if(!file.is_open()) {
        std::cout << "Error: Could not open file " << filename << std::endl;
        return;
    }
    if(auto res = client.Get("/")){
        std::cout << "Status: "<<res->status << std::endl;
        std::cout << "Body: "<<res->body << std::endl;
    } else {
        std::cout << "Error: "<<res.error()<<std::endl;
    }
    httplib::Params params;
    std::string line;
    for(int line_number=1; getline(file, line); line_number++){
        params.clear();
        if(line.empty() || line[0]=='#') continue;

        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if(line.empty()) continue;

        std::istringstream iss(line);
        std::string command, key, value;
        iss >> command >> key;
        if(command == "exit"){
            break;
        }
        else if(command == "print"){
            if(auto res = client.Get("/printcache")){
                std::cout <<"Get Response: "<<res->body<<std::endl;
            }
        }
        else if(command == "delete"){
            
            params.emplace("key", key);
            if(auto res = client.Post("/delete", params)) {
                std::cout << "POST Response: "<< res->body << std::endl; 
            } 
        } else if(command == "create"){
            getline(iss, value);
            if (!value.empty() && value[0] == ' ') value.erase(0, 1);
            
            params.emplace("key", key);
            params.emplace("value", value);
            
            if(auto res = client.Post("/create",params)) {
                std::cout << "POST Response: "<< res->body << std::endl;
            }
        } else if(command == "update"){
            getline(iss, value);
            if (!value.empty() && value[0] == ' ') value.erase(0, 1);
            
            params.emplace("key", key);
            params.emplace("value", value);
            
            if(auto res = client.Post("/update",params)) {
                std::cout << "POST Response: "<< res->body << std::endl;
            }
        } else if(command == "read"){
            params.emplace("key", key);

            if(auto res = client.Get("/read?key="+key)) {
                std::cout << "GET Response: "<< res->body << std::endl;
            }
        }
        
    }
}

void console(httplib::Client &client){
    if(auto res = client.Get("/")){
        std::cout << "Status: "<<res->status << std::endl;
        std::cout << "Body: "<<res->body << std::endl;
    } else {
        std::cout << "Error: "<<res.error()<<std::endl;
    }
    httplib::Params params;
    params.emplace("key", "1");
    params.emplace("value", "John Doe");
    if(auto res = client.Post("/create",params)) {
        std::cout << "POST Response: "<< res->body << std::endl;
    }
    params.clear();
    params.emplace("key", "1");
    params.emplace("value", "Jane Day");
    if(auto res = client.Post("/update", params)) {
        std::cout << "POST Response: "<< res->body << std::endl;
    }
    params.clear();
    params.emplace("key", "1");
    if(auto res = client.Post("/delete", params)) {
        std::cout << "POST Response: "<< res->body << std::endl; 
    }
}

int main(int argc, char *argv[]){
    if(argc < 3){
        std::cout<<"Usage ./client <host> <portno>"<<std::endl;
        return 1;
    } 
    httplib::Client client(argv[1], std::stoi(argv[2]));
    
    // interactive_console(client);
    std::string filename = "commands.txt";
    file_console(client, filename);
    return 0;
}