#include <typeinfo>
#include "helper.h"
#include <gtest/gtest.h>
#include <iostream>

#include <cassert>
#include <thread>
using namespace hxk::internal;

long f_void(void)
{
    return 1;
}

int f_int(int a)
{
    return a++;
}

void f_lvalue_int(int& a)
{
    ++a;
}

void f_rvalue_int(int&& a)
{
    ++a;
}


struct MyStruct {
    int x;
    double y;
};

TEST(call, res) 
{
    int myInt = 42;
    double myDouble = 3.14;
    MyStruct myStruct;

    const std::type_info& intType = typeid(myInt);
    const std::type_info& doubleType = typeid(myDouble);
    const std::type_info& structType = typeid(myStruct);

    std::cout << "myInt type: " << intType.name() << std::endl;
    std::cout << "myDouble type: " << doubleType.name() << std::endl;
    std::cout << "myStruct type: " << structType.name() << std::endl;
}

TEST(call, CallableResult)
{
    EXPECT_TRUE(typeid(long) == typeid(CallableResult<decltype(f_void), int>::Arg::Type));
    EXPECT_TRUE(typeid(int) == typeid(CallableResult<decltype(f_int), int>::Arg::Type));
    
    typedef typename CallableResult<decltype(f_void), int>::Arg Arguments;
    auto a = Arguments();
    std::cout << typeid(a).name() << std::endl;
    auto lbd = []()-> int {
        return 0;
    };

    EXPECT_TRUE(typeid(int) == typeid(CallableResult<decltype(lbd), void>::Arg::Type));
    EXPECT_TRUE(typeid(int) == typeid(CallableResult<decltype(lbd), int>::Arg::Type));
}


TEST(call, CanCallWith) {
    auto vlbd = []() {};
    EXPECT_TRUE((CanCallWith<decltype(vlbd)>::value));
    EXPECT_FALSE((CanCallWith<decltype(vlbd), int>::value));

    EXPECT_TRUE((CanCallWith<decltype(f_void)>::value));
    EXPECT_FALSE((CanCallWith<decltype(f_void), void>::value));
    EXPECT_FALSE((CanCallWith<decltype(f_void), int>::value));

    EXPECT_TRUE((CanCallWith<decltype(f_int), int>::value));
    EXPECT_TRUE((CanCallWith<decltype(f_int), int&>::value));
    EXPECT_TRUE((CanCallWith<decltype(f_int), int&&>::value));
    EXPECT_FALSE((CanCallWith<decltype(f_int)>::value));

    EXPECT_FALSE((CanCallWith<decltype(f_lvalue_int), int>::value));
    EXPECT_TRUE((CanCallWith<decltype(f_lvalue_int), int&>::value));
    EXPECT_FALSE((CanCallWith<decltype(f_lvalue_int), int&&>::value));
    EXPECT_FALSE((CanCallWith<decltype(f_lvalue_int)>::value));

    EXPECT_TRUE((CanCallWith<decltype(f_rvalue_int), int>::value));
    EXPECT_FALSE((CanCallWith<decltype(f_rvalue_int)>::value));
    EXPECT_FALSE((CanCallWith<decltype(f_rvalue_int), int&>::value));
    EXPECT_TRUE((CanCallWith<decltype(f_rvalue_int), int&&>::value));
}

// 定义一个用于测试的结构体
struct TestData {
    int value;

    TestData(int val) : value(val) {}
};

// TEST(call, CollectAllVariadicContext)
// {

//     // 创建一个 CollectAllVariadicContext 实例
//     CollectAllVariadicContext<int, std::string, TestData> context;

//     // 定义一个 lambda 函数，用于设置 partial result
//     auto setResult = [&](auto&& result, size_t index) {
//         context.SetPartialResult<decltype(result), index>(std::move(result));
//     };

//     // 创建一个线程，用于设置 partial result
//     std::thread t1(setResult, 42, 0);
//     std::thread t2(setResult, std::string("Hello"), 1);
//     std::thread t3(setResult, TestData(100), 2);

//     // 等待线程执行完毕
//     t1.join();
//     t2.join();
//     t3.join();

//     // 获取结果
//     auto future = context.pm.GetFuture();
//     auto result = future.Get();

    // 验证结果是否正确
    // if(std::get<0>(result) == 42);
    // assert(std::get<1>(result) == "Hello");
    // assert(std::get<2>(result).value == 100);

//     std::cout << "All tests passed!" << std::endl;
// }



int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}