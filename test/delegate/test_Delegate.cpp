#include <gtest/gtest.h>
#include "Delegate.h"
#include <iostream>


void Increase(int& b)
{
    ++b;
    std::cout << b << std::endl;
}


TEST(delegate, simple)
{
    int n = 0;
    hxk::SimpleDelegate<void(int&)> cb;
    cb += [](int &b){
        ++b;
        std::cout << b << std::endl;
    };
    cb += Increase;
    cb += [](int &b){
        ++b;
        std::cout << b << std::endl;
    };
    cb(n);
    EXPECT_TRUE(n == 3);
    cb -= Increase;
    cb(n);
    EXPECT_TRUE(n == 5);
}

TEST(delegate, operator_add)
{
    hxk::Delegate<void(int)> del;

    auto lambda1 = [](int x) { std::cout << "First lambda x = " << x << std::endl; };
    auto lambda2 = [](int x) { std::cout << "Second lambda x = " << x << std::endl; };
    auto lambda3 = [](int x) { std::cout << "Third lambda x = " << x << std::endl; };
   
    del += {lambda1, lambda2, lambda3};

    del(4);
}

TEST(delegate, subscriber1)
{
    hxk::Delegate<void(int)> del;

    auto lambda1 = [](int x) { std::cout << "First lambda x = " << x << std::endl; };
    auto lambda2 = [](int x) { std::cout << "Second lambda x = " << x << std::endl; };
    auto lambda3 = [](int x) { std::cout << "Third lambda x = " << x << std::endl; };
   
    del.subscribe(lambda1, {10, 13, 15});

    del.invoke();
}

TEST(delegate, subscriber2)
{
    hxk::Delegate<void(int)> del;

    auto lambda1 = [](int x) { std::cout << "First lambda x = " << x << std::endl; };
    auto lambda2 = [](int x) { std::cout << "Second lambda x = " << x << std::endl; };
    auto lambda3 = [](int x) { std::cout << "Third lambda x = " << x << std::endl; };
   
    del.subscribe({lambda1, lambda2, lambda3}, 21);

    del.invoke();
}

TEST(delegate, subscriber3)
{
    hxk::Delegate<void(int, float, std::string)> del;

    auto lambda = [](int x1, float x2, std::string x3)
    {
        std::cout << "x1 = " << x1 << ", x2 = " << x2 << ", x3 = " << x3 << std::endl;
    };

    // Subscribing one lambda with two tuples of parameters for later evaluation.
    del.subscribe(lambda, {{1, 3.5f, "foo"}, {2, 5.74f, "bar"}});

    del.invoke();
}


TEST(delegate, subscriber4)
{
    hxk::Delegate<void(int&)> del;
    auto lambda = [](int& x) { x++; };

    int y = 0;
    del.subscribe(lambda, y);

    // Calling only those functions that were subscribed to the delegate with Subscribe() method:
    del.invoke();
    EXPECT_TRUE(y == 1);
    // Calling all subscribed functions (saved parameters will not be used):
    
    int z = 0;
    del(z);
    EXPECT_TRUE(z == 1);
}

TEST(delegate, sub_return)
{
    hxk::ReturnDelegate<int(int&)> del;
    auto lambda = [](int& x) -> int { return x; };

    int y = 0;
    int z = 1;
    del.subscribe(lambda, {y,z});

    // Calling only those functions that were subscribed to the delegate with Subscribe() method:
    int a = del.invoke();
    std::cout << a << std::endl;
    EXPECT_TRUE(a == 1);
    // Calling all subscribed functions without saved parameters:
    int b = del(a);
    EXPECT_TRUE(b == 2);
}

TEST(delegate, Duplicating)
{
    hxk::Delegate<void(int)> del;
    auto lambda = [](int x) { std::cout << "x = " << x << std::endl; };

    del.subscribe(lambda, {4, 6, 8});

    // Duplicating the last subscribed function with the specified integer parameter 8:
    // Note that both postfix and prefix operators are valid.
    del++;

    del.invoke();
}

TEST(delegate, Removing)
{
    hxk::Delegate<void(int)> del;
    auto lambda = [](int x) { std::cout << "x = " << x << std::endl; };

    del.subscribe(lambda, {4, 6, 8});

    // Duplicating the last subscribed function with the specified integer parameter 8:
    // Note that both postfix and prefix operators are valid.
    del--;

    del.invoke();
}


TEST(delegate, D_R)
{
    hxk::Delegate<void(int)> del;
    auto lambda1 = [](int x) { std::cout << "x = " << x << std::endl; };
    auto lambda2 = [](int y) { std::cout << "y = " << y << std::endl; };

    del.subscribe(lambda1, {4, 6, 8});
    del.subscribe(lambda2, {-5, -7});

    std::cout << "Before:" << std::endl;
    del.invoke();

    // Removing 1 function from the end:
    del.remove(1);

    std::cout << "\nAfter removing 1 function from the end:" << std::endl;
    del.invoke();

    // Removing 2 functions from the beginning:
    del.remove(2, false);

    std::cout << "\nAfter removing 2 functions from the beginning: " << std::endl;
    del.invoke();
}

TEST(delegate, Combine)
{
    hxk::Delegate<void(int)> del1;
    hxk::Delegate<void(int)> del2;

    auto lambda1 = [](int x) { std::cout << "x = " << x << std::endl; };
    auto lambda2 = [](int y) { std::cout << "y = " << y << std::endl; };

    // Subscribing first lambda with two different parameters for later evaluation to the first delegate.
    del1.subscribe(lambda1, {10, 15});

    // Subscribing second lambda with two different parameters for later evaluation to the second delegate.
    del2.subscribe(lambda2, {-2, -6});

    // Combining first delegate with the second delegate.
    del1.combine(del2);

    del1.invoke();
}

TEST(delegate, shift)
{
    hxk::Delegate<void(int)> del1;
    hxk::Delegate<void(int)> del2;

    auto lambda1 = [](int x) { std::cout << "x = " << x << std::endl; };
    auto lambda2 = [](int y) { std::cout << "y = " << y << std::endl; };

    // Subscribing first lambda with two different parameters for later evaluation to the first delegate.
    del1.subscribe(lambda1, {10, 15});

    // Subscribing second lambda with two different parameters for later evaluation to the second delegate.
    del2.subscribe(lambda2, {-2, -6});

    del1 << del2;

    std::cout << "\nFirst delegate: " << std::endl;
    del1.invoke();

    std::cout << "\nSecond delegate: None" << std::endl;
    del2.invoke();
}

class Context
{
public:
    void increase(int& b)
    {
        ++b;
    }

    int add(int& c)
    {
        c += 1;
        return c;
    }
};

TEST(delegate, mem_func)
{
    Context c;
    int n = 0;
    hxk::MemberDelegate<Context, void(int&)> del;

    del += &Context::increase;
    // cb.subscribe(f, n);
    del(&c,n);
    EXPECT_TRUE(n == 1);
}

TEST(delegate, mem_func_ret)
{
    Context c;
    int n = 0;
    hxk::ReturnMemberDelegate<int, Context, int(int&)> del;

    del += &Context::add;
    // cb.subscribe(f, n);
    int x = del(&c,n);

    std::cout << x << std::endl;
    EXPECT_TRUE(n == 1);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
