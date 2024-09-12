#include <iostream>
#include <thread>
#include <atomic>

void increment(std::atomic<int>* a_ptr) {
    ++*a_ptr;
}

int main() {
    std::atomic<int> a = 0;

    std::thread th1(increment, &a);
    std::thread th2(increment, &a);

    th1.join();
    th2.join();

    std::cout << "a = " << a << '\n';
}
