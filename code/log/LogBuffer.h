#ifndef LOG_BUFFER_H
#define LOG_BUFFER_H

#include "code/util/Noncopyable.h"
#include <new>
#include <iostream>
#include <vector>
#include <string>
#include <memory>

namespace hxk
{
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template<int SIZE>
class LogBuffer : public Noncopyable
{
public:
    using _uptr = std::unique_ptr<LogBuffer<SIZE>>;
    LogBuffer();
    ~LogBuffer();

    bool append(const char* buf, size_t len);
    bool append(const std::string& str);

    const std::string toString() const;
    const char* data() const;
    char* current();

    size_t length() const;

    size_t avail() const ;

    void addLen(size_t len);
    void clear();
    void bzero();
private:
    const char* end() const;
private:
    // std::vector<char> m_data;
    char m_data[SIZE];
    char *m_cur;
};

template<int SIZE>
LogBuffer<SIZE>::LogBuffer() : m_cur(m_data)
{

}

template<int SIZE>
LogBuffer<SIZE>::~LogBuffer()
{
    
}

template<int SIZE>
bool LogBuffer<SIZE>::append(const char* buf, size_t len)
{
    if(avail() > len)
    {
        memcpy(m_cur, buf, len);
        m_cur += len;
        return true;
    }
    return false;
}

template<int SIZE>
bool LogBuffer<SIZE>::append(const std::string& str)
{
    return append(str.c_str(), str.size());
}

template<int SIZE>
const std::string LogBuffer<SIZE>::toString() const
{
    return std::string(m_data);
}

template<int SIZE>
const char* LogBuffer<SIZE>::data() const
{
    return m_data;
}

template<int SIZE>
void LogBuffer<SIZE>::addLen(size_t len)
{
    m_cur += len;
}

template<int SIZE>
size_t LogBuffer<SIZE>::length() const
{
    return static_cast<size_t>(m_cur - m_data);
}

template<int SIZE>
char* LogBuffer<SIZE>::current() 
{
    return m_cur;
}

template<int SIZE>
size_t LogBuffer<SIZE>::avail() const 
{
    return static_cast<size_t>(end() - m_cur);
}

template<int SIZE>
void LogBuffer<SIZE>::clear()
{
    m_cur = m_data;
    bzero();
}

template<int SIZE>
const char* LogBuffer<SIZE>::end() const
{
    return m_data + sizeof(m_data);
}

template<int SIZE>
void LogBuffer<SIZE>::bzero()
{
    memset(m_data, 0, sizeof(m_data));
}

}


#endif