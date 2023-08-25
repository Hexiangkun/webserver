#ifndef DELEGATE_H
#define DELEGATE_H

#include <list>
#include <functional>
#include <vector>
#include <algorithm>
#include <tuple>

namespace hxk
{

template<typename ReturnType, typename... Args>
class SimpleDelegateBase
{
protected:
    typedef ReturnType (*FunctionType)(Args...);
    //using FunctionType = std::function<ReturnType(Args...)>;

    std::vector<FunctionType> m_functions;

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
        m_functions.erase(std::remove(m_functions.begin(),m_functions.end(), (rhs)), m_functions.end());
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

    void subscribe(const FunctionType& func, std::vector<std::tuple<Params...>> params)
    {
        for(size_t i = 0; i < params.size(); i++) {
            this->m_functions.push_back(func);
            attachParameters(params[i], std::index_sequence_for<Params...>());
        }
    }

    void invoke()
    {
        for(size_t i = 0; i < m_parameters.size(); i++) {
            callInvoke(m_parameters[i].parameters, m_parameters[i].index, std::index_sequence_for<Params...>());
        }
        return;
    }

    void remove(int count = 1, bool fromBack = true)
    {
        int adjustCount = count < m_functions.size() ? count : m_functions.size() - 1;

        if(!fromBack) {
            for(size_t i = 0; i< adjustCount; ++i) {
                detachParameters(0);
            }
            m_functions.erase(m_functions.begin(), m_functions.begin() + count);
            return;
        }
        for(size_t i = 0; i < adjustCount; ++i) {
            detachParameters(m_parameters.size() - 1);
            m_functions.pop_back();
        }
    }

    void remove(const FunctionType& func)
    {
        m_functions.erase(std::remove_if(m_functions.begin(), m_functions.end(),
            [func](const FunctionType& f) { return &f == &func; }), m_functions.end());
    }

    void remove(const std::vector<FunctionType>& funcs)
    {
        for(auto& f : funcs) {
            this->m_functions.erase(std::remove_if(m_functions.begin(), m_functions.end(),
                [f](const FunctionType& func){ return &f == &func; }), m_functions.end());
        }
    }

    void clear()
    {
        this->m_functions.clear();
        this->m_parameters.clear();
    }

    DelegateBase& operator+=(const FunctionType& rhs)
    {
        this->m_functions.push_back(rhs);
        return *this;
    }

    DelegateBase& operator+=(const std::initializer_list<FunctionType>& rhs)
    {
        for(auto x : rhs)
        {
            this->m_functions.push_back(x);
        }
        return *this;
    }

    DelegateBase& operator-=(const FunctionType& rhs)
    {
        this->m_functions.erase(std::remove(m_functions.begin(), m_functions.end(), rhs), m_functions.end());
        return *this;
    }

    DelegateBase& operator-=(const std::initializer_list<FunctionType>& rhs)
    {
        auto toRemove = [&](const FunctionType &func) -> bool {
            return std::find(rhs.begin(), rhs.end(), func) != rhs.end();
        };
        m_functions.erase(std::remove_if(m_functions.begin(), m_functions.end(), toRemove), m_functions.end());
        return *this;
    }

    DelegateBase& operator++()
    {
        if(m_functions.empty()){
            return *this;
        }

        FunctionType newFunc = m_functions.front();

        if(m_parameters.empty()) {
            m_functions.insert(m_functions.begin(), newFunc);
            return *this;
        }

        m_functions.insert(m_functions.begin(), newFunc);
        attachParameters(m_parameters.front().parameters, std::index_sequence_for<Params...>());

        return *this;
    }

    DelegateBase& operator++(int)
    {
        if(m_functions.empty()){
            return *this;
        }

        FunctionType newFunc = m_functions.back();

        if(m_parameters.empty()) {
            m_functions.push_back(newFunc);
            return *this;
        }

        m_functions.push_back(newFunc);
        attachParameters(m_parameters.back().parameters, std::index_sequence_for<Params...>());
        return *this;
    }

    DelegateBase& operator--()
    {
        if(m_functions.empty()){
            return *this;
        }
        if(m_parameters.empty()) {
            m_functions.pop_back();
            return *this;
        }

        int index = m_parameters.back().index;
        m_parameters.pop_back();
        m_functions.erase(m_functions.begin() + index);
        return *this;
    }

    DelegateBase& operator--(int)
    {
        if(m_functions.empty()) {
            return *this;
        }
        if(m_parameters.empty()) {
            m_functions.pop_back();
            return *this;
        }

        int index = m_parameters.back().index;
        m_parameters.pop_back();
        m_functions.erase(m_functions.begin() + index);

        return *this;
    }

    DelegateBase& operator<<(DelegateBase& rhs)
    {
        if(this != &rhs) {
            this->combine(rhs);
            rhs.clear();
        }
        return *this;
    }

    DelegateBase& operator>>(DelegateBase& rhs)
    {
        if(this != &rhs) {
            rhs.combine(*this);
            this->clear();
        }
        return *this;
    }

    bool operator<(const DelegateBase& rhs)
    {
        return m_functions.size() < rhs.m_functions.size();
    }

    bool operator<=(const DelegateBase& rhs)
    {
        return m_functions.size() <= rhs.m_functions.size();
    }

    bool operator>(const DelegateBase& rhs)
    {
        return m_functions.size() > rhs.m_functions.size();
    }

    bool operator>=(const DelegateBase& rhs)
    {
        return m_functions.size() >= rhs.m_functions.size();
    }

    bool operator==(const DelegateBase& rhs)
    {
        return m_functions == rhs.m_functions;
    }

