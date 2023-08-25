#ifndef LOG_LEVEL_H
#define LOG_LEVEL_H

#include <string>

namespace hxk
{

#define XX(name) \
    case LogLevel::name:    \
        return #name;   \
        break;

class LogLevel
{
private:
    /* data */
public:
    enum LEVEL
    {
        UNKONWN = 0,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };
    LogLevel(/* args */) = default;
    ~LogLevel() = default;

    static std::string levelToString(LogLevel::LEVEL level)
    {
        switch (level)
        {
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
        default:
            return "UNKNOWN";
            break;
        }
        return "UNKNOWN";
    }

    static LogLevel::LEVEL fromStringToLevel(const std::string& str)
    {
        #define XXX(level, v)   \
            if(str == #v) {     \
                return LogLevel::level; \
            }
        
            XXX(DEBUG, debug);
            XXX(INFO, info);
            XXX(WARN, warn);
            XXX(ERROR, error);
            XXX(FATAL, fatal);
            XXX(DEBUG, DEBUG);
            XXX(INFO, INFO);
            XXX(WARN, WARN);
            XXX(ERROR, ERROR);
            XXX(FATAL, FATAL);
            return LogLevel::UNKONWN;
        #undef XXX
    }
};

}

#endif