#ifndef LOG_BUFFER_H
#define LOG_BUFFER_H

#include "code/util/noncopyable.h"
#include <new>
#include <iostream>
#include <vector>
#include <string>
#include <memory>

namespace hxk
{
const int kSmallBuffer = 1000 * 1024;
const int kLargeBuffer = 4000 * 1024;

class LogBuffer : public noncopyable
{
public:
    using _uptr = std::unique_ptr<LogBuffer>;
    LogBuffer(const size_t size = 4000 * 1024);
    ~LogBuffer();

    bool append(const char* buf, size_t len);
    bool append(const std::string& str);
    const std::string data() const;
    void clear();
    size_t length() const;
    size_t avail() const ;
private:
    // char* m_datas;
    // char* m_cur;
    std::vector<char> m_data;
};

LogBuffer::LogBuffer(const size_t size)
{
    m_data.reserve(size);
    // try
    // {
    //     m_datas = new char[SIZE];
    // }
    // catch(const std::bad_alloc& e)
    // {
    //     std::cerr << "LogBuffer::LogBuffer exception"
    //         << e.what() << std::endl;
    // }
    // m_cur = m_datas;
    
}

LogBuffer::~LogBuffer()
{
    // delete [] m_datas;
}


bool LogBuffer::append(const char* buf, size_t len)
{
    if(avail() > len)
    {
        for(size_t i=0; i<len; i++){
            m_data.emplace_back(buf[i]);
        }
        return true;
    }
    return false;
}

bool LogBuffer::append(const std::string& str)
{
    return append(str.c_str(), str.size());
}

const std::string LogBuffer::data() const
{
    return std::string(m_data.begin(), m_data.end());
}

void LogBuffer::clear()
{
    m_data.clear();
}
size_t LogBuffer::length() const
{
    return m_data.size();
}

size_t LogBuffer::avail() const 
{
    return m_data.capacity() - m_data.size();
}

}


#endif