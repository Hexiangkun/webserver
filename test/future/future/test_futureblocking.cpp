// #include <thread>
// #include <iostream>

// #include "code/future/future.h"

// template<typename Type>
// void ThreadFunc(hxk::Promise<Type>& pm)
// {
//     static Type v = 10;
//     std::this_thread::sleep_for(std::chrono::milliseconds(v));
//     try
//     {
//         pm.SetValue(v++);
//     }
//     catch(const std::exception& e)
//     {
//         pm.SetException(std::make_exception_ptr(e));
//     }
    
// }

// int main()
// {
//     hxk::Promise<int> pm;

//     std::thread t(ThreadFunc<int>, std::ref(pm));
//     hxk::Future<int> fut = pm.GetFuture();
//     try
//     {
//         int v = fut.Wait(std::chrono::milliseconds(10));
//         std::cout << "Got" << v << " from future." << std::endl;
//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << "Got future exception: " << e.what() << '\n';
//     }

//     t.join();

//     return 0;
// }

#include <thread>
#include <iostream>
#include "code/future/future.h"

using namespace hxk;

using std::cout;
using std::endl;

template <typename Type>
void ThreadFunc(Promise<Type>& pm) {
    static Type v = 10;
    std::this_thread::sleep_for(std::chrono::milliseconds(v));
    try {
        pm.SetValue(v++);
        //throw std::runtime_error("ThreadFunc exception");
    } catch (const std::exception& e) {
        pm.SetException(std::make_exception_ptr(e));
    }
}

int main() {
    Promise<int> pm;
    std::thread t(ThreadFunc<int>, std::ref(pm));

    Future<int> fut = pm.GetFuture();
    try {
        int v = fut.Wait(std::chrono::milliseconds(10));
        cout << "Got " << v << " from future." << endl;
    } catch(const std::exception& e) {
        cout << "Got future exception: " << e.what() << endl;
    }

    t.join();

    return 0;
}

