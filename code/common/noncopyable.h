#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace hxk
{

/// @brief 禁止类的拷贝构造
class noncopyable
{
private:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
protected:
    noncopyable(/* args */) = default;
    ~noncopyable() = default;
};


}

#endif