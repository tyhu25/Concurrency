// Basic C++ std::thread example
// 1: function with value input.
// 2: function with reference input.
// 3: member function with value input.
// 4: member function with reference input.
// 5: functor.
// 6: lambada.
#include <iostream>
#include <sstream>
#include <thread>

void f1(int n) {
    for(int i=0; i<5; i++) {
        std::stringstream msg;
        msg <<"void f1(int n) is executing, n = "<< n <<std::endl;
        std::cout<<msg.str();
        ++n;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void f2(int &n) {
    for(int i=0; i<5; i++) {
        std::stringstream msg;
        msg<<"void f2(int& n) is executing, n = "<< n <<std::endl;
        std::cout<<msg.str();
        ++n;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

struct s1 {
    void f3(int n) {
        for(int i=0; i<5; i++) {
            std::stringstream msg;
            msg<<"void s1::f3(int n) is executing, n = "<< n <<std::endl;
            std::cout<<msg.str();
            ++n;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
    void f4(int& n) {
        for(int i=0; i<5; i++) {
            std::stringstream msg;
            msg<<"void s1::f4(int& n) is executing, n = "<< n <<std::endl;
            std::cout<<msg.str();
            ++n;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
    void operator()(int n) {
        for(int i=0; i<5; i++) {
            std::stringstream msg;
            msg<<"void s1()(int n) is executing, n = "<< n <<std::endl;
            std::cout<<msg.str();
            ++n;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

    }
};

int main() {
    int n1 = 0, n2 = 5, n3 = 10, n4 = 15, n5 = 20, n6 = 25;
    std::thread t1(f1, n1);
    std::thread t2(f2, std::ref(n2));
    s1 s1obj;
    std::thread t3(&s1::f3, &s1obj, n3);
    std::thread t4(&s1::f4, &s1obj, std::ref(n4));
    std::thread t5(s1obj, n5);
    auto foo = [](int n) {
        for(int i=0; i<5; i++) {
            std::stringstream msg;
            msg<<"void lambda[](int n) is executing, n = "<< n <<std::endl;
            std::cout<<msg.str();
            ++n;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    };
    std::thread t6(foo, n6);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    std::cout<<"After executing f1(int n), n1 = "<<n1<<std::endl;
    std::cout<<"After executing f2(int& n), n2 = "<<n2<<std::endl;
    std::cout<<"After executing s1::f3(int n), n3 = "<<n3<<std::endl;
    std::cout<<"After executing s1::f4(int& n), n4 = "<<n4<<std::endl;
    std::cout<<"After executing s1(int n), n5 = "<<n5<<std::endl;
    std::cout<<"After executing [](int n), n6 = "<<n6<<std::endl;
    return 0;
}
