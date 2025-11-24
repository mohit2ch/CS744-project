#include <iostream>
#include <string>
#include "httplib.h"

void print_menu() {
    std::cout << "\n==== KV Client ====\n"
              << "1. PUT (create/update)\n"
              << "2. GET (read)\n"
              << "3. DELETE (remove)\n"
              << "4. Health check\n"
              << "5. Exit\n"
              << "Choose: ";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <server_host> <port>\n";
        std::cerr << "Example: " << argv[0] << " 127.0.0.1 8080\n";
        return 1;
    }

    std::string host = argv[1];
    int port = std::stoi(argv[2]);

    httplib::Client cli(host, port);

    while (true) {
        print_menu();
        int choice;
        std::cin >> choice;

        if (choice == 5) break;

        if (choice == 1) {
            // PUT
            int key;
            std::string value;

            std::cout << "Enter key (int): ";
            std::cin >> key;
            std::cin.ignore();

            std::cout << "Enter value (string): ";
            std::getline(std::cin, value);

            auto res = cli.Put(("/kv/" + std::to_string(key)).c_str(), value, "text/plain");
            if (res) {
                std::cout << "Response: " << res->status << " => " << res->body << "\n";
            } else {
                std::cout << "Connection error!\n";
            }

        } else if (choice == 2) {
            // GET
            int key;
            std::cout << "Enter key: ";
            std::cin >> key;

            auto res = cli.Get(("/kv/" + std::to_string(key)).c_str());
            if (res) {
                std::cout << "Response: " << res->status << " => " << res->body << "\n";
            } else {
                std::cout << "Connection error!\n";
            }

        } else if (choice == 3) {
            // DELETE
            int key;
            std::cout << "Enter key: ";
            std::cin >> key;

            auto res = cli.Delete(("/kv/" + std::to_string(key)).c_str());
            if (res) {
                std::cout << "Response: " << res->status << " => " << res->body << "\n";
            } else {
                std::cout << "Connection error!\n";
            }

        } else if (choice == 4) {
            // Health
            auto res = cli.Get("/health");
            if (res) {
                std::cout << "Health: " << res->status << " => " << res->body << "\n";
            } else {
                std::cout << "Connection error!\n";
            }

        } else {
            std::cout << "Invalid choice.\n";
        }
    }

    return 0;
}
