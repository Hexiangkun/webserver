#ifndef COUROUTINE_H
#define COUROUTINE_H

#include <ucontext.h>
#include <memory>
#include <functional>


namespace hxk
{

using AnyPointer = std::shared_ptr<void>;

class Coroutine
{
public:
    using _ptr = std::shared_ptr<Coroutine>;

    enum State {
        INIT,
        RUNNING,
        FINISH
    };

public:


public:
    explicit Coroutine(size_t stack_size = 0);

    template<typename F, typename... Args,
            typename = typename std::enable_if<std::is_void<typename std::result_of<F(Args...)>::type>::value, void>::type, typename Dummy = void>
    Coroutine(F&& f, Args&&... args) : Coroutine(kDefaultStackSize)
    {
        using resultType = typename std::result_of<F(Args...)>::type;

        auto me =this;
        auto temp = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        m_func = [temp, me]() mutable {
            me->m_result = std::make_shared<resultType>(temp());0.
        }
    }
    ~Coroutine();

private:
    std::function<void()> m_func;

    AnyPointer m_result;
    static const size_t kDefaultStackSize = 8*1024;
};


}

#endif