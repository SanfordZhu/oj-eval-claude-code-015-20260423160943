#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        std::cout << "Read line: '" << line << "'" << std::endl;
    }
    return 0;
}