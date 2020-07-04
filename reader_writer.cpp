# Implement a C++ reader writer lock using std::mutex and std::condition_variable.
#include <chrono>
#include <cstdlib>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

struct ReaderWriterLock {
public:
    ReaderWriterLock():waiting(0), reader(0), writer(0) {}
    void lockRead() {
        std::unique_lock<std::mutex> lck(mtx);
        readOk.wait(lck, [this]{ return waiting==0 && writer==0; });
        reader++;
    }
    void unlockRead() {
        std::unique_lock<std::mutex> lck(mtx);
        reader--;
        if(reader==0) writeOk.notify_all();
    }
    void lockWrite() {
        std::unique_lock<std::mutex> lck(mtx);
        waiting++;
        writeOk.wait(lck, [this]{ return reader==0 && writer==0 ;});
        waiting--;
        writer++;
    }
    void unlockWrite() {
        std::unique_lock<std::mutex> lck(mtx);
        writer--;
        if(writer==0) {
            writeOk.notify_all();
            readOk.notify_all();
        }
    }
private:
    int waiting;
    int reader;
    int writer;
    std::mutex mtx;
    std::condition_variable readOk;
    std::condition_variable writeOk;
};

ReaderWriterLock rwLock;
std::unordered_map<int, int> entries;

bool done = false;
const int MAX_KEY = 10;
const int MAX_VALUE = 100;
const int NUMBER_OF_WRITES = 20;
const int READER_CNT = 10;
const int WRITER_CNT = 2;

void read(int id) {
    while(!done) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand()%200));
        int key = rand()%(MAX_KEY+1);
        rwLock.lockRead();
        if(entries.count(key)) {
            printf("[Reader %d] reads %d->%d\n", id, key, entries[key]);
        } else {
            printf("[Reader %d] no value found for key: %d\n", id, key);
        }
        rwLock.unlockRead();
    }
}

void write(int id) {
    for(int i=0; i<NUMBER_OF_WRITES; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand()%1000));
        int key = rand()%(MAX_KEY+1);
        int value = rand()%MAX_VALUE;
        rwLock.lockWrite();
        printf("[Writer %d] writes %d->%d\n", id, key, value);
        entries[key] = value;
        rwLock.unlockWrite();
    }
    done = true;
}

int main() {
    std::vector<std::thread> readers;
    std::vector<std::thread> writers;
    for(int i=0; i<READER_CNT; i++) {
        readers.push_back(std::thread(read, i));
    }
    for(int i=0; i<WRITER_CNT; i++) {
        writers.push_back(std::thread(write, i));
    }
    for(auto& t:writers) t.join();
    for(auto& t:readers) t.join();

    return 0;
}
