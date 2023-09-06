#ifndef _FUTURE_H
#define _FUTURE_H

#include <functional>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <memory>

#include "scheduler.h"
#include "try.h"
#include "helper.h"

namespace hxk
{

namespace internal
{
    enum class Progress
    {
        None,       //无进度
        Timeout,    //超时
        Done,       //完成
        Retrieved,  //已检索
    };

    using TimeoutCallback = std::function<void()>;

    template<typename T>
    struct State
    {
        //判断T是否为void，或者可以进行拷贝构造或移动构造
        static_assert(std::is_same<T, void>::value || std::is_copy_constructible<T>() || std::is_move_constructible<T>(), "must be copyable or movable or void");

        using ValueType = typename TryWrapper<T>::Type;     //Try<T>

        State() : m_progress(Progress::None), m_retrieved(false) {}
        bool IsRoot() const { return !m_onTimeout; }    //没有设置超时回调函数，视为根状态

        std::mutex m_then_lock;
        std::atomic<bool> m_retrieved;  //标记状态是否被获取
        ValueType m_value;      //Try<T>
        Progress m_progress;
        std::function<void(ValueType&&)> m_then;
        std::function<void(TimeoutCallback&&)> m_onTimeout;
    };
}

template<typename T>
class Future;

using namespace internal;

template<typename T>
class Promise
{
private:
    std::shared_ptr<State<T>> m_state;  //存储异步操作的状态

public:
    Promise():m_state(std::make_shared<State<T>>()) {}

    Promise(const Promise&) = default;
    Promise& operator=(const Promise&) = default;
    Promise(Promise&&) = default;
    Promise& operator=(Promise&& pm) = default;

    void SetException(std::exception_ptr exp)
    {
        std::unique_lock<std::mutex> lock(m_state->m_then_lock);
        bool isRoot = m_state->IsRoot();
        //为根状态，并且当前状态已经被设置
        if(isRoot && m_state->m_progress != Progress::None) {
            return;
        }
        m_state->m_progress = Progress::Done;
        m_state->m_value = typename State<T>::ValueType(std::move(exp));
        lock.unlock();

        //调用then函数,m_state->m_value由于重载了()，可以直接访问到T类型的值
        if(m_state->m_then) {   
            m_state->m_then(std::move(m_state->m_value));
        }
    }

    //如果T(SHIT)不是void，返回值为void
    template<typename SHIT = T>
    typename std::enable_if<!std::is_void<SHIT>::value, void>::type SetValue(SHIT&& t)
    {
        std::unique_lock<std::mutex> guard(m_state->m_then_lock);
        bool isRoot = m_state->IsRoot();
        if(isRoot && m_state->m_progress != Progress::None) {
            return;
        }

        m_state->m_progress = Progress::Done;
        m_state->m_value = std::forward<SHIT>(t);

        guard.unlock();

        if(m_state->m_then) {
            m_state->m_then(std::move(m_state->m_value));
        }
    }

    template<typename SHIT = T>
    typename std::enable_if<std::is_void<SHIT>::value, void>::type SetValue()
    {
        std::unique_lock<std::mutex> guard(m_state->m_then_lock);
        if(m_state->m_progress != Progress::None) {
            return;
        }

        m_state->m_progress = Progress::Done;
        m_state->m_value = Try<void>();

        guard.lock();
        if(m_state->m_then) {
            m_state->m_then(std::move(m_state->m_value));
        }
    }

    Future<T> GetFuture()
    {
        bool expect = false;
        if(!m_state->m_retrieved.compare_exchange_strong(expect, true)) {
            throw std::runtime_error("Future already retrieved");
        }
        return Future<T>(m_state);
    }

    bool IsReady() const        //检查异步操作是否完成
    {
        return m_state->m_progress != Progress::None;
    }

};

template<typename T2>
Future<T2> MakeExceptionFuture(std::exception_ptr&&);


template<typename T>
class Future
{
private:
    std::shared_ptr<State<T>> m_state;

public:
    using InnerType = T;

    template<typename U>
    friend class Future;

