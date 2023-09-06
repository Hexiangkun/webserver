#include <iostream>
#include <thread>
#include "code/future/future.h"


template<typename Type>
void ThreadFunc(hxk::Promise<Type>& pm)
{
    static std::atomic<Type> v{10};
    std::this_thread::sleep_for(std::chrono::milliseconds(80/v));
    pm.SetValue(v++);
}

int main()
{
    std::vector<std::thread> threads;
    std::vector<hxk::Promise<int>> pmv(9);

    for(auto& pm :pmv) {
        std::thread t(ThreadFunc<int>, std::ref(pm));
        threads.emplace_back(std::move(t));
    }

    std::vector<hxk::Future<int>> futures;
    for(auto& pm :pmv) {
        futures.emplace_back(pm.GetFuture());
    }

    auto fall = hxk::WhenAll(std::begin(futures), std::end(futures));
    fall.Then([](const std::vector<hxk::Try<int>>& results) {
        std::cerr << "Then collet all! \n";
        for(auto& t:results) {
            std::cerr << t << std::endl;
        }
    });

    for(auto& t : threads) {
        t.join();
    }

    return 0;
}