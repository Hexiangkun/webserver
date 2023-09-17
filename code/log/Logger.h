#ifndef LOGGER_H
#define LOGGER_H

#include "LogAppender.h"
#include <list>
#include <algorithm>

namespace hxk
{

class Logger
{

public:
    using _ptr = std::shared_ptr<Logger>;
    Logger();
    Logger(const std::string& name, LogLevel::LEVEL level, const std::string& pattern);
    ~Logger();

    void log(LogEvent::_ptr ev);

    void addAppender(LogAppender::_ptr appender);
    void delAppender(LogAppender::_ptr appender);

    LogLevel::LEVEL getLevel() const;
    void setLevel(LogLevel::LEVEL level);

    const std::string getName();
    void setName(const std::string& name);
    const std::string getLoggerName();

private:
    LogLevel::LEVEL m_level;
    std::string m_name;   //日志器名称
    std::string m_formatter_pattern;
    LogFormatter::_ptr m_formatter;
    std::list<LogAppender::_ptr> m_appender_list;
    Mutex m_mutex;
};

Logger::Logger():m_level(LogLevel::DEBUG), m_name("default"), 
                m_formatter_pattern("[%d] [%p] [T:%t F:%F]%T%m%n")
{
    m_formatter.reset(new LogFormatter(m_formatter_pattern));
}

Logger::Logger(const std::string& name, LogLevel::LEVEL level, const std::string& pattern)
                :m_name(name), m_level(level), m_formatter_pattern(pattern)
{
    m_formatter.reset(new LogFormatter(m_formatter_pattern));
}

Logger::~Logger()
{
}

void Logger::addAppender(LogAppender::_ptr appender)
{
    ScopedLock lock(&m_mutex);
    if(!appender->getFormatter()) {
        appender->setFormatter(m_formatter);
    }
    m_appender_list.push_back(appender);
}

void Logger::delAppender(LogAppender::_ptr appender)
{
    ScopedLock lock(&m_mutex);
    auto iter = std::find(m_appender_list.begin(), m_appender_list.end(),appender);
    if(iter == m_appender_list.end()) {
        return;
    }
    m_appender_list.erase(iter);
}

LogLevel::LEVEL Logger::getLevel() const
{
    return m_level;
}

void Logger::setLevel(LogLevel::LEVEL level)
{
    m_level = level;
}

const std::string Logger::getLoggerName()
{
    return m_name;
}

void Logger::log(LogEvent::_ptr ev)
{
    if(ev->getLevel() < m_level) {
        return;
    }
    ScopedLock lock(&m_mutex);
    for(const auto& item : m_appender_list) {
        item->log(ev->getLevel(), ev);
    }
}

const std::string Logger::getName()
{
    return m_name;
}

void Logger::setName(const std::string& name)
{
    m_name = name;
}

}


#endif