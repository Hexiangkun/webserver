#include "timeUtil.h"

namespace hxk
{

TimeUtil::TimeUtil() : m_valid(false)
{
    this->now();
}

void TimeUtil::now()
{
    m_now = std::chrono::system_clock::now();
    m_valid = false;
}

int64_t TimeUtil::milliSeconds() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_now.time_since_epoch()).count();
}

int64_t TimeUtil::microSeconds() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(m_now.time_since_epoch()).count();
}

void TimeUtil::updateTime() const
{
    if(m_valid) {
        return;
    }

    m_valid = true;
    const time_t now(milliSeconds() / 1000UL);
    ::localtime_r(&now, &m_tm);
}

std::once_flag TimeUtil::m_init;
const char* TimeUtil::m_year[] = {
    "2015", "2016", "2017", "2018", "2019",
    "2020", "2021", "2022", "2023", "2024",
    "2025", "2026", "2027", "2028", "2029",
    "2030", "2031", "2032", "2033", "2034",
    "2035", "2036", "2037", "2038", "2039",
    "2040", "2041", "2042", "2043", "2044",
    "2045", "2046", "2047", "2048", "2049",
    nullptr,
};

char TimeUtil::m_number[60][2] = {""};


void TimeUtil::Init()
{
    for(size_t i=0; i < sizeof(m_number) / sizeof(m_number[0]); i++) {
        char tmp[3];
        snprintf(tmp, 3, "%02d", static_cast<int>(i));
        memcpy(m_number[i], tmp, 2);
    }
}

std::size_t TimeUtil::formatTime(char* buf) const
{
    std::call_once(m_init, &TimeUtil::Init);
    updateTime();

    memcpy(buf, m_year[m_tm.tm_year + 1990 - 2015], 4);
    buf[4] = '-';
    memcpy(buf + 5, m_number[m_tm.tm_mon + 1], 2);
    buf[7] = '-';
    memcpy(buf + 8, m_number[m_tm.tm_mday], 2);
    buf[10] = '[';
    memcpy(buf + 11, m_number[m_tm.tm_hour], 2);
    buf[13] = ':';
    memcpy(buf + 14, m_number[m_tm.tm_min], 2);
    buf[16] = ':';
    memcpy(buf + 17, m_number[m_tm.tm_sec], 2);
    buf[19] = '.';
    auto msec = microSeconds();
    snprintf(buf+20, 8, "%06d]", static_cast<int>(msec % 1000000));
    return 27;
}
}
