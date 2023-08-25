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
class TryWrapper;

namespace internal
{
    //用于获取函数类型F在给定参数Args下返回值类型
    template<typename F, typename... Args>
    using ResultOf = decltype(std::declval<F>()(std::declval<Args>()...));

    template<typename F, typename... Args>
    struct ResultOfWrapper
    {
        using Type = ResultOf<F, Args...>;
    };

    //测试F能否接受Args参数
    template<typename F, typename... Args>
    struct CanCallWith
    {
        template<typename T, typename Dummy = ResultOf<T, Args...>>
        static constexpr std::true_type Check(std::nullptr_t dummy)
        {
            return std::true_type{};
        }

        template<typename T, typename Dummy = ResultOf<T, Args...>>
        static constexpr std::false_type Check(...)
        {
            return std::false_type();
        }

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
        typedef std::conditional<CanCallWith<F>::value, //if true, F can call with void
                                ResultOfWrapper<F>,
                                typename std::conditional<CanCallWith<F, T&&>::value,
                                                            ResultOfWrapper<F, T&&>,
                                                            ResultOfWrapper<F, T&>>::type>::type Arg;
        /*
            通过CanCallWith<F>判断F是否可以被调用，如果可以，使用ResultOfWrapper<F>作为Arg类型
            否则，通过CanCallWith<F, T&&>判断F是否可以被调用，如果可以，使用ResultOfWrapper<F, T&&>作为Arg类型
            否则，使用ResultOfWrapper<F, T&>作为Arg类型
        */

        //用于判断Arg类型是否为一个Future类型的包装器
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

            std::get<I>(results) = std::move(t);
            collects.push_back(I);
            if(collects.size() == std::tuple_size<decltype<results>::value) {
                lock.unlock();
                pm.SetValue(std::move(results));
            }
        }

        #define _TRYELEM_ typename TryWrapper<ELEM>::Type...
            Promise<std::tuple<_TRYELEM_>> pm;
            std::mutex mutex;
            std::tuple<_TRYELEM_> results;
            std::vector<size_t> collects;
            typedef Future<std::tuple<_TRYELEM_>> FutureType;
        #undef _TRYELEM_
    };

    template<template<typename...> class CTX, typename... Ts>
    void CollectVariadicHelper(const std::shared_ptr<CTX<Ts...>>&)
    {

    }

    template<template<typename...> class CTX, typename... Ts, typename THead, typename... TTail>
    void CollectVariadicHelper(const std::shared_ptr<CTX<Ts...>>& ctx, THead&& head, TTail&&... tail) 
    {
        using InnerTry = typename TryWrapper<typename Thead::InnerType>::Type;
        head.then([ctx](InnerTry&& t){
            ctx->template SetPartialResult<InnerTry, sizeof...(Ts) - sizeof...(TTail) - 1>(std::move(t));
        });

        CollectVariadicHelper(ctx, std::forward<TTail>(tail)...);
    }


}

};

#endif