#ifndef SINGLEINSTANCE_H
#define SINGLEINSTANCE_H

#include <memory>

namespace hxk
{

template<class T>
class SingleInstance final
{
public:
    static T* GetInstance()
    {
        static T ins;
        return &ins;
    }

private:
    SingleInstance();
    ~SingleInstance();

    SingleInstance(const SingleInstance&);
    SingleInstance& operator=(const SingleInstance&);
};


template<typename T>
class SingleInstancePtr final
{
private:
    /// @brief 将构造函数和析构函数声明未private，禁止外部创建该类
    SingleInstancePtr();
    ~SingleInstancePtr();

    SingleInstancePtr(const SingleInstancePtr&);
    const SingleInstancePtr operator=(const SingleInstancePtr&);
public:
    static std::shared_ptr<T> GetInstancePtr()
    {
        static std::shared_ptr<T> ins = std::make_shared<T>();
        return ins;
    }
};

}
#endif