#ifndef TRY_H
#define TRY_H

#include <exception>
#include <stdexcept>

namespace hxk
{
template<typename T>
class Try
{
    enum class State
    {
        None,
        Exception,
        Value,
    };
private:
    State m_state;
    union   //默认public，当多个数据需要共享内存或者多个数据只取其一
    {
        T m_value;
        std::exception_ptr m_exception;
    };
    
public:
    Try(): m_state(State::None) {}
    ~Try()
    {
        if(m_state == State::Exception) {
            m_exception.~exception_ptr();
        }
        else if(m_state == State::Value) {
            m_value.~T();
        }
    }

    Try(const T& t) : m_state(State::Value), m_value(t) {}
    Try(T&& t) : m_state(State::Value), m_value(std::move(t)) {}
    Try(std::exception_ptr e) : m_state(State::Exception), m_exception(std::move(e)) {}

    //move
    Try(Try<T>&& t) : m_state(t.m_state)
    {
        if(m_state == State::Value) {
            new (&m_value)T(std::move(t.m_value));
        }
        else if(m_state == State::Exception) {
            new (&m_exception)std::exception_ptr(std::move(t.m_exception));
        }
    }

    Try<T>& operator=(Try<T>&& t)
    {
        if(this == &t) {
            return *this;
        }

        this->~Try();

        m_state = t.m_state;
        if(m_state == State::Value) {
            new (&m_value)T(std::move(t.m_value));
        }
        else if(m_state == State::Exception) {
            new (&m_exception)std::exception_ptr(std::move(t.m_exception));
        }
        return *this;
    }

    //copy
    Try(const Try<T>& t) : m_state(t.m_state)
    {
        if(m_state == State::Value) {
            new (&m_value)T(t.m_value);
        }
        else if(m_state == State::Exception) {
            new (&m_exception)std::exception_ptr(t.m_exception);
        }
    }

    Try<T>& operator=(const Try<T>& t)
    {
        if(this == &t) {
            return *this;
        }

        this->~Try();
        m_state = t.m_state;
        if(m_state == State::Value) {
            new (&m_value)T(t.m_value);
        }
        else if(m_state == State::Exception) {
            new (&m_exception)std::exception_ptr(t.m_exception);
        }
        return *this;
    }

    //隐式转换
    operator const T& () const &    //隐式类型转换符
    {
        return Value();
    }

    operator T& () & 
    {
        return Value();
    }

    operator T&& () && 
    {
        return Value();
    }

    //get value
    const T& Value() const&
    {
        Check();
        return m_value;
    }

    T& Value() &
    {
        Check();
        return m_value;
    }

    T&& Value() &&
    {
        Check();
        return std::move(m_value);
    }

    // get exception
    const std::exception_ptr& Exception() const &
    {
        if(!HasException()) {
            throw std::runtime_error("Not Exception State");
        }
        return m_exception;
    }

    std::exception_ptr& Exception() &
    {
        if(!HasException()) {
            throw std::runtime_error("Not Exception State");
        }
        return m_exception;
    }

    std::exception_ptr&& Exception() &&
    {
        if(!HasException()) {
            throw std::runtime_error("Not Exception State");
        }
        return std::move(m_exception);
    }

    bool HasValue() const
    {
        return m_state == State::Value;
    }

    bool HasException() const
    {
        return m_state == State::Exception;
    }

    const T& operator*() const
    {
        return Value();
    }

    T& operator*()
    {
        return Value();
    }

    struct UninitializedTry {};

    void Check() const
    {
        if(m_state == State::Exception) {
            std::rethrow_exception(m_exception);
        }
        else if(m_state == State::None) {
            throw UninitializedTry();
        }
    }

    template<typename R>
    R Get()
    {
        return std::forward<R>(Value());
    }
};

template<>
class Try<void>
{
    enum class State
    {
        Exception,
        Value,
    };
private:
    State m_state;
    std::exception_ptr m_exception;
public:
    Try() : m_state(State::Exception) {}
    explicit Try(std::exception_ptr e) :m_state(State::Exception), m_exception(std::move(e)) {}
    ~Try()
    {
        if(m_state == State::Exception) {
            m_exception.~exception_ptr();
        }
    }
    //move
    Try(Try<void>&& t):m_state(t.m_state)
    {
        if(m_state == State::Exception) {
            new (&m_exception)std::exception_ptr(std::move(t.m_exception));
        }
    }

