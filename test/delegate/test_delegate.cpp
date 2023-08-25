#include "delegate.h"
#include <gtest/gtest.h>

void Increase(int& b)
{
    ++b;
    std::cout << b << std::endl;
}

TEST(delegate, c_func)
{
    int n = 0;
    hxk::Delegate<void(int&)> cb;

    cb += Increase;
    cb(n);
    EXPECT_TRUE(n == 1);
}

class Context
{
public:
    void increase(int& b)
    {
        ++b;
    }
};

TEST(delegate, mem_func)
{
    Context c;
    int n = 0;
    hxk::Delegate<void(int&)> cb;
    cb += std::bind(&Context::increase, &c, std::placeholders::_1);
    cb(n);
    EXPECT_TRUE(n == 1);
}
void IncreaseCopy(int b)
{
    ++b;
}

TEST(delegate, copt_f)
{
    int n = 0;
    hxk::Delegate<void(int)> cb;
    
    cb += IncreaseCopy;
    cb += [](int b){
        ++b;
    };
    cb(n);

    EXPECT_TRUE(n == 0);
}



TEST(delegate, lamba)
{
    int n = 0;
    hxk::Delegate<void(int&)> cb;
    cb += [](int &b){
        ++b;
    };
    cb(n);
    EXPECT_TRUE(n == 1);
}

TEST(delegate, operator_)
{
    int n = 0;
    hxk::Delegate<void(int&)> cb;
    cb += [](int &b){
        ++b;
    };
    cb += Increase;
    // cb -= Increase;  //todo
    cb(n);
    EXPECT_TRUE(n == 1);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

