// C++ std::async example.
#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>

std::string func1() {
    return "Future returned by func1";
}
std::string func2() {
   throw std::runtime_error("Exception thrown by func2"); 
}
void func3() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout<<"executing func3()" << std::endl;
}

int main() {
    auto f1 = std::async(func1);
    auto f2 = std::async(func2);
    auto f3 = std::async(func3);
    std::cout<< "f1 = " << f1.get() << std::endl;
    try {
        f2.get();
    } catch(std::exception& e) {
        std::cout << "Caught exception from f2 " << e.what() << std::endl;
    }

    return 0;
}
