#include "code/threadpool/Threadpool.h"
#include "code/future/Future.h"

#include <chrono>
#include <vector>
#include <gtest/gtest.h>
#include <iostream>
using namespace hxk;

class ThreadPoolTest : public testing::Test
{

public:
    ThreadPoolTest() = default;

    int ShortTask()
    {
        return 22;
    }

    int LongTask()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return 33;
    }

    void VoidTask(int& a)
    {
        std::cout << a << std::endl;
    }

public:
    ThreadPool pool_;
};


TEST_F(ThreadPoolTest, empty_test)
{
    EXPECT_EQ(pool_.GetWorkThreadsNum(), std::thread::hardware_concurrency());
    EXPECT_EQ(pool_.GetTasksNum(), 0);
}


TEST_F(ThreadPoolTest, exec_task_test)
{
    auto future = pool_.Execute(&ThreadPoolTest::ShortTask, this);
    EXPECT_EQ(future.Wait(), 22);
}

TEST_F(ThreadPoolTest, exec_many_task)
{
    auto start = std::chrono::steady_clock::now();
    std::vector<Future<int>> futures;
    for(int i = 0; i < std::thread::hardware_concurrency(); i++) {
        auto future = pool_.Execute(&ThreadPoolTest::LongTask, this);
        futures.emplace_back(std::move(future));
    }

    for(auto& f : futures) {
        void(f.Wait());
    }

    auto usedMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
    EXPECT_LE(usedMs.count(), 2 * std::chrono::milliseconds(1000).count());
}

TEST_F(ThreadPoolTest, exec_void_task)
{
    int c = 0;
    auto future = pool_.Execute(&ThreadPoolTest::VoidTask, this, std::ref(c));

    future.Wait();

    EXPECT_EQ(c, 0);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

