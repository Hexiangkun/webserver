#ifndef LOG_FORMATTER
#define LOG_FORMATTER

#include "LogEvent.h"
#include "LogBuffer.h"
#include <vector>
#include <iostream>
#include <map>
#include <assert.h>
#include <sstream>

namespace hxk
{

class FormatItem
{
public:
    typedef std::shared_ptr<FormatItem> _ptr;
    virtual void format(std::ostream& os, LogEvent::_ptr& ev) = 0;
    virtual void Format(LogBuffer<kSmallBuffer>& buf, LogEvent::_ptr& ev) = 0;
};

/// @brief 输出字符串
class PlainFormatItem : public FormatItem
{
public:
    explicit PlainFormatItem(const std::string& str) : m_str(str){}
    void format(std::ostream& out, LogEvent::_ptr& ev) override
    {
        out << m_str;
    }
    void Format(LogBuffer<kSmallBuffer>& buf, LogEvent::_ptr& ev)
    {
        buf.append(m_str);
    }
private:
    std::string m_str;
};

/// @brief 输出日志等级
class LevelFormatItem : public FormatItem
{
public: 
    void format(std::ostream& out, LogEvent::_ptr& ev)
    {
        out << LogLevel::levelToString(ev->getLevel());
    }
    void Format(LogBuffer<kSmallBuffer>& buf, LogEvent::_ptr& ev)
    {
        buf.append(LogLevel::levelToString(ev->getLevel()));
    }
};

/// @brief 输出文件名
class FileNameFormatItem : public FormatItem
{
public: 
    void format(std::ostream& out, LogEvent::_ptr& ev)
    {
        out << ev->getFilename();
    }
    void Format(LogBuffer<kSmallBuffer>& buf, LogEvent::_ptr& ev)
    {
        buf.append(ev->getFilename());
    }
};

/// @brief 输出行号
class LineFormatItem : public FormatItem
{
public: 
    void format(std::ostream& out, LogEvent::_ptr& ev)
    {
        out << ev->getLine();
    }
    void Format(LogBuffer<kSmallBuffer>& buf, LogEvent::_ptr& ev)
    {
        buf.append(std::to_string(ev->getLine()));
    }
};

/// @brief 输出线程id
class ThreadIdFormatItem : public FormatItem
{
public: 
    void format(std::ostream& out, LogEvent::_ptr& ev)
    {
        out << ev->getThreadId();
    }
    void Format(LogBuffer<kSmallBuffer>& buf, LogEvent::_ptr& ev)
    {
        buf.append(std::to_string(ev->getThreadId()));
    }
};

/// @brief 输出协程id
class FiberIdFormatItem : public FormatItem
{
public: 
    void format(std::ostream& out, LogEvent::_ptr& ev)
    {
        out << ev->getFiberId();
    }
    void Format(LogBuffer<kSmallBuffer>& buf, LogEvent::_ptr& ev)
    {
        buf.append(std::to_string(ev->getFiberId()));
    }
};

/// @brief 输出时间
class TimeFormatItem : public FormatItem
{
public: 
    explicit TimeFormatItem(const std::string &str = "%Y-%m-%d %H:%M:%S"):m_time_pattern(str)
    {
        if(m_time_pattern.empty()){
            m_time_pattern = "%Y-%m-%d %H:%M:%S";
        }
    }
    void format(std::ostream& out, LogEvent::_ptr& ev)
    {
        struct tm t;
        TimeStamp::_ptr tt = ev->getTime();
        out << tt->formatTime();
    }
    void Format(LogBuffer<kSmallBuffer>& buf, LogEvent::_ptr& ev)
    {
        struct tm t;
        TimeStamp::_ptr tt = ev->getTime();
        buf.append(tt->formatTime());
    }
private:
    std::string m_time_pattern;
};

/// @brief 输出内容
class ContentFormatItem : public FormatItem
{
public: 
    void format(std::ostream& out, LogEvent::_ptr& ev)
    {
        out << ev->getContent();
    }
    void Format(LogBuffer<kSmallBuffer>& buf, LogEvent::_ptr& ev)
    {
        buf.append(ev->getContent());
    }
};

/// @brief 换行
class NewLineFormatItem : public FormatItem
{
public: 
    void format(std::ostream& out, LogEvent::_ptr& ev)
    {
        out << std::endl;
    }
    void Format(LogBuffer<kSmallBuffer>& buf, LogEvent::_ptr& ev)
    {
        buf.append("\n", 2);
    }
};

/// @brief 百分号%
class PercentSignFormatItem : public FormatItem
{
public: 
    void format(std::ostream& out, LogEvent::_ptr& ev)
    {
        out << "%";
    }
    void Format(LogBuffer<kSmallBuffer>& buf, LogEvent::_ptr& ev)
    {
        buf.append("%", 1);
    }
};

/// @brief 制表符
class TabFormatItem : public FormatItem
{
public: 
    void format(std::ostream& out, LogEvent::_ptr& ev)
    {
        out << '\t';
    }
    void Format(LogBuffer<kSmallBuffer>& buf, LogEvent::_ptr& ev)
    {
        buf.append("\t");
    }
};

thread_local static std::map<char, FormatItem::_ptr> format_item_map{
#define FN(CH, ITEM_NAME)                   \
    {                                       \
        CH, std::make_shared<ITEM_NAME>()   \
    } 
    FN('p', LevelFormatItem),
    FN('f', FileNameFormatItem),
    FN('l', LineFormatItem),
    FN('d', TimeFormatItem),
    FN('F', FiberIdFormatItem),
    FN('t', ThreadIdFormatItem),
    FN('m', ContentFormatItem),
    FN('n', NewLineFormatItem),
    FN('%', PercentSignFormatItem),
    FN('T', TabFormatItem),
#undef FN
};

class LogFormatter
{
public:
    using _ptr = std::shared_ptr<LogFormatter>;

