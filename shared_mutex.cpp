// Implement a reader/writer example using std::shared_timed_mutex.
#include <cstdlib>
#include <unordered_map>
#include <mutex> // For std::unique_lock
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>

class Database {
public:
    void read(int id, const std::string& key) {
        std::shared_lock<std::shared_timed_mutex> lck(mtx);
        if(!entities.count(key)) {
            printf("[Reader %d] No record found for key: %s\n", id, key.c_str());
            return;
        }
        printf("[Reader %d] Reads %s->%s\n", id, key.c_str(), entities[key].c_str());
    }
    void write(int id, std::string key, std::string val) {
        std::unique_lock<std::shared_timed_mutex> lck(mtx);
        printf("[Writer %d] Writes %s->%s\n", id, key.c_str(), val.c_str());
        entities[key] = val;
    }

private:
    std::shared_timed_mutex mtx;
    std::unordered_map<std::string, std::string> entities;
};

int main() {
    const int WRITER_CNT = 2;
    const int READER_CNT = 10;
    std::vector<std::thread> readers;
    std::vector<std::thread> writers;
    std::vector<std::string> keys = {"k1", "k2", "k3", "k4", "k5"};
    std::vector<std::string> vals = {"v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10"};
    Database db;
    for(int i=0; i<WRITER_CNT; i++) {
        int keyIdx = rand()%keys.size();
        int valIdx = rand()%vals.size();
        std::thread t(&Database::write, &db, i, keys[keyIdx], vals[valIdx]);
        writers.push_back(std::move(t));
    }
    for(int i=0; i<READER_CNT; i++) {
        int idx = rand()%keys.size();
        std::thread t(&Database::read, &db, i, keys[idx]);
        readers.push_back(std::move(t));
    }
    for(auto& t:writers) t.join();
    for(auto& t:readers) t.join();

    return 0;
}