    Future() = default;
    explicit Future(std::shared_ptr<State<T>> state) : m_state(std::move(state)) {}

    Future(const Future&) = delete;
    void operator=(const Future&) = delete;

    Future(Future&&) = default;
    Future& operator=(Future&&) = default;

    bool valid() const
    {
        return m_state != nullptr;
    }
    
    /// @brief 用于等待异步操作完成并获取其结果
    /// @param timeout 
    /// @return Try<T>
    typename State<T>::ValueType Wait(const std::chrono::milliseconds& timeout = std::chrono::milliseconds(24*3600*1000))
    {
        std::unique_lock<std::mutex> guard(m_state->m_then_lock);
        switch (m_state->m_progress)
        {
        case Progress::None:        //异步操作尚未完成，执行后续代码
            break;      
        case Progress::Timeout:     //操作超时，抛出异常
            throw std::runtime_error("Future timeout");

        case Progress::Done:        //操作完成，将状态设置为Retrieved，表示已获取，返回异步结果
            m_state->m_progress = Progress::Retrieved;
            return std::move(m_state->m_value);
        default:                    //抛出异常，表示结果已被获取
            throw std::runtime_error("Future already retrieved");

        }
        guard.unlock();

        auto cond(std::make_shared<std::condition_variable>());
        auto mutex(std::make_shared<std::mutex>());
        bool ready = false;     //表示结果是否准备好

        typename State<T>::ValueType value; //用于存储异步操作的结果

        //调用then函数注册一个回调函数，该回调函数会在异步操作的结果可用时被调用
        this->Then([&value, &ready, 
                wcond = std::weak_ptr<std::condition_variable>(cond),
                wmutex = std::weak_ptr<std::mutex>(mutex)](typename State<T>::ValueType&& v)
                {
                    auto cond = wcond.lock();
                    auto mutex = wmutex.lock();

                    if(!cond || !mutex) {
                        return;
                    }
                    std::unique_lock<std::mutex> guard(*mutex);
                    value = std::move(v);
                    ready = true;
                    cond->notify_one();
                });
        std::unique_lock<std::mutex> waiter(*mutex);

        //等待ready变为true或者timeout
        bool success = cond->wait_for(waiter, timeout, [&ready](){
            return ready;
        });
        if(success) {
            return std::move(value);
        }
        else {
            throw std::runtime_error("Future wait_for timeout");
        }
    }

    /// @brief when T(SHIT) is Future<InnerType> can be valid
    /// @tparam SHIT = Future<I>
    /// @return Future<I>
    template<typename SHIT = T>
    typename std::enable_if<IsFuture<SHIT>::value, SHIT>::type Unwrap()
    {
        using InnerType = typename IsFuture<SHIT>::Inner;   //获取future内部对象类型，Future<I>即I

        static_assert(std::is_same<SHIT, Future<InnerType>>::value, "SHIT is not Future<T> type");

        Promise<InnerType> prom;
        Future<InnerType> fut = prom.GetFuture();

        std::unique_lock<std::mutex> guard(m_state->m_then_lock);

        if(m_state->m_progress == Progress::Timeout) {
            throw std::runtime_error("Wrong State : Timeout");
        }
        else if(m_state->m_progress == Progress::Done) {
            try
            {
                auto innerFuture = std::move(m_state->m_value);     //Try<Future<I>>
                return std::move(innerFuture.Value());  //Future<I>
            }
            catch(const std::exception& e)
            {
                return MakeExceptionFuture<InnerType>(std::current_exception());
            }
        }
        else {                                          //Try<Future<I>>
            _SetCallback([pm = std::move(prom)](typename TryWrapper<SHIT>::Type&& innerFuture) mutable {
                try
                {   //Future<I>  隐式转换
                    SHIT future = std::move(innerFuture);               //Try<I>
                    future._SetCallback([pm = std::move(pm)](typename TryWrapper<InnerType>::Type&& t) mutable {
                        pm.SetValue(std::move(t));
                    });
                }
                catch(...)
                {
                    pm.SetException(std::current_exception());
                }
            });
        }
        return fut;
    }


