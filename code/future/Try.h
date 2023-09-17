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
        None,       //未初始化
        Exception,  //异常
        Value,      //有返回值
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

    //隐式转换，隐式类型转换符
    operator const T& () const &    //将Try<T>对象转换成const T&    常量引用
    {
        return Value();
    }

    operator T& () &                //将Try<T>对象转换成T&          左值引用
    {
        return Value();
    }

    operator T&& () &&              //将Try<T>对象转换成T&&         右值引用
    {
        return std::move(Value());
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
    R Get() //将值转换为R类型
    {
        return std::forward<R>(Value());
    }
};

template<>
class Try<void>
{
    enum class State
    {
        Exception,  //异常
        Value,      //值状态
    };
private:
    State m_state;
    std::exception_ptr m_exception;
public:
    Try() : m_state(State::Value) {}
    explicit Try(std::exception_ptr e) :m_state(State::Exception), m_exception(std::move(e)) {}
    ~Try()
    {
        if(m_state == State::Exception) {
            m_exception.~exception_ptr();
        }
    }
    //move
    //移动构造函数
    Try(Try<void>&& t):m_state(t.m_state)
    {
        if(m_state == State::Exception) {
            new (&m_exception)std::exception_ptr(std::move(t.m_exception));
        }
    }

    //移动赋值运算符
    Try<void>& operator=(Try<void>&& t) 
    {
        if(this == &t) {
            return *this;
        }
        this->~Try();

        this->m_state = t.m_state;
        if(m_state == State::Exception) {
            new (&m_exception)std::exception_ptr(std::move(t.m_exception));
        }
        return *this;
    }

    //copy
    //拷贝构造
    Try(const Try<void>& t):m_state(t.m_state)
    {
        if(m_state == State::Exception) {
            new (&m_exception)std::exception_ptr(t.m_exception);
        }
    }
    //拷贝赋值运算符
    Try<void>& operator=(const Try<void>& t)
    {
        if(this == &t) {
            return *this;
        }

        this->~Try();
        // if(m_state == State::Exception) {
        //     m_exception.~exception_ptr();
        // }
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
//return type Try<F(Args..)>


/// @brief std::enable_if模板用于编译时根据条件决定是否启用该模板的实例化
/// @brief std::is_same模板用于检查F(Args...)的返回值是否为void   如果不是void，则启用该函数模板的实例化
/// @return 函数模板返回值是一个嵌套类型，即Try<F(Args...)>
template<typename F, typename... Args>
typename std::enable_if<
        !std::is_same<typename std::result_of<F(Args...)>::type, void>::value,
        typename TryWrapper<typename std::result_of<F(Args...)>::type>::Type>::type
    WrapWithTry(F&& f, Args&&... args)
{
    using Type = typename std::result_of<F(Args...)>::type;
    //调用f(args...)
    try
    {
        return typename TryWrapper<Type>::Type(std::forward<F>(f)(std::forward<Args>(args)...));
    }
    catch(std::exception& e)
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
    catch(std::exception& e)
    {
        return Try<void>(std::current_exception());
    }
    
}

//F() ,return Type
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
    catch(std::exception& e)
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
    catch(std::exception& e)
    {
        return Try<void>(std::current_exception());
    }
}

}


#endif