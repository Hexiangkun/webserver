#include <typeinfo>

#include <iostream>

template<typename F>
void fun(F&& f)
{
    using T = typename std::decay<F>::type;
    auto func = [fu = std::forward<T>(f)](){
        fu(1);
    };
    func();
    std::cout  << typeid(typename std::decay<F>::type).name() << std::endl;
}

int main()
{
    auto f = [](int a){
        std::cout << a << std::endl;
    };
    fun(f);
}