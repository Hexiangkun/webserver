#ifndef TIME_UTIL_H
#define TIME_UTIL_H

#include <cstdint>
#include <chrono>
#include <mutex>
#include <string>
#include <cstring>
#include <ctime>
#include <memory>
namespace hxk
{

class TimeStamp
{
public:
    typedef std::shared_ptr<TimeStamp> _ptr;
    TimeStamp();

    TimeStamp& now();

    int64_t milliSeconds() const;
    int64_t microSeconds() const;

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
};


inline bool operator<(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.microSeconds() < rhs.microSeconds();
}

inline bool operator==(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.microSeconds() == rhs.microSeconds();
}

inline bool operator>(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.microSeconds() > rhs.microSeconds();
}

inline double timeDifference(TimeStamp high, TimeStamp low)
{
    int64_t diff = high.microSeconds() - low.microSeconds();
    return static_cast<double>(diff) / (1000 * 1000);
}

}

#endif