// std::promise and std::future which are used to return values from functions running in threads.
#include <future>
#include <iostream>
#include <string>
#include <thread>


void func1(std::promise<std::string>& p) {
    p.set_value("Future set by f1(promise& p)");
}

void func2(std::promise<std::string>&& p) {
    p.set_value("Future set by f2(promise&& p)");
}

void func3(std::promise<std::string>& p) {
    try {
        throw std::runtime_error("exception thrown from func3(promise<string>& p)");
    } catch(...) {
        p.set_exception(std::current_exception());
    }
}

void func4(std::promise<std::string>&& p) {
    try {
        throw std::runtime_error("exception thrown from func4(promise<string>& p)");
    } catch(...) {
        p.set_exception(std::current_exception());
    }
}

int main(){
    std::promise<std::string> p1, p2, p3, p4;
    auto f1 = p1.get_future();
    std::future<std::string> f2 = p2.get_future();
    auto f3 = p3.get_future();
    auto f4 = p4.get_future();

    std::thread t1(func1, std::ref(p1));
    std::thread t2(func2, std::move(p2));
    std::thread t3(func3, std::ref(p3));
    std::thread t4(func4, std::move(p4));

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::cout << "f1 = " << f1.get()<< std::endl;
    std::cout << "f2 = " << f2.get()<< std::endl;
    try {
        f3.get();
    } catch(std::exception& e) {
        std::cout << "catch f3 exception thrown by func3(promise<>& p)" << e.what() << std::endl;
    }
    try {
        f4.get();
    } catch(std::exception& e) {
        std::cout << "catch f4 exception thrown by func4(promise<>&7 p)" << e.what() << std::endl;
    }
    return 0;
}
