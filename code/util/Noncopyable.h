#ifndef Noncopyable_H
#define Noncopyable_H

namespace hxk
{

/// @brief 禁止类的拷贝构造
class Noncopyable
{
private:
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
protected:
    Noncopyable(/* args */) = default;
    ~Noncopyable() = default;
};


}

#endif