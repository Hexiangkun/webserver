#include "TimeStamp.h"

namespace hxk
{

TimeStamp::TimeStamp() : m_valid(false)
{
    this->now();
}

TimeStamp& TimeStamp::now()
{
    m_now = std::chrono::system_clock::now();
    m_valid = false;
    return *this;
}

int64_t TimeStamp::milliSeconds() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_now.time_since_epoch()).count();
}

int64_t TimeStamp::microSeconds() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(m_now.time_since_epoch()).count();
}

void TimeStamp::updateTime() const
{
    if(m_valid) {
        return;
    }

    m_valid = true;
    const time_t now(milliSeconds() / 1000UL);
    ::localtime_r(&now, &m_tm);
}

std::string TimeStamp::formatTime() const
{
    updateTime();
    char buf[64] = {0};
    int microseconds = static_cast<int>(microSeconds() % 1000 * 1000);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
        m_tm.tm_year + 1900, m_tm.tm_mon + 1, m_tm.tm_mday,
        m_tm.tm_hour, m_tm.tm_min, m_tm.tm_sec,
        microseconds);
  return buf;
}


int TimeStamp::getYear() const
{
    updateTime();
    return m_tm.tm_year + 1900;
}

int TimeStamp::getMonth() const
{
    updateTime();
    return m_tm.tm_mon + 1;
}

int TimeStamp::getDay() const
{
    updateTime();
    return m_tm.tm_mday;
}

int TimeStamp::getHour() const
{
    updateTime();
    return m_tm.tm_hour;
}

int TimeStamp::getMinute() const
{
    updateTime();
    return m_tm.tm_min;
}

int TimeStamp::getSecond() const
{
    updateTime();
    return m_tm.tm_sec;
}


}