    //返回值类型Future<F(T)>
    template<typename F, typename R = CallableResult<F,T>>
    auto Then(F&& f) -> typename R::ReturnFutureType
    {
        typedef typename R::Arg Arguments;  //ResultOfWrapper<F()|F*(T&)|F(T&&)>
        return _ThenImpl<F, R>(nullptr, std::forward<F>(f), Arguments());
    }

    template<typename F, typename R = CallableResult<F,T>>
    auto Then(Scheduler* sched, F&& f) -> typename R::ReturnFutureType
    {
        typedef typename R::Arg Arguments;
        return _ThenImpl<F,R>(sched, std::forward<F>(f), Arguments());
    }

    /// @brief 
    /// @tparam F 回调函数的类型
    /// @tparam R R=CallableResult<F,T> 返回类型封装类
    /// @tparam ...Args 
    /// @param sched 
    /// @param f 
    /// @param  如果返回类型不是Future类型，推导出Future类型
    /// @return 
    template<typename F, typename R, typename... Args>
    typename std::enable_if<!R::IsReturnsFuture::value, typename R::ReturnFutureType>::type _ThenImpl(Scheduler* sched, F&& f, ResultOfWrapper<F, Args...>)
    {
        static_assert(sizeof...(Args) <= 1, "Then must take zero/one arguments");

        using FReturnType = typename R::IsReturnsFuture::Inner; //F()|F(T&)|F(T&&)返回值类型

        Promise<FReturnType> pm;
        auto nextFuture = pm.GetFuture();

        using FuncType = typename std::decay<F>::type;

        std::unique_lock<std::mutex> guard(m_state->m_then_lock);
        if(m_state->m_progress == Progress::Timeout) {          //超时
            throw std::runtime_error("Wrong state: Timeout");
        }
        else if(m_state->m_progress == Progress::Done) {        //已经完成
            typename TryWrapper<T>::Type t;
            try
            {
                t = std::move(m_state->m_value);
            }
            catch(const std::exception& e)
            {
                t = (typename TryWrapper<T>::Type)(std::current_exception());
            }
            guard.unlock();

            if(sched) { //异步执行
                sched->Schedule([t = std::move(t), f = std::forward<FuncType>(f), 
                                    pm = std::move(pm)]() mutable {
                    auto result = WrapWithTry(f, std::move(t)); //f(t)--->F(Try<T>)--->F(T&)|F(T&&)
                    pm.SetValue(std::move(result));
                });
            }
            else {  //直接执行
                auto result = WrapWithTry(f, std::move(t));
                pm.SetValue(std::move(result));
            }
        }
        else {  //设置pm超时回调函数，也就是设置nextfuture的超时回调函数
            nextFuture._SetOnTimeout([weak_parent = std::weak_ptr<State<T>>(m_state)](TimeoutCallback&& cb) {
                auto parent = weak_parent.lock();
                if(!parent) {
                    return;
                }

                {
                    std::unique_lock<std::mutex> guard(parent->m_then_lock);
                    if(parent->m_progress != Progress::None) {  //已经被设置状态了，说明没有超时，直接返回
                        return;
                    }
                    parent->m_progress = Progress::Timeout;
                }

                if(!parent->IsRoot()) { //m_onTimeout不为nullptr
                    parent->m_onTimeout(std::move(cb));
                }
                else {  //是根节点，直接调用超时回调函数
                    cb();
                }
            });
            _SetCallback([sched, func = std::forward<FuncType>(f), prom = std::move(pm)]
                            (typename TryWrapper<T>::Type&& t) mutable {
                if(sched) {
                    sched->Schedule([func = std::move(func), t = std::move(t), prom = std::move(prom)]() mutable {
                        auto result = WrapWithTry(func, std::move(t));
                        prom.SetValue(std::move(result));
                    });
                }
                else {
                    auto result = WrapWithTry(func, std::move(t));
                    prom.SetValue(std::move(result));
                }
            });
        }

        return std::move(nextFuture);
    }

