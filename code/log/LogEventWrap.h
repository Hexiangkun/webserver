#ifndef LOG_EVENT_WRAP_H
#define LOG_EVENT_WRAP_H

#include "Logger.h"
#include "code/util/Util.h"

#define MAKE_LOG_WRAPEVENT(level, message)  \
    std::make_shared<hxk::LogEvent>(__FILE__, __LINE__, hxk::GetThreadID(),hxk::GetFiberID(), nullptr, message, level)


namespace hxk
{

class LogEventWrap
{

public:
    LogEventWrap(Logger::_ptr logger, LogLevel::LEVEL level);
    ~LogEventWrap();
    
    std::stringstream& getSS()
    {
        return ss;
    }

private:
    Logger::_ptr m_logger;
    LogLevel::LEVEL m_level;
    std::stringstream ss;
};

LogEventWrap::LogEventWrap(Logger::_ptr logger, LogLevel::LEVEL level):m_level(level)
{
    m_logger = logger;
}

LogEventWrap::~LogEventWrap()
{
    m_logger->log(MAKE_LOG_WRAPEVENT(m_level, ss.str()));
}


}

#endif