// C++ sample code for Dining Philosophers problem.
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <mutex>
#include <thread>

const int PHI_CNT = 5;

enum {
    HUNGRY = 0,
    THINKING = 1,
    EATING = 2,
};

std::mutex m;
std::condition_variable cond[PHI_CNT];
std::atomic<bool> done;

int phi_states[PHI_CNT];
long phi_priorities[PHI_CNT];

long priority = 0;

void pick_up(int id);
void put_down(int id);
bool can_i_eat(int id);

void philosopher(int id) {
    while(!done) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand()%200));
        printf("[Philosopher %d] is hungry\n", id);
        pick_up(id);
        printf("[Philosopher %d] is eating\n", id);
        std::this_thread::sleep_for(std::chrono::milliseconds(rand()%200));
        put_down(id);
        printf("[Philosopher %d] is thinking\n", id);
        std::this_thread::sleep_for(std::chrono::milliseconds(rand()%200));
    }
    printf("[Philosopher %d] is done\n", id);
}

void pick_up(int id) {
    std::unique_lock<std::mutex> l(m);
    phi_states[id] = HUNGRY;
    priority++;
    phi_priorities[id] = priority;

    cond[id].wait(l, [&id]{return can_i_eat(id);});
    
    phi_states[id] = EATING;
}

void put_down(int id) {
    std::unique_lock<std::mutex> l(m);
    phi_states[id] = THINKING;
    //Notify neighbors
    cond[(id+1)%PHI_CNT].notify_one();
    cond[(id+PHI_CNT-1)%PHI_CNT].notify_one();
}

bool can_i_eat(int id) {
    int left = (id+PHI_CNT-1)%PHI_CNT;
    int right = (id+1)%PHI_CNT;
    if(phi_states[left]==EATING || phi_states[right]==EATING) {
        return false;
    }
    //Check if neighbors are waiting and have higher priority
    if(phi_states[left]==HUNGRY && phi_priorities[id]>phi_priorities[left]) {
        return false;
    }
    if(phi_states[right]==HUNGRY && phi_priorities[id]>phi_priorities[right]) {
        return false;
    }

    return true;
}

void stop() {
    done = true;
}

int main() {
    std::thread philosophers[PHI_CNT];
    int ids[PHI_CNT];
    for(int i=0; i<PHI_CNT; i++) {
        phi_states[i] = THINKING;
        phi_priorities[i] = 0;   
    }

    for(int i=0; i<PHI_CNT; i++) {
        ids[i] = i;
        philosophers[i] = std::thread(philosopher, ids[i]);
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
    stop();
    for(int i=0; i<PHI_CNT; i++) {
        philosophers[i].join();
    }
    return 0;
}