    /// @brief 
    /// @tparam F 
    /// @tparam R R=CallableResult<F,T> Future<I>
    /// @tparam ...Args 
    /// @param sched 
    /// @param f 
    /// @param  
    /// @return Future<I>
    template<typename F, typename R, typename... Args>
    typename std::enable_if<R::IsReturnsFuture::value, typename R::ReturnFutureType>::type _ThenImpl(Scheduler* sched, F&& f, ResultOfWrapper<F, Args...>)
    {
        static_assert(sizeof...(Args) <= 1, "Then must take one/zero arguments");

        using FReturnType = typename R::IsReturnsFuture::Inner; //I
        
        Promise<FReturnType> pm;
        auto nextFuture = pm.GetFuture();

        using FuncType = typename std::decay<F>::type;

        std::unique_lock<std::mutex> guard(m_state->m_then_lock);
        if(m_state->m_progress == Progress::Timeout) {
            throw std::runtime_error("Wrong state: Timeout");
        }
        else if(m_state->m_progress == Progress::Done) {
            typename TryWrapper<T>::Type t;
            try
            {
                t = std::move(m_state->m_value);
            }
            catch(const std::exception& e)
            {
                t = decltype(t)(std::current_exception());
            }
            guard.unlock();

            auto cb = [res = std::move(t), f = std::forward<FuncType>(f),
                                         prom = std::move(pm)]() mutable {
                //从res中获取Args类型的参数，使用decltype关键字推导f()的返回值类型
                decltype(f(res.template Get<Args>()...)) innerFuture;
                if(res.HasException()) {
                    innerFuture = f(typename TryWrapper<typename std::decay<Args...>::type>::Type(res.Exception()));
                }
                else {
                    innerFuture = f(res.template Get<Args>()...);
                }

                if(!innerFuture.valid()) {
                    return;
                }

                std::unique_lock<std::mutex> guard(innerFuture.m_state->m_then_lock);
                if(innerFuture.m_state->m_progress == Progress::Timeout) {
                    throw std::runtime_error("Wrong State : timeout");
                }
                else if(innerFuture.m_state->m_progress == Progress::Done) {
                    typename TryWrapper<FReturnType>::Type t;
                    try
                    {
                        t = std::move(innerFuture.m_state->m_value);
                    }
                    catch(const std::exception& e)
                    {
                        t = decltype(t)(std::current_exception());
                    }
                    guard.unlock();
                    prom.SetValue(std::move(t));
                }
                else {
                    innerFuture._SetCallback([prom = std::move(prom)](typename TryWrapper<FReturnType>::Type&& t) mutable {
                        prom.SetValue(std::move(t));
                    });
                }
            };
            if(sched) {
                sched->Schedule(std::move(cb));
            }
            else {
                cb();
            }
        }
        else {
            nextFuture._SetOnTimeout([weak_parent = std::weak_ptr<State<T>>(m_state)](TimeoutCallback&& cb) {
                auto parent = weak_parent.lock();
                if(!parent) {
                    return;
                }
                {
                    std::unique_lock<std::mutex> guard(parent->m_then_lock);
                    if(parent->m_progress != Progress::None) {
                        return;
                    }
                    parent->m_progress = Progress::Timeout;
                }

                if(!parent->IsRoot()) {
                    parent->m_onTimeout(std::move(cb));
                }
                else {
                    cb();
                }
            });

            _SetCallback([sched = sched, func = std::forward<FuncType>(f), prom = std::move(pm)](typename TryWrapper<T>::Type&& t) mutable {
                auto cb = [func = std::move(func), t = std::move(t), prom = std::move(prom)]() mutable {
                    decltype(func(t.template Get<Args>()...)) innerFuture;
                    if(t.HasException()) {
                        innerFuture = func(typename TryWrapper<typename std::decay<Args...>::type>::Type(t.Exception()));
                    }
                    else {
                        innerFuture = func(t.template Get<Args...>());
                    }

                    if(!innerFuture.valid()) {
                        return;
                    }
                    std::unique_lock<std::mutex> guard(innerFuture.m_state->m_then_lock);
                    if(innerFuture.m_state->m_progress == Progress::Timeout) {
                        throw std::runtime_error("Wrong state: Timeout");
                    }
                    else if(innerFuture.m_state->m_progress == Progress::Done) {
                        typename TryWrapper<FReturnType>::Type t;
                        try
                        {
                            t = std::move(innerFuture.m_state->m_value);
                        }
                        catch(const std::exception& e)
                        {
                            t = decltype(t)(std::current_exception());
                        }
                        guard.unlock();
                        prom.SetValue(std::move(t));
                    }
                    else {
                        innerFuture._SetCallback([prom = std::move(prom)](typename TryWrapper<FReturnType>::Type&& t) mutable {
                            prom.SetValue(std::move(t));
                        });
                    }
                };
                if(sched) {
                    sched->Schedule(std::move(cb));
                }
                else {
                    cb();
                }
            });
        }
        return std::move(nextFuture);
    }

