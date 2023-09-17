#ifndef LOG_CONFIG_H
#define LOG_CONFIG_H

#include "LogLevel.h"
#include <vector>

namespace hxk
{

struct LogAppenderConfig
{
    enum Type
    {
        STDOUT = 0,
        FILE,
        ASYNCFILE
    };

    LogAppenderConfig::Type type;   //输出的类型
    LogLevel::LEVEL level;          //输出器有效日志等级
    std::string formatter;          //输出器格式化日志的样式
    std::string filepath;           //输出到目标文件

    LogAppenderConfig():type(Type::STDOUT), level(LogLevel::UNKONWN)
    {

    }
    LogAppenderConfig(LogAppenderConfig::Type type, LogLevel::LEVEL level, const std::string& pattern, const std::string& filepath)
                :type(type), level(level), formatter(pattern), filepath(filepath)
    {

    }

    bool operator==(const LogAppenderConfig& rhs)
    {
        return type == rhs.type && level == rhs.level && 
                formatter == rhs.formatter &&
                filepath == rhs.filepath;
    }
};

struct LoggerConfig
{
    std::string name;   //日志器的名称
    LogLevel::LEVEL level;  //日志器的输出等级
    std::string formatter;  //日志格式化
    std::vector<LogAppenderConfig> appenders;

    LoggerConfig():level(LogLevel::UNKONWN){}
    LoggerConfig(const std::string& name, LogLevel::LEVEL level, const std::string& pattern)
            :name(name), level(level), formatter(formatter)
    {
        
    }
    bool operator==(const LoggerConfig& rhs) const
    {
        return name == rhs.name;
    }
};

}

#endif