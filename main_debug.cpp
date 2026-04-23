#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <sstream>

const std::string DATA_FILE = "data.bin";

struct Entry {
    char index[65];
    int value;
};

class FileStorage {
private:
    std::fstream data_file;

public:
    FileStorage() {
        // Create or open the data file
        std::ifstream check_file(DATA_FILE, std::ios::binary);
        bool file_exists = check_file.good();
        check_file.close();

        if (!file_exists) {
            // Create the file if it doesn't exist
            std::ofstream create_file(DATA_FILE, std::ios::binary);
            create_file.close();
        }

        data_file.open(DATA_FILE, std::ios::in | std::ios::out | std::ios::binary);
        if (!data_file.is_open()) {
            data_file.clear();
            data_file.open(DATA_FILE, std::ios::out | std::ios::binary);
            data_file.close();
            data_file.open(DATA_FILE, std::ios::in | std::ios::out | std::ios::binary);
        }
    }

    ~FileStorage() {
        if (data_file.is_open()) data_file.close();
    }

    void insert(const std::string& index, int value) {
        std::cerr << "Insert: " << index << " = " << value << std::endl;
        // Append new entry to the end of file
        Entry entry;
        std::memset(entry.index, 0, 65);
        std::strncpy(entry.index, index.c_str(), 64);
        entry.value = value;

        data_file.seekp(0, std::ios::end);
        data_file.write(reinterpret_cast<const char*>(&entry), sizeof(Entry));
        data_file.flush();
        std::cerr << "Written entry, file good: " << data_file.good() << std::endl;
    }

    void remove(const std::string& index, int value) {
        std::cerr << "Delete: " << index << " = " << value << std::endl;
        // We need to find and mark the entry as deleted
        // Since we can't easily delete from middle of file, we'll create a new file
        std::vector<Entry> entries;
        Entry entry;

        // Read all entries except the one to delete
        data_file.seekg(0, std::ios::beg);
        while (data_file.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            if (index != entry.index || value != entry.value) {
                entries.push_back(entry);
            }
        }

        // Rewrite the file with remaining entries
        data_file.close();
        std::ofstream new_file(DATA_FILE, std::ios::binary);
        for (const auto& e : entries) {
            new_file.write(reinterpret_cast<const char*>(&e), sizeof(Entry));
        }
        new_file.close();

        // Reopen for future operations
        data_file.open(DATA_FILE, std::ios::in | std::ios::out | std::ios::binary);
    }

    std::vector<int> find(const std::string& index) {
        std::cerr << "Find: " << index << std::endl;
        std::vector<int> result;
        Entry entry;

        // Scan through the file to find all matching entries
        data_file.seekg(0, std::ios::beg);
        int count = 0;
        while (data_file.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            count++;
            if (index == entry.index) {
                result.push_back(entry.value);
                std::cerr << "Found value: " << entry.value << std::endl;
            }
        }
        std::cerr << "Total entries scanned: " << count << std::endl;

        // Sort the result
        std::sort(result.begin(), result.end());
        return result;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    FileStorage storage;

    int n;
    std::cin >> n;
    std::cin.ignore();
    std::cerr << "n = " << n << std::endl;

    for (int i = 0; i < n; i++) {
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);

        std::string command;
        iss >> command;
        std::cerr << "Command: " << command << std::endl;

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