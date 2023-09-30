#ifndef LOG_BUFFER_H
#define LOG_BUFFER_H


#include "code/util/Noncopyable.h"
#include <new>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>


namespace hxk
{
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;


template<int SIZE>
class LogBuffer : public Noncopyable
{
public:
    using _uptr = std::unique_ptr<LogBuffer<SIZE>>;
    LogBuffer(): m_cur(m_data){}
    ~LogBuffer() {}


    bool append(const char* buf, size_t len)
    {
        if(avail() > len)
        {
            memcpy(m_cur, buf, len);
            m_cur += len;
            return true;
        }
        return false;
    }
    bool append(const std::string& str)
    {
        return append(str.data(), str.length());
    }


    const std::string toString() const
    {
        return std::string(m_data);
    }


    const char* data() const
    {
        return m_data;
    }
    char* current()
    {
        return m_cur;
    }


    size_t length() const
    {
        return static_cast<size_t>(m_cur - m_data);
    }


    size_t avail() const 
    {
        return static_cast<size_t>(end() - m_cur);
    }


    void addLen(size_t len)
    {
        m_cur += len;
    }


    void clear()
    {
        m_cur = m_data;
        bzero();
    }


    void bzero()
    {
        memset(m_data, 0, sizeof(m_data));
    }


private:
    const char* end() const
    {
        return m_data + sizeof(m_data);
    }


private:
    // std::vector<char> m_data;
    char m_data[SIZE];
    char *m_cur;
};




class LogStream : public Noncopyable
{
public:
    typedef LogBuffer<kSmallBuffer> Buffer;






    LogStream& operator<<(bool v)
    {
        m_buffer.append(v ? "true " : "false" , 5);
        return *this;
    }
    LogStream& operator<<(int v)
    {
        formatInteger(v);
        return *this;
    }
    LogStream& operator<<(short v)
    {
        *this << static_cast<int>(v);
        return *this;
    }
    LogStream& operator<<(unsigned short v)
    {
        *this << static_cast<unsigned int>(v);
        return *this;
    }
    LogStream& operator<<(unsigned int v)
    {
        formatInteger(v);
        return *this;
    }
    LogStream& operator<<(long v)
    {
        formatInteger(v);
        return *this;
    }
    LogStream& operator<<(unsigned long v)
    {
        formatInteger(v);
        return *this;
    }
    LogStream& operator<<(long long v)
    {
        formatInteger(v);
        return *this;
    }
    LogStream& operator<<(unsigned long long v)
    {
        formatInteger(v);
        return *this;
    }
    LogStream& operator<<(char v)
    {
        m_buffer.append(&v, 1);
        return *this;
    }
    LogStream& operator<<(const char* str)
    {
        if(str){
            m_buffer.append(str, static_cast<size_t>(strlen(str)));
        }
        else{
            m_buffer.append("(null)", 6);
        }
        return *this;
    }
    LogStream& operator<<(const unsigned char* str)
    {
        *this << reinterpret_cast<const char*>(str);
        return *this;
    }
    LogStream& operator<<(const std::string& str)
    {
        m_buffer.append(str.c_str(), static_cast<int>(str.size()));
        return *this;
    }
    LogStream& operator<<(const Buffer& v)
    {
        m_buffer.append(v.data(), v.length());
        return *this;
    }
    LogStream& operator<<(const void* p)
    {
        uintptr_t v = reinterpret_cast<uintptr_t>(p);
        if(m_buffer.avail() > kMaxNumericSize) {
            char *buf = m_buffer.current();
            buf[0] = '0';
            buf[1] = 'x';
            size_t len = convertHex(buf+2, v);
            m_buffer.addLen(len + 2);
        }
        return *this;
    }
    LogStream& operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }
    LogStream& operator<<(double v)
    {
        if (m_buffer.avail() >= kMaxNumericSize)
        {
            int len = snprintf(m_buffer.current(), kMaxNumericSize, "%.12g", v);
            m_buffer.addLen(len);
        }
        return *this;
    }


    void append(const char* data, int len)
    {
        m_buffer.append(data, len);
    }


    void append(const std::string& str)
    {
        append(str.c_str(), str.length());
    }


    const Buffer& buffer()
    {
        return m_buffer;
    }


    void resetBuffer()
    {
        m_buffer.clear();
    }
private:
    template<class T>
    void formatInteger(T v)
    {
        if(m_buffer.avail() >= kMaxNumericSize){
            size_t len = convert(m_buffer.current(), v);
            m_buffer.addLen(len);
        }
    }


    template<typename T>
    size_t convert(char buf[], T value)
    {
        T i = value;
        char* p = buf;
        const char digits[] = "9876543210123456789";
        const char* zero = digits + 9;
        do{
            int lsd = i % 10;
            i /= 10;
            *p++ = zero[lsd];
        }while(i != 0);


        if(value < 0) {
            *p++ = '-';
        }
        *p ='\0';
        std::reverse(buf, p);


        return p-buf;
    }


    size_t convertHex(char buf[], uintptr_t value)
    {
        const char digitsHex[] = "0123456789ABCDEF";
        uintptr_t i = value;
        char *p =buf;


        do{
            int lsd = static_cast<int>(i%16);
            i /= 16;
            *p++ = digitsHex[lsd];
        }while(i != 0);


        *p = '\0';
        std::reverse(buf, p);
        return p - buf;
    }
private:
    Buffer m_buffer;
    static const int kMaxNumericSize = 64;
};


}
#endif
