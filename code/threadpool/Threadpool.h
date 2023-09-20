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
#include "code/util/Noncopyable.h"
#include "code/future/Future.h"
#include <iostream>
namespace hxk
{

class ThreadPool : public Noncopyable
{
private:
    bool m_stop;
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    mutable std::mutex m_mutex;
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
            std::unique_lock<std::mutex> lock(m_mutex);
            m_stop = true;
        }
        m_cond.notify_all();
        for(auto& th : m_threads) {
            th.join();
        }
    }

    size_t GetWorkThreadsNum() const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_threads.size();
    }

    size_t GetTasksNum() const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_tasks.size();
    }


    template<typename F, typename... Args>
    auto Submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;

    template<typename F, typename... Args>
    auto SubmitTask(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;


    template<typename F, typename... Args,
            typename = typename std::enable_if<!std::is_void<typename std::result_of<F(Args...)>::type>::value, void>::type,
            typename Dummy = void>
    auto Execute(F&& f, Args&&... args) -> Future<typename std::result_of<F(Args...)>::type>;

    template<typename F, typename... Args,
            typename = typename std::enable_if<std::is_void<typename std::result_of<F(Args...)>::type>::value, void>::type>
    auto Execute(F&& f, Args&&... args) -> Future<void>;
};



template<typename F, typename... Args>
auto ThreadPool::Submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
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
/*
	std::future可以保存某一函数的结果，std::future通常把线程的结果放到一个future对象中，另一个线程中可以wait或get这个将来的结果
	std::packaged_task提供对某函数的封装，然后同步/异步运行该函数，函数结果通常保存在其内部的一个future对象中
	std::make_shared可以返回一个指定类型的std::shared_ptr
*/
template<typename F, typename... Args>
auto ThreadPool::SubmitTask(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(m_stop) {
            throw std::runtime_error("ThreadPool has already stop!");
        }
        m_tasks.emplace([task](){
            (*task)();
        });

    }
    m_cond.notify_one();
    return res;
}


template<typename F, typename... Args, typename, typename>
auto ThreadPool::Execute(F&& f, Args&&... args) -> Future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    Promise<return_type> promise;
    auto ft = promise.GetFuture();

    {
        std::unique_lock<std::mutex> lock(m_mutex);


        auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task = [f = std::move(func), pm = std::move(promise)]() mutable {
            try
            {
                pm.SetValue(Try<return_type>(f()));
            }
            catch(...)
            {
                pm.SetException(std::current_exception());
            }
        };

        m_tasks.emplace(std::move(task));
    }
    m_cond.notify_one();
    return ft;
}

template<typename F, typename... Args, typename>
auto ThreadPool::Execute(F&& f, Args&&... args) -> Future<void>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    static_assert(std::is_void<return_type>::value, "must be void");

    std::unique_lock<std::mutex> lock(m_mutex);
    
    Promise<return_type> promise;
    auto ft = promise.GetFuture();

    auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto task = [f = std::move(func), pm = std::move(promise)]() mutable {
        try
        {
            f();
            pm.SetValue();
        }
        catch(...)
        {
            pm.SetException(std::current_exception());
        }
    };

    m_tasks.emplace(std::move(task));
    m_cond.notify_one();
    return ft;
}


}

#endif