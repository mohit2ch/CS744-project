#include <iostream>
#include <pqxx/pqxx>

int main() {
    std::cout << "libpqxx is working!" << std::endl;
    std::cout << "libpqxx version: " << PQXX_VERSION << std::endl;
    return 0;
}