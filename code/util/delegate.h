#ifndef DELEGATE_HPP
#define DELEGATE_HPP

#include <functional>
#include <list>

namespace hxk
{

template<typename T>
class Delegate;

template<typename... Args>
class Delegate<void(Args...)>
{
public:
    typedef Delegate<void(Args...)> Self;

    Delegate() = default;
    Delegate(const Self&) = delete;
    Self& operator=(const Self&) = delete;

    template<typename F>
    Delegate(F&& f)
    {
        addFunc(std::forward<F>(f));
    }

    Delegate(Self&& other)
    {
        m_functions(std::move(other.m_functions));
    }

    template<typename F>
    Self& operator+=(F&& f)
    {
        addFunc(std::forward<F>(f));
        return *this;
        
    }

    template<typename... ARGS>
    void operator()(ARGS&&... args)
    {
        call(std::forward<ARGS>(args)...);
    }

private:
    template<typename F>
    void addFunc(F&& f)
    {
        m_functions.emplace_back(std::forward<F>(f));
    }

    template<typename... ARGS>
    void call(ARGS&&... args)
    {
        for(const auto& func : m_functions) {
            func(std::forward<ARGS>(args)...);
        }
    }


private:
    std::list<std::function<void(Args...)>> m_functions;
};


}


#endif