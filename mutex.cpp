// Basic C++ std::mutex Usage Example.
//

#include <iostream>
#include <mutex>
#include <thread>

int v1 = 0;
int v2 = 0;

std::mutex mtx;

void func1() {
    for(int i=0; i<100000; i++) v1++;
}

void func2() {
    for(int i=0; i<100000; i++) {
        std::lock_guard<std::mutex> lck(mtx);
        v2++;
    }
}

int main() {
    std::thread t1(func1);
    std::thread t2(func1);
    std::thread t3(func2);
    std::thread t4(func2);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    std::cout << "v1 = " << v1 << std::endl;
    std::cout << "v2 = " << v2 << std::endl;
    return 0;
}
