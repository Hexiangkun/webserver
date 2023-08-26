#ifndef EXECUTEONEXITSCOPE_H
#define EXECUTEONEXITSCOPE_H

#include <functional>

namespace hxk
{

class ExecuteOnExitScope
{
private:
    std::function<void()> m_func;
public:
    ExecuteOnExitScope(/* args */) = default;
    ~ExecuteOnExitScope()
    {
        if(m_func){
            m_func();
        }
    }

    ExecuteOnExitScope(const ExecuteOnExitScope&) = delete;
    void operator=(const ExecuteOnExitScope&) = delete;

    ExecuteOnExitScope(ExecuteOnExitScope&&) = default;
    ExecuteOnExitScope& operator=(ExecuteOnExitScope&&) = default;

    template<typename F, typename... Args>
    ExecuteOnExitScope(F&& f, Args&&... args)
    {
        m_func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    }
};

#define _CONCAT(a,b) a##b
#define _MAKE_DEFER_(line) hxk::ExecuteOnExitScope _CONCAT(defer, line) = [&]()

#undef HXK_DEFER
#define HXK_DEFER _MAKE_DEFER_(__LINE__)

}

#endif