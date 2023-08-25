#ifndef TIME_UTIL_H
#define TIME_UTIL_H

#include <cstdint>
#include <chrono>
#include <mutex>
#include <string>
#include <cstring>
#include <ctime>
namespace hxk
{

class TimeUtil
{
public:
    TimeUtil();

    void now();

    int64_t milliSeconds() const;
    int64_t microSeconds() const;

    std::size_t formatTime(char* buf) const;
    std::string formatTime() const;
    int getYear() const
    {
        updateTime();
        return m_tm.tm_year + 1900;
    }
    int getMonth() const
    {
        updateTime();
        return m_tm.tm_mon + 1;
    }
    int getDay() const
    {
        updateTime();
        return m_tm.tm_mday;
    }
    int getHour() const
    {
        updateTime();
        return m_tm.tm_hour;
    }
    int getMinute() const
    {
        updateTime();
        return m_tm.tm_min;
    }
    int getSecond() const
    {
        updateTime();
        return m_tm.tm_sec;
    }

    operator int64_t() const
    {
        return milliSeconds();
    }


private:
    void updateTime() const;
    static void Init();

private:
    std::chrono::system_clock::time_point m_now;
    mutable tm m_tm;
    mutable bool m_valid;

    static std::once_flag m_init;
    static const char* m_year[];
    static char m_number[60][2];
};

}

#endif