#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <unordered_map>

const std::string DATA_FILE = "data.bin";

struct Entry {
    char index[65];
    int value;
};

// Simple file-based storage with minimal memory usage
class FileStorage {
public:
    void insert(const std::string& index, int value) {
        // First check if already exists by scanning file
        if (exists(index, value)) {
            return;
        }

        // Append to data file
        std::ofstream file(DATA_FILE, std::ios::binary | std::ios::app);
        if (!file.is_open()) return;

        Entry entry;
        std::memset(entry.index, 0, 65);
        std::strncpy(entry.index, index.c_str(), 64);
        entry.value = value;

        file.write(reinterpret_cast<const char*>(&entry), sizeof(Entry));
        file.close();
    }

    void remove(const std::string& index, int value) {
        std::ifstream in_file(DATA_FILE, std::ios::binary);
        if (!in_file.is_open()) return;

        std::vector<Entry> entries;
        Entry entry;

        while (in_file.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            if (index != entry.index || value != entry.value) {
                entries.push_back(entry);
            }
        }
        in_file.close();

        // Rewrite file
        std::ofstream out_file(DATA_FILE, std::ios::binary);
        for (const auto& e : entries) {
            out_file.write(reinterpret_cast<const char*>(&e), sizeof(Entry));
        }
        out_file.close();
    }

    std::vector<int> find(const std::string& index) {
        std::vector<int> result;
        std::ifstream file(DATA_FILE, std::ios::binary);

        if (!file.is_open()) {
            return result;
        }

        // Use a small buffer to reduce system calls
        const size_t BUFFER_SIZE = 1024; // Read multiple entries at once
        Entry buffer[BUFFER_SIZE];

        while (file.good()) {
            file.read(reinterpret_cast<char*>(buffer), sizeof(Entry) * BUFFER_SIZE);
            size_t count = file.gcount() / sizeof(Entry);

            for (size_t i = 0; i < count; ++i) {
                if (index == buffer[i].index) {
                    result.push_back(buffer[i].value);
                }
            }
        }
        file.close();

        std::sort(result.begin(), result.end());
        return result;
    }

private:
    bool exists(const std::string& index, int value) {
        std::ifstream file(DATA_FILE, std::ios::binary);
        if (!file.is_open()) return false;

        Entry entry;
        while (file.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            if (index == entry.index && value == entry.value) {
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    FileStorage storage;

    int n;
    std::cin >> n;
    std::cin.ignore();

    for (int i = 0; i < n; i++) {
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);

        std::string command;
        iss >> command;

        if (command == "insert") {
            std::string index;
            int value;
            iss >> index >> value;
            storage.insert(index, value);
        } else if (command == "delete") {
            std::string index;
            int value;
            iss >> index >> value;
            storage.remove(index, value);
        } else if (command == "find") {
            std::string index;
            iss >> index;
            std::vector<int> values = storage.find(index);

            if (values.empty()) {
                std::cout << "null\n";
            } else {
                for (size_t j = 0; j < values.size(); j++) {
                    if (j > 0) std::cout << " ";
                    std::cout << values[j];
                }
                std::cout << "\n";
            }
        }
    }

    return 0;
}