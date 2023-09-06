#include "code/future/try.h"
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

int divide(int a, int b)
{
    if (b == 0)
        throw std::runtime_error("Division by zero");
    return a / b;
}

void test_trywithwrap()
{
    try
    {
        auto result = hxk::WrapWithTry(divide, 10, 2);
        if (result.HasException())
        {
            std::cout << "Exception occurred: " << std::endl;
        }
        else
        {
            std::cout << "Result: " << result.Value() << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception occurred: " << e.what() << std::endl;
    }

    try
    {
        auto result = hxk::WrapWithTry(divide, 10, 0);
        if (result.HasException())
        {
            std::cout << "Exception occurred: " << std::endl;
        }
        else
        {
            std::cout << "Result: " << result.Value() << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception occurred: " << e.what() << std::endl;
    }
}

int TEST() 
{
    // 测试return value类型为int的函数
    hxk::Try<int> result1 = hxk::WrapWithTry(test_func1, 10);
    if(result1.HasValue()) 
    {
        std::cout << "Result1: " << result1.Value() << std::endl;
        std::cout << "Result1: " << result1 << std::endl;
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

//测试value
void test_conference()
{
    hxk::Try<int> tryObj(42);

    const int& value1 = tryObj.Value();  // 获取存储的值的常量引用
    int& value2 = tryObj.Value();        // 获取存储的值的左值引用
    int&& value3 = std::move(tryObj).Value();  // 获取存储的值的右值引用

    std::cout << value1 << std::endl;
    std::cout << value2 << std::endl;
    std::cout << value3 << std::endl;
}

//重载*
void test_slove()
{
    hxk::Try<int> tryObj(42);

    // 解引用操作符的使用
    std::cout << "Value: " << *tryObj << std::endl;

    // 修改存储的值
    *tryObj = 100;

    // 再次使用解引用操作符获取修改后的值
    std::cout << "Modified Value: " << *tryObj << std::endl;
}

//测试隐式转换
void PrintValue(const int& value) {
    std::cout << "Value: " << value << std::endl;
}

void test_implict_convert()
{
    hxk::Try<int> tryObj(42);

    // 隐式转换为const int&
    PrintValue(tryObj);

    // 隐式转换为int&
    int& valueRef = tryObj;
    valueRef = 100;

    // 隐式转换为int&&
    int&& valueRRef = std::move(tryObj);

    std::cout << "Modified Value: " << valueRRef << std::endl;

}

void test_Get()
{
    hxk::Try<int> tryObj(42);
    int intValue = tryObj.Get<int>();
    std::cout << "Value: " << intValue << std::endl;

    hxk::Try<char> tryObj1('a');
    int intValue1 = tryObj1.Get<int>();
    std::cout << "Value: " << intValue1 << std::endl;

    hxk::Try<std::string> tryObj2("Hello");
    std::string strValue = tryObj2.Get<std::string>();
    std::cout << "Value: " << strValue << std::endl;
}

void test_void()
{
    hxk::Try<void> tryObj1;
    std::cout << "HasException: " << tryObj1.HasException() << std::endl;

    // Test Try<void> constructor with exception
    std::exception_ptr exceptionPtr = std::make_exception_ptr(std::runtime_error("Test exception"));
    hxk::Try<void> tryObj2(exceptionPtr);
    std::cout << "HasException: " << tryObj2.HasException() << std::endl;

    // Test Try<void> move constructor
    hxk::Try<void> tryObj3(std::move(tryObj2));
    std::cout << "HasException: " << tryObj3.HasException() << std::endl;

    // Test Try<void> move assignment operator
    tryObj1 = std::move(tryObj3);
    std::cout << "HasException: " << tryObj1.HasException() << std::endl;

    // Test Try<void> copy constructor
    hxk::Try<void> tryObj4(tryObj1);
    std::cout << "HasException: " << tryObj4.HasException() << std::endl;

    // Test Try<void> copy assignment operator
    tryObj3 = tryObj4;
    std::cout << "HasException: " << tryObj3.HasException() << std::endl;

    // Test Exception() function
    try {
        std::exception_ptr& exception = tryObj3.Exception();
        std::rethrow_exception(exception);
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    // Test Check() function
    // tryObj3.Check();
}

int main() {
    hxk::Try<void> t;
    std::cout << test<void>(t) << std::endl;

    TEST();

    test_conference();

    test_slove();

    test_implict_convert();

    test_Get();

    test_void();

    test_trywithwrap();
    return 0;
}
