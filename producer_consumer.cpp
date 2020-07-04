// A C++ producer consumer example using std::mutex + std::condition_variable.
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

const int item_cnt = 100;
const int queue_size = 10;
const int producer_cnt = 2;
const int consumer_cnt = 10;

class Container {
public:
    Container(int cnt, int s):total_cnt(cnt), size(s), done(0) {}
    void produce(int id) {
        for(int i=0; i<total_cnt; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(rand()%500));
            std::unique_lock<std::mutex> lck(mtx);
            not_full.wait(lck, [this]{return q.size()<size;});
            printf("producer #%d produces item #%d\n", id, i);
            q.push(i);
            lck.unlock();
            not_empty.notify_all();
        }
        done++;
        printf("producer #%d is done\n", id);
    }
    void consume(int id) {
        while(true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            std::unique_lock<std::mutex> lck(mtx);
            not_empty.wait(lck, [this]{return !q.empty() || done==producer_cnt;});
            if(done == producer_cnt) break;
            printf("consumer #%d consumes item #%d\n", id, q.front());
            q.pop();
            lck.unlock();
            not_full.notify_all();
        }
        printf("consumer #%d is done\n", id);
    }
private:
    std::mutex mtx;
    std::condition_variable not_full, not_empty;
    int total_cnt;
    int size;
    std::atomic<int> done;
    std::queue<int> q;
};

int main() {
    Container c(item_cnt, queue_size);
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    for(int i=0; i<producer_cnt; i++) {
        std::thread t(&Container::produce, &c, i);
        producers.push_back(std::move(t));
    } 
    for(int i=0; i<consumer_cnt; i++) {
        std::thread t(&Container::consume, &c, i);
        consumers.push_back(std::move(t));
    }
    for(auto& t:producers) t.join();
    for(auto& t:consumers) t.join();

    return 0;
}

