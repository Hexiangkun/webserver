#ifndef DELEGATE_H
#define DELEGATE_H

#include <list>
#include <functional>
#include <vector>
#include <algorithm>

namespace hxk
{

template<typename ReturnType, typename... Args>
class SimpleDelegateBase
{
protected:
    // typedef ReturnType (*FunctionType)(Args...);
    using FunctionType = std::function<ReturnType(Args...)>;

    std::list<FunctionType> m_functions;

    SimpleDelegateBase() = default;
};

template<typename T>
class SimpleDelegate;

template<typename... Args>
class SimpleDelegate<void(Args...)> : public SimpleDelegateBase<void, Args...>
{
public:
    using Parent = SimpleDelegateBase<void, Args...>;
    using Parent::m_functions;
    using typename Parent::FunctionType;

    void operator()(Args... args)
    {
        for(auto&& m_func : m_functions)
        {
            m_func(std::forward<Args>(args)...);
        }
    }

    SimpleDelegate& operator+=(const FunctionType& rhs)
    {
        this->m_functions.push_back(rhs);
        return *this;
    }

    SimpleDelegate& operator-=(const FunctionType& rhs)
    {
        m_functions.erase(std::remove(m_functions.begin(),m_functions.end(), rhs), m_functions.end());
        return *this;
    }
};


template<typename ReturnType, typename... Params>
class DelegateBase: public SimpleDelegateBase<ReturnType, Params...>
{
    template<typename... Args>
    struct FunctionParams
    {
        size_t index = -1;
        std::tuple<Args...> parameters;
    };

protected:
    using Parent = SimpleDelegateBase<ReturnType, Params...>;
    using Parent::m_functions;
    using typename Parent::FunctionType;

    std::vector<FunctionParams<Params...>> m_parameters;

public:

    const std::vector<FunctionType>& getFunctions() const
    {
        return this->m_functions;
    }

    void combine(const DelegateBase& other)
    {
        for(size_t i = 0; i < other.m_functions.size(); i++) {
            this->m_functions.push_back(other.m_functions[i]);
            attachParameters(other.m_parameters[i].parameters, std::index_sequence_for<Params...>());
        }
    }

    void subscribe(const FunctionType& func, Params... params)
    {
        this->m_functions.push_back(func);
        attachParameters(std::tuple<Params...>(params...), std::index_sequence_for<Params...>());
    }

    void subscribe(const std::initializer_list<FunctionType>& funcs, Params... params)
    {
        for(auto&& func : funcs) {
            this->m_functions.push_back(func);
            attachParameters(std::tuple<Params...>(params...), std::index_sequence_for<Params...>());
        }
    }

    

private:
    void attachParameters(Params... params)
    {
        this->m_parameters.push_back(FunctionParams<Params...>{m_functions.size()-1, params...});
    }

    template<size_t... Indices>
    void attachParameters(const std::tuple<Params...>& tuple, std::index_sequence<Indices...>)
    {
        this->m_parameters.push_back(FunctionParams<Params...>{m_functions.size()-1, tuple});
    }
};

template<typename T>
class Delegate;

template<typename... Params>
class Delegate<void(Params...)> : public DelegateBase<void, Params...>
{
public:
    using Parent = DelegateBase<void, Params...>;
    
    using typename Parent::FunctionType;


};

}


#endif