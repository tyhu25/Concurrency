// Implements a semaphore class using std::mutex and std::condition_variable.
// Implements a producer consumer example based on implemented semaphore class.
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>


class semaphore {
public:
    semaphore(int cnt): count(cnt) {}
    void wait() {
        std::unique_lock<std::mutex> lck(mtx);
        cond.wait(lck, [this]{return count!=0;});
        count--;
    }
    void signal() {
        std::unique_lock<std::mutex> lck(mtx);
        count++;
        cond.notify_all();
    }

private:
    int count;
    std::mutex mtx;
    std::condition_variable cond;
};

const int COUNT = 20;
const int SIZE = 10;
std::queue<int> Q;
semaphore NOT_FULL(SIZE);
semaphore NOT_EMPTY(0);
std::mutex MTX;
std::atomic<int> producer_done;
bool done = false;

const int producer_cnt = 5;
const int consumer_cnt = 10;

std::mutex control_mtx;
std::condition_variable finish;

void control() {
    std::unique_lock<std::mutex> lck(control_mtx);
    finish.wait(lck, []{return producer_done==producer_cnt;});
    done = true;
    for(int i=0; i<consumer_cnt; i++) NOT_EMPTY.signal();
}

void produce(int id) {
    for(int i=0; i<COUNT; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand()%500));
        NOT_FULL.wait();
        std::unique_lock<std::mutex> lck(MTX);
        Q.push(i);
        printf("producer #%d produces item #%d\n", id, i);
        lck.unlock();
        NOT_EMPTY.signal();
    }
    ++producer_done;
    printf("producer #%d is done\n", id);
    finish.notify_one();
}

void consume(int id) {
    while(!done) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand()%200));
        NOT_EMPTY.wait();
        std::unique_lock<std::mutex> lck(MTX);
        if(done) break;
        printf("consumer #%d consumes item #%d\n", id, Q.front());
        Q.pop();
        lck.unlock();
        NOT_FULL.signal();
    }
    printf("consumes #%d is done\n", id);
}

int main() {
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    for(int i=0; i<producer_cnt; i++) {
        producers.push_back(std::thread(produce, i));
    }
    for(int i=0; i<consumer_cnt; i++) {
        consumers.push_back(std::thread(consume, i));
    }
    std::thread control_thread(control);
    for(auto& t:producers) t.join();
    for(auto& t:consumers) t.join();
    control_thread.join();
    return 0;
}
