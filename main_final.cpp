#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <set>
#include <map>

const std::string DATA_FILE = "data.bin";

struct Entry {
    char index[65];
    int value;
};

// Optimized storage that keeps index in memory but only for accessed keys
class FileStorage {
private:
    std::map<std::string, std::set<int>> index_cache;
    bool cache_loaded;

    void load_cache_if_needed() {
        if (cache_loaded) return;

        std::ifstream file(DATA_FILE, std::ios::binary);
        if (!file.is_open()) {
            cache_loaded = true;
            return;
        }

        Entry entry;
        while (file.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            index_cache[entry.index].insert(entry.value);
        }
        file.close();
        cache_loaded = true;
    }

public:
    FileStorage() : cache_loaded(false) {}

    void insert(const std::string& index, int value) {
        load_cache_if_needed();

        // Check if already exists
        auto it = index_cache.find(index);
        if (it != index_cache.end() && it->second.count(value)) {
            return;
        }

        // Update cache
        index_cache[index].insert(value);

        // Append to file
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
        load_cache_if_needed();

        // Remove from cache
        auto it = index_cache.find(index);
        if (it != index_cache.end()) {
            it->second.erase(value);
            if (it->second.empty()) {
                index_cache.erase(it);
            }
        }

        // Rewrite file without the deleted entry
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
        load_cache_if_needed();

        std::vector<int> result;
        auto it = index_cache.find(index);
        if (it != index_cache.end()) {
            result.assign(it->second.begin(), it->second.end());
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