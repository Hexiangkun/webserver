#include <typeinfo>
#include "code/future/Helper.h"
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

template<typename T>
struct MY
{
    T value;
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


int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}