    void OnTimeout(std::chrono::milliseconds duration, TimeoutCallback f, Scheduler* scheduler)
    {
        scheduler->ScheduleLater(duration, [state = m_state, cb = std::move(f)]() mutable {
            if(state->m_progress != Progress::None) {
                return;
            }
            state->m_progress = Progress::Timeout;

            if(!state->IsRoot()) {
                state->m_onTimeout(std::move(cb));
            }
            else {
                cb();
            }
        });
    }

private:
    void _SetCallback(std::function<void(typename TryWrapper<T>::Type&&)>&& func) 
    {
        m_state->m_then = std::move(func);
    }

    void _SetOnTimeout(std::function<void(TimeoutCallback&&)>&& func)
    {
        m_state->m_onTimeout = std::move(func);
    }
};


template<typename T2>
inline Future<typename std::decay<T2>::type> MakeReadyFuture(T2&& value)
{
    Promise<typename std::decay<T2>::type> pm;
    auto f(pm.GetFuture());
    pm.SetValue(std::forward<T2>(value));

    return f;
}

inline Future<void> MakeReadyFuture()
{
    Promise<void> pm;
    auto f(pm.GetFuture());
    pm.SetValue();
    return f;
}


template<typename T2, typename E>
inline Future<T2> MakeExceptionFuture(E&& exp)
{
    Promise<T2> pm;
    pm.SetException(std::make_exception_ptr(std::forward<E>(exp)));

    return pm.GetFuture();
}

template<typename T2>
inline Future<T2> MakeExceptionFuture(std::exception_ptr&& ptr)
{
    Promise<T2> pm;
    pm.SetException(std::move(ptr));

    return pm.GetFuture();
}


template<typename... FT>
typename CollectAllVariadicContext<typename std::decay<FT>::type::InnerType...>::FutureType WhenAll(FT&&... futures)
{
    auto ctx = std::make_shared<CollectAllVariadicContext<typename std::decay<FT>::type::InnerType...>>();

    CollectVariadicHelper<CollectAllVariadicContext>(ctx, std::forward<typename std::decay<FT>::type>(futures)...);

    return ctx->pm.GetFuture();
}

template<class InputIterator>
Future<std::vector<typename TryWrapper<typename std::iterator_traits<InputIterator>::value_type::InnerType>::Type>> WhenAll(InputIterator first, InputIterator last)
{
    using TryT = typename TryWrapper<typename std::iterator_traits<InputIterator>::value_type::InnerType>::Type;
    if(first == last) {
        return MakeReadyFuture(std::vector<TryT>());
    }

    struct AllContext
    {
        AllContext(int n) : results(n) {}
        ~AllContext(){}

        Promise<std::vector<TryT>> pm;
        std::vector<TryT> results;
        std::atomic<size_t> collected{0};
    };

    auto ctx = std::make_shared<AllContext>(std::distance(first, last));

    for(size_t i=0; first != last; ++first, ++i) {
        first->Then([ctx, i](TryT&& t) {
            if(ctx->results.size() - 1 == std::atomic_fetch_add(&ctx->collected, std::size_t(1))) {
                ctx->pm.SetValue(std::move(ctx->results));
            }
        });
    }

    return ctx->pm.GetFuture();
}
}

#endif