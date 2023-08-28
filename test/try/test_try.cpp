#include "try.h"
#include <iostream>
#include <exception>
int f()
{
    return 1;
}

template<typename T, typename... Args>
int test(hxk::Try<T> t)
{
    return f(t.template Get<Args>()...);
}

int main() {
    hxk::Try<void> t;
    std::cout << test<void>(t) << std::endl;

    return 0;
}

// 测试return value类型为int的函数
int test_func1(int num) 
{
    std::cout << num << std::endl;
    return num * 2;
}

// 测试return value类型为void的函数
void test_func2(int num) 
{
    std::cout << "Test Function 2" << std::endl;
}

int TEST() 
{
    // 测试return value类型为int的函数
    hxk::Try<int> result1 = hxk::WrapWithTry(test_func1, 10);
    if(result1.HasValue()) 
    {
        std::cout << "Result1: " << result1.Value() << std::endl;
    }
    else if(result1.HasException()) {
        std::cout << "Exception caught: ";
        std::rethrow_exception(result1.Exception()); 
        std::cout << std::endl;
    }

    // 测试return value类型为void的函数
    hxk::Try<void> result3 = hxk::WrapWithTry(test_func2, 10);
    if(result3.HasValue()) {
        std::cout << "Result3: Success" << std::endl;
    } else if(result3.HasException()) {
        std::cout << "Exception caught: " << std::endl;
        try
        {
            if (result3.Exception())
                std::rethrow_exception(result3.Exception());
            else {
                std::cout  << "nullptr"  << std::endl;
            }
        }
        catch(const std::exception& e)
        {
            std::cout << "Caught exception: '" << e.what() << "'\n";
        }
        
    }

    return 0;
}