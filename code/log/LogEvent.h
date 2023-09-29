#ifndef LOG_EVENT_H
#define LOG_EVENT_H

#include "LogLevel.h"
#include "code/util/TimeStamp.h"
#include <memory>

namespace hxk
{
class LogEvent
{
public:
    typedef std::shared_ptr<LogEvent> _ptr;
    LogEvent(const std::string& filename, uint32_t line, uint32_t thread_id, uint32_t fiber_id, TimeStamp::_ptr time, const std::string& content, LogLevel::LEVEL level = LogLevel::DEBUG)
            :m_level(level), m_filename(filename), m_line(line), m_thread_id(thread_id), m_fiber_id(fiber_id), m_time(time),m_content(content)
    {
        if(!m_time) {
            m_time = std::make_shared<TimeStamp>();
        }
    }
    ~LogEvent()
    {

    }

    LogLevel::LEVEL getLevel() const 
    {
        return m_level;
    }
    void setLevel(LogLevel::LEVEL level) 
    {
        m_level = level;
    }

    const std::string& getFilename() const
    {
        return m_filename;
    }

    uint32_t getLine() const 
    {
        return m_line;
    }

    uint32_t getThreadId() const 
    {
        return m_thread_id;
    }

    uint32_t getFiberId() const 
    {
        return m_fiber_id;
    }

    TimeStamp::_ptr getTime() const 
    {
        return m_time;
    }
    const std::string& getContent() const 
    {
        return m_content;
    }

private:
    LogLevel::LEVEL m_level;    //日志等级
    std::string m_filename;     //文件名
    uint32_t m_line;            //行号
    uint32_t m_thread_id;       //线程id
    uint32_t m_fiber_id;        //协程id
    TimeStamp::_ptr m_time;              //时间
    std::string m_content;      //日志内容
};


}

#endif