    bool operator!=(const DelegateBase& rhs)
    {
        return m_functions != rhs.m_functions;
    }


protected:
    template<size_t... Indices>
    ReturnType callInvoke(const std::tuple<Params...>& tuple, int index, std::index_sequence<Indices...>)
    {
        return m_functions[index](std::get<Indices>(tuple)...);
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

    void detachParameters(const size_t index)
    {
        int erasedCount = 0;

        m_parameters.erase(std::remove_if(m_parameters.begin(), m_parameters.end(),
            [index, &erasedCount](const FunctionParams<Params...>& param){
                if(param.index == index){
                    erasedCount++;
                    return true;
                }
                return false;
            }), m_parameters.end());
        
        if(index > 0) {
            return ;
        }

        for(size_t i = 0; i < m_parameters.size(); i++) {
            m_parameters[i].index -= erasedCount;
        }
    }
};

template<typename T>
class Delegate;

template<typename... Params>
class Delegate<void(Params...)> : public DelegateBase<void, Params...>
{
public:
    using Parent = DelegateBase<void, Params...>;
    using Parent::clear;
    using Parent::invoke;
    using Parent::m_functions;
    using typename Parent::FunctionType;

    void operator()(Params... params)
    {
        for(auto&& func : m_functions)
        {
            func(params...);
        }
    }
};

template<typename T>
class ReturnDelegate;

template<typename ReturnType, typename... Params>
class ReturnDelegate<ReturnType(Params...)> : public DelegateBase<ReturnType, Params...>
{
public:
    static_assert(!std::is_void<ReturnType>::value, "ReturnDelegate can't have void return type!");

    using Parent = DelegateBase<ReturnType, Params...>;
    using Parent::clear;
    using Parent::callInvoke;
    using Parent::m_parameters;
    using Parent::m_functions;
    using typename Parent::FunctionType;

    ReturnType invoke()
    {
        ReturnType result = ReturnType();
        for(size_t i = 0; i < m_parameters.size(); i++) {
            result += callInvoke(m_parameters[i].parameters, i, std::index_sequence_for<Params...>());
        }
        return result;
    }

    ReturnType operator()(Params... params)
    {
        ReturnType sum = ReturnType();
        for(auto&& f : m_functions) {
            sum += f(params...);
        }
        return sum;
    }
};

template<typename ReturnType, class ObjType, typename... Params>
class MemberDelegateBase
{
    template<typename... T>
    struct MemberFunctionParams
    {
        size_t index = -1;
        ObjType *object;
        std::tuple<T...> parameters;
    };

public:
    typedef ReturnType (ObjType::*MemberFunctionType)(Params...);

protected:
    std::vector<MemberFunctionType> subscribers;
    std::vector<MemberFunctionParams<Params...>> parameters;

    MemberDelegateBase() = default;

public:
    void subscribe(ObjType *obj, const MemberFunctionType& method, Params... params)
    {
        subscribers.push_back(method);
        attachParameters(obj, std::tuple<Params...>(params...), std::index_sequence_for<Params...>());
    }

    void clear()
    {
        subscribers.clear();
        parameters.clear();
    }

    MemberDelegateBase& operator+=(const MemberFunctionType& rhs)
    {
        subscribers.push_back(rhs);
        return *this;
    }

    MemberDelegateBase& operator+=(const std::initializer_list<MemberFunctionType>& rhs)
    {
        for(auto f : rhs)
        {
            subscribers.push_back(f);
        }
        return *this;
    }

    MemberDelegateBase& operator-=(const MemberFunctionType& rhs)
    {
        subscribers.erase(std::remove(subscribers.begin(), subscribers.end(), rhs), subscribers.end());
        return *this;
    }

    MemberDelegateBase& operator-=(const std::initializer_list<MemberFunctionType>& rhs)
    {
        auto removeFunc = [&](const MemberFunctionType& func) -> bool {
            return std::find(rhs.begin(), rhs.end(), func) != rhs.end();
        };

        subscribers.erase(std::remove_if(subscribers.begin(), subscribers.end(), removeFunc), subscribers.end());
        return *this;
    }

private:
    void attachParameters(ObjType* obj, Params... params)
    {
        this->parameters.push_back(MemberFunctionParams<Params...>{subscribers.size()-1, obj, params...});
    }

    template<size_t... Indices>
    void attachParameters(ObjType* obj, const std::tuple<Params...>& tuple, std::index_sequence<Indices...>)
    {
        this->parameters.push_back(MemberFunctionParams<Params...>{subscribers.size()-1, obj, tuple});
    }
};

template<typename ObjType, typename... Params>
class MemberDelegate : public MemberDelegateBase<void, ObjType, Params...>
{
public:
    using Parent = MemberDelegateBase<void, ObjType, Params...>;
    using Parent::parameters;
    using Parent::subscribers;
    using typename Parent::MemberFunctionType;

    void invoke()
    {
        for(size_t i = 0; i < parameters.size(); i++)
        {
            helperMemberInvoke(parameters[i].object, parameters[i].parameters, i, std::index_sequence_for<Params...>()); 
        }
        return;
    }

    void operator()(ObjType* obj, Params... params)
    {
        for(auto&& func : subscribers) {
            (obj->*func)(params...);
        }
    }

private:
    template<size_t... Indices>
    void helperMemberInvoke(ObjType* obj, const std::tuple<Params...>& tuple, int index, std::index_sequence<Indices...>)
    {
        (obj->*subscribers[index])(std::get<Indices>(tuple)...);
    }
};

}


#endif