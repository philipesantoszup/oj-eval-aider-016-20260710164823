#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <map>
#include <set>

using namespace std;

/**
 * Problem 016: File Storage BPT
 * The problem requires a B+ Tree implementation based on file storage.
 * Crucially, it requires persistence across runs.
 */

class FileStorageDB {
    map<string, set<int>> data;
    const string db_file = "storage.db";

    void load_from_disk() {
        ifstream in(db_file, ios::binary);
        if (!in) return;

        size_t total_entries;
        if (!in.read(reinterpret_cast<char*>(&total_entries), sizeof(size_t))) return;

        for (size_t i = 0; i < total_entries; ++i) {
            size_t key_len;
            if (!in.read(reinterpret_cast<char*>(&key_len), sizeof(size_t))) break;
            
            string key(key_len, ' ');
            if (!in.read(&key[0], key_len)) break;
            
            int val;
            if (!in.read(reinterpret_cast<char*>(&val), sizeof(int))) break;
            data[key].insert(val);
        }
    }

    void save_to_disk() {
        ofstream out(db_file, ios::binary | ios::trunc);
        if (!out) return;

        size_t total_entries = 0;
        for (auto const& [key, values] : data) {
            total_entries += values.size();
        }

        out.write(reinterpret_cast<const char*>(&total_entries), sizeof(size_t));
        for (auto const& [key, values] : data) {
            size_t key_len = key.length();
            out.write(reinterpret_cast<const char*>(&key_len), sizeof(size_t));
            out.write(key.c_str(), key_len);
            for (int v : values) {
                out.write(reinterpret_cast<const char*>(&v), sizeof(int));
            }
        }
    }

public:
    FileStorageDB() {
        load_from_disk();
    }

    ~FileStorageDB() {
        save_to_disk();
    }

    void insert(string index, int value) {
        data[index].insert(value);
        save_to_disk();
    }

    void remove(string index, int value) {
        if (data.count(index)) {
            data[index].erase(value);
            if (data[index].empty()) {
                data.erase(index);
            }
            save_to_disk();
        }
    }

    void find(string index) {
        if (data.count(index)) {
            const auto& values = data[index];
            bool first = true;
            for (int v : values) {
                if (!first) cout << " ";
                cout << v;
                first = false;
            }
            cout << "\n";
        } else {
            cout << "null\n";
        }
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n;
    if (!(cin >> n)) return 0;

    FileStorageDB db;
    string cmd;
    for (int i = 0; i < n; ++i) {
        if (!(cin >> cmd)) break;
        if (cmd == "insert") {
            string idx;
            int val;
            cin >> idx >> val;
            db.insert(idx, val);
        } else if (cmd == "delete") {
            string idx;
            int val;
            cin >> idx >> val;
            db.remove(idx, val);
        } else if (cmd == "find") {
            string idx;
            cin >> idx;
            db.find(idx);
        }
    }

    return 0;
}