    LogFormatter(const std::string& pattern);
    ~LogFormatter();

    std::string format(LogEvent::_ptr& ev);
    std::string Format(LogEvent::_ptr& ev);

private:
    /// @brief 解析m_format_pattern，将对应的字符的格式解析出来放到m_format_items中
    void init();

private:
    std::string m_format_pattern;
    std::vector<FormatItem::_ptr> m_format_items;   //
};

LogFormatter::LogFormatter(const std::string& pattern):m_format_pattern(pattern)
{
    init();
}

LogFormatter::~LogFormatter()
{
    
}

std::string LogFormatter::format(LogEvent::_ptr& ev)
{
    std::stringstream ss;
    for(const auto& item : m_format_items) {
        item->format(ss, ev);
    }
    return ss.str();
}

std::string LogFormatter::Format(LogEvent::_ptr& ev)
{
    LogBuffer<kSmallBuffer> buf;
    for(const auto& item : m_format_items) {
        item->Format(buf, ev);
    }
    return buf.toString();
}

void LogFormatter::init()
{
    enum PRASE_STATUS
    {
        SCAN_STATUS,    //扫描到普通字符
        CREATE_STATUS   //扫描到%,处理占位符
    };
    PRASE_STATUS status = SCAN_STATUS;
    ssize_t str_begin = 0;
    ssize_t str_end = 0;
    //日志格式"[%d] [%p] [%f]%T%m%n"
    for(size_t i = 0; i < m_format_pattern.size(); i++) 
    {
        switch (status)
        {
        case SCAN_STATUS:
            str_begin = i;
            for(str_end = i ; str_end < m_format_pattern.size(); str_end++) {
                if(m_format_pattern[str_end] == '%') {
                    status = CREATE_STATUS;
                    break;
                }
            }
            i = str_end;
            m_format_items.emplace_back(std::make_shared<PlainFormatItem>(m_format_pattern.substr(str_begin, str_end-str_begin)));
            break;
        case CREATE_STATUS:
            assert(!format_item_map.empty() && "format_item_map don't init");
            auto iter = format_item_map.find(m_format_pattern[i]);
            if(iter == format_item_map.end()) {
                m_format_items.emplace_back(std::make_shared<PlainFormatItem>("<error format>"));
            }
            else {
                m_format_items.emplace_back(iter->second);
            }
            status = SCAN_STATUS;
            break;
        }
    }
}

}

#endif