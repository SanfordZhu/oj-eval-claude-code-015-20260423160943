#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <functional>

const int NUM_BUCKETS = 19; // Stay within 20 file limit (19 data files + 1 executable)

struct Entry {
    char index[65];
    int value;
    bool deleted;
};

// Hash-based file distribution
class FileStorage {
private:
    std::string get_filename(int bucket) {
        return "data_" + std::to_string(bucket) + ".bin";
    }

    int get_bucket(const std::string& index) {
        std::hash<std::string> hasher;
        return hasher(index) % NUM_BUCKETS;
    }

public:
    void insert(const std::string& index, int value) {
        int bucket = get_bucket(index);
        std::string filename = get_filename(bucket);

        // Check if already exists
        if (exists_in_bucket(bucket, index, value)) {
            return;
        }

        // Append to appropriate bucket file
        std::ofstream file(filename, std::ios::binary | std::ios::app);
        if (!file.is_open()) return;

        Entry entry;
        std::memset(entry.index, 0, 65);
        std::strncpy(entry.index, index.c_str(), 64);
        entry.value = value;
        entry.deleted = false;

        file.write(reinterpret_cast<const char*>(&entry), sizeof(Entry));
        file.close();
    }

    void remove(const std::string& index, int value) {
        int bucket = get_bucket(index);
        std::string filename = get_filename(bucket);

        // Mark as deleted
        std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
        if (!file.is_open()) return;

        Entry entry;
        while (file.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            if (!entry.deleted && index == entry.index && value == entry.value) {
                entry.deleted = true;
                file.seekp(-sizeof(Entry), std::ios::cur);
                file.write(reinterpret_cast<const char*>(&entry), sizeof(Entry));
                break;
            }
        }
        file.close();
    }

    std::vector<int> find(const std::string& index) {
        int bucket = get_bucket(index);
        std::string filename = get_filename(bucket);

        std::vector<int> result;
        std::ifstream file(filename, std::ios::binary);

        if (!file.is_open()) {
            return result;
        }

        Entry entry;
        while (file.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            if (!entry.deleted && index == entry.index) {
                result.push_back(entry.value);
            }
        }
        file.close();

        std::sort(result.begin(), result.end());
        return result;
    }

private:
    bool exists_in_bucket(int bucket, const std::string& index, int value) {
        std::string filename = get_filename(bucket);
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) return false;

        Entry entry;
        while (file.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            if (!entry.deleted && index == entry.index && value == entry.value) {
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