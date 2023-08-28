#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <mutex>
#include <future>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include "code/util/noncopyable.h"

namespace hxk
{

class ThreadPool : public noncopyable
{
private:
    bool m_stop;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_mutex;
    std::condition_variable m_cond;
public:
    explicit ThreadPool(size_t thread_num = std::thread::hardware_concurrency())
    {
        for(size_t i = 0; i < thread_num; i++) {
            m_threads.emplace_back([this](){
                while (true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(m_mutex);
                        m_cond.wait(lock, [this](){
                            return m_stop || !m_tasks.empty();
                        });
                        if(m_stop && m_tasks.empty()){
                            return;
                        }
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }
                    task();
                } 
            });
        }
    }
    ~ThreadPool()
    {
        {
            std::unique_lock lock(m_mutex);
            m_stop = true;
        }
        m_cond.notify_all();
        for(auto& th : m_threads) {
            th.join();
        }
    }

    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>
                        (std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if(m_stop) {
                throw std::runtime_error("ThreadPool is stoped!");
            }
            m_tasks.emplace([task_ptr](){
                (*task_ptr)();
            });
        }
        m_cond.notify_one();
        return task_ptr->get_future();
    }

    template<typename F, typename... Args>
    auto execute(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
};


/*
	std::future可以保存某一函数的结果，std::future通常把线程的结果放到一个future对象中，另一个线程中可以wait或get这个将来的结果
	std::packaged_task提供对某函数的封装，然后同步/异步运行该函数，函数结果通常保存在其内部的一个future对象中
	std::make_shared可以返回一个指定类型的std::shared_ptr
*/
template<typename F, typename... Args>
auto ThreadPool::execute(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(stop) {
            throw std::runtime_error("ThreadPool has already stop!");
        }
        m_tasks.emplace([task](){
            (*task)();
        });

    }
    m_cond.notify_one();
    return res;
}

}

#endif