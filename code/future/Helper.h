#ifndef HELPER_H
#define HRLPER_H

#include <tuple>
#include <vector>
#include <memory>
#include <mutex>

namespace hxk
{

template<typename T>
class Future;

template<typename T>
class Promise;

template<typename T>
class Try;

template<typename T>
struct TryWrapper;

namespace internal
{
    /// @brief 使用std::declval推断出函数返回值类型，
    /// @brief 用于获取函数类型F在给定参数Args下返回值类型，即F(Args...)的返回值类型
    template<typename F, typename... Args>
    using ResultOf = decltype(std::declval<F>()(std::declval<Args>()...));

    template<typename F, typename... Args>
    struct ResultOfWrapper
    {
        using Type = ResultOf<F, Args...>;  //定义F(Args...)返回值类型别名
    };

    //测试F能否接受Args参数
    template<typename F, typename... Args>
    struct CanCallWith
    {
        template<typename T, typename Dummy = ResultOf<T, Args...>> //Dummy如果推导成功，返回true
        static constexpr std::true_type Check(std::nullptr_t dummy)
        {
            return std::true_type{};
        };

        template<typename Dummy>
        static constexpr std::false_type Check(...)
        {
            return std::false_type{};
        };

        typedef decltype(Check<F>(nullptr)) type;   //true如果F能够接受Args
        static constexpr bool value = type::value;
    };

    template<typename F, typename... Args>
    constexpr bool CanCallWith<F, Args...>::value;

    //当模板参数为Future<T>时，特化模板定义为true，否则为false
    template<typename T>
    struct IsFuture : std::false_type
    {
        using Inner = T;
    };

    template<typename T>
    struct IsFuture<Future<T>> : std::true_type
    {
        using Inner = T;
    };

    template<typename F, typename T>    
    struct CallableResult
    {
        //Test F call with args type : void, T&& , T&
        typedef typename std::conditional<CanCallWith<F>::value, //if true, F can call with void, F()
                                ResultOfWrapper<F>, 
                                typename std::conditional<  //NO, F() is invalid
                                    CanCallWith<F, T&&>::value,  //if true, F(T&&) is valid
                                    ResultOfWrapper<F, T&&>, // F(T&&) is OK
                                    ResultOfWrapper<F, T&>>::type>::type Arg;  //else F(T&) is valid
        
        /*
            通过CanCallWith<F>判断F是否可以被调用，如果可以，使用ResultOfWrapper<F>作为Arg类型
            否则，通过CanCallWith<F, T&&>判断F是否可以被调用，如果可以，使用ResultOfWrapper<F, T&&>作为Arg类型
            否则，使用ResultOfWrapper<F, T&>作为Arg类型
        */

        //用于判断Arg类型是否为一个Future类型的包装器
        typedef IsFuture<typename Arg::Type> IsReturnsFuture;

        typedef Future<typename IsReturnsFuture::Inner> ReturnFutureType;
    };

    template<typename F>
    struct CallableResult<F, void>
    {
        typedef typename std::conditional<CanCallWith<F>::value, //if true, F can call with void
                                ResultOfWrapper<F>, 
                                typename std::conditional<  //NO, F(void) is invalid
                                    CanCallWith<F, Try<void>&&>::value,  //if true, F(Try<void>&&) is valid
                                    ResultOfWrapper<F, Try<void>&&>, // F(Try<void>&&) is OK
                                    ResultOfWrapper<F, const Try<void>&>>::type>::type Arg;  //else F(const Try<void>&) is valid
        
        typedef IsFuture<typename Arg::Type> IsReturnsFuture;

        typedef Future<typename IsReturnsFuture::Inner> ReturnFutureType;
    };

    template<typename... ELEM>
    struct CollectAllVariadicContext
    {
        CollectAllVariadicContext() {}
        ~CollectAllVariadicContext() {}

        CollectAllVariadicContext(const CollectAllVariadicContext&) = delete;
        void operator=(const CollectAllVariadicContext&) = delete;

        template<typename T, size_t I>
        inline void SetPartialResult(typename TryWrapper<T>::Type&& t)
        {
            std::unique_lock<std::mutex> lock(mutex);

            std::get<I>(results) = std::move(t);    //获取std::tuple中位置I的元素，并将t的值移动到该位置
            collects.push_back(I);      //将I添加到collect向量中，表示已经收集到位置I的结果
            if(collects.size() == std::tuple_size<decltype(results)>::value) {
                lock.unlock();
                pm.SetValue(std::move(results));
            }
        }

        //_TRYELEM_，该宏接受一个或者多个类型参数ELEM。
        //在宏展开时，每个类型参数ELEM都会通过TryWrapper进行包装，并使用...表示多个包装后的类型
        #define _TRYELEM_ typename TryWrapper<ELEM>::Type...
            Promise<std::tuple<_TRYELEM_>> pm;
            std::mutex mutex;
            std::tuple<_TRYELEM_> results;
            std::vector<size_t> collects;
            typedef Future<std::tuple<_TRYELEM_>> FutureType;
        #undef _TRYELEM_
    };

    //模板模板参数
    //CTX可以接受一个或多个类型参数。
    template<template<typename... Args> class CTX, typename... Ts>
    void CollectVariadicHelper(const std::shared_ptr<CTX<Ts...>>& ctx)
    {

    }

    template<template<typename... Args> class CTX, typename... Ts, typename THead, typename... TTail>
    void CollectVariadicHelper(const std::shared_ptr<CTX<Ts...>>& ctx, THead&& head, TTail&&... tail) 
    {
        using InnerTry = typename TryWrapper<typename THead::InnerType>::Type;
        head.Then([ctx](InnerTry&& t){
            ctx->template SetPartialResult<InnerTry, sizeof...(Ts) - sizeof...(TTail) - 1>(std::move(t));
        });

        CollectVariadicHelper(ctx, std::forward<TTail>(tail)...);
    }


}

}

#endif