#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <sstream>
#include <cstring>

const std::string DATA_FILE = "data.bin";
const std::string INDEX_FILE = "index.bin";

struct Entry {
    char index[65];
    int value;
    int next_offset;  // For chaining in case of hash collisions
};

class FileStorage {
private:
    std::fstream data_file;
    std::fstream index_file;
    std::unordered_map<std::string, std::set<int>> memory_index;

    void loadIndex() {
        std::ifstream idx_file(INDEX_FILE, std::ios::binary);
        if (!idx_file.is_open()) return;

        std::string key;
        int count;
        while (idx_file >> key >> count) {
            for (int i = 0; i < count; i++) {
                int value;
                idx_file >> value;
                memory_index[key].insert(value);
            }
        }
        idx_file.close();
    }

    void saveIndex() {
        std::ofstream idx_file(INDEX_FILE, std::ios::binary);
        if (!idx_file.is_open()) return;

        for (const auto& pair : memory_index) {
            idx_file << pair.first << " " << pair.second.size();
            for (int value : pair.second) {
                idx_file << " " << value;
            }
            idx_file << "\n";
        }
        idx_file.close();
    }

public:
    FileStorage() {
        data_file.open(DATA_FILE, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
        if (!data_file.is_open()) {
            std::ofstream create_file(DATA_FILE, std::ios::binary);
            create_file.close();
            data_file.open(DATA_FILE, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
        }
        loadIndex();
    }

    ~FileStorage() {
        saveIndex();
        if (data_file.is_open()) data_file.close();
    }

    void insert(const std::string& index, int value) {
        // Check if value already exists for this index
        if (memory_index.find(index) != memory_index.end() &&
            memory_index[index].find(value) != memory_index[index].end()) {
            return;  // Value already exists
        }

        // Add to memory index
        memory_index[index].insert(value);

        // Append to data file
        Entry entry;
        std::strncpy(entry.index, index.c_str(), 64);
        entry.index[64] = '\0';
        entry.value = value;
        entry.next_offset = -1;

        data_file.seekp(0, std::ios::end);
        data_file.write(reinterpret_cast<const char*>(&entry), sizeof(Entry));
        data_file.flush();
    }

    void remove(const std::string& index, int value) {
        auto it = memory_index.find(index);
        if (it == memory_index.end()) return;

        auto value_it = it->second.find(value);
        if (value_it == it->second.end()) return;

        it->second.erase(value_it);
        if (it->second.empty()) {
            memory_index.erase(it);
        }
    }

    std::vector<int> find(const std::string& index) {
        std::vector<int> result;
        auto it = memory_index.find(index);
        if (it != memory_index.end()) {
            result.assign(it->second.begin(), it->second.end());
            std::sort(result.begin(), result.end());
        }
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