    Try<void>& operator=(Try<void>&& t) 
    {
        if(this == &t) {
            return *this;
        }
        this->m_state = t.m_state;
        if(m_state == State::Exception) {
            new (&m_exception)std::exception_ptr(std::move(t.m_exception));
        }
    }

    //copy
    Try(const Try<void>& t):m_state(t.m_state)
    {
        if(m_state == State::Exception) {
            new (&m_exception)std::exception_ptr(t.m_exception);
        }
    }

    Try<void>& operator=(const Try<void>& t)
    {
        if(this == &t) {
            return *this;
        }

        // this->~Try();
        if(m_state == State::Exception) {
            m_exception.~exception_ptr();
        }
        m_state = t.m_state;
        if(m_state == State::Exception) {
            new (&m_exception)std::exception_ptr(t.m_exception);
        }
        return *this;
    }

    //get exception
    const std::exception_ptr& Exception() const &
    {
        if(!HasException()) {
            throw std::runtime_error("Not Exception state");
        }
        return m_exception;
    }

    std::exception_ptr& Exception() &
    {
        if(!HasException()) {
            throw std::runtime_error("Not Exception state");
        }
        return m_exception;        
    }

    std::exception_ptr&& Exception() &&
    {
        if(!HasException()) {
            throw std::runtime_error("Not Exception state");
        }
        return std::move(m_exception);        
    }

    bool HasValue() const
    {
        return m_state == State::Value;
    }

    bool HasException() const
    {
        return m_state == State::Exception;
    }

    void Check() const
    {
        if(m_state == State::Exception) {
            std::rethrow_exception(m_exception);
        }
    }

    template<typename R>
    R Get()
    {
        return std::forward<R>(*this);
    }

};

template<typename T>
struct TryWrapper
{
    using Type = Try<T>;
};

template<typename T>
struct TryWrapper<Try<T>>
{   
    using Type = Try<T>;
};

//if F(Args..) return value's type isn't void ,use TryWrapper to pack T
template<typename F, typename... Args>
typename std::enable_if<
        !std::is_same<typename std::result_of<F(Args...)>::type, void>::value,
        typename TryWrapper<typename std::result_of<F(Args...)>::type>::Type>::type
    WrapWithTry(F&& f, Args&&... args)
{
    using Type = typename std::result_of<F(Args...)>::type;

    try
    {
        return typename TryWrapper<Type>::Type(std::forward<F>(f)(std::forward<Args>(args)...));
    }
    catch(const std::exception& e)
    {
        return typename TryWrapper<Type>::Type(std::current_exception());
    }
}

//F(Args...) return value type is void
template<typename F, typename... Args>
typename std::enable_if<std::is_same<typename std::result_of<F(Args...)>::type, void>::value, Try<void>>::type
    WrapWithTry(F&& f, Args&&... args)
{
    try
    {
        std::forward<F>(f)(std::forward<Args>(args)...);    //函数执行
        return Try<void>();
    }
    catch(const std::exception& e)
    {
        return Try<void>(std::current_exception());
    }
    
}

//F(void) ,return Type
template<typename F>
typename std::enable_if<
        !std::is_same<typename std::result_of<F()>::type, void>::value,
        typename TryWrapper<typename std::result_of<F()>::type>::Type>::type
    WrapWithTry(F&& f, Try<void>&& args)
{
    using Type = typename std::result_of<F()>::type;

    try
    {
        return typename TryWrapper<Type>::Type(std::forward<F>(f)());
    }
    catch(const std::exception& e)
    {
        return typename TryWrapper<Type>::Type(std::current_exception());
    }
}

//F(void) , return void
template<typename F>
typename std::enable_if<std::is_same<typename std::result_of<F()>::type, void>::value, Try<typename std::result_of<F()>::type>>::type
    WrapWithTry(F&& f, Try<void>&& args)
{
    try
    {
        std::forward<F>(f)();
        return Try<void>();
    }
    catch(const std::exception& e)
    {
        return Try<void>(std::current_exception());
    }
}

}


#endif