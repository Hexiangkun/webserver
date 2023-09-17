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

    int getYear() const;
    int getMonth() const;
    int getDay() const;
    int getHour() const;
    int getMinute() const;
    int getSecond() const;

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