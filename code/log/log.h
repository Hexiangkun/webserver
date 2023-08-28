#ifndef LOG_H
#define LOG_H

#include "code/util/util.h"
#include "logger.h"
#include "logEventWrap.h"
#include "loggerManager.h"
#include "logLexicalCast.h"
#include "logIniter.h"

#define MAKE_LOG_EVENT(level, message)  \
    std::make_shared<hxk::LogEvent>(__FILE__, __LINE__, hxk::GetThreadID(), hxk::GetFiberID(), time(nullptr), message, level)

#define LOG_LEVEL(logger, level, message)   \
    logger->log(MAKE_LOG_EVENT(level, message));

#define LOG_DEBUG(logger, message) LOG_LEVEL(logger, hxk::LogLevel::DEBUG, message)
#define LOG_INFO(logger, message) LOG_LEVEL(logger, hxk::LogLevel::INFO, message)
#define LOG_WARN(logger, message) LOG_LEVEL(logger, hxk::LogLevel::WARN, message)
#define LOG_ERROR(logger, message) LOG_LEVEL(logger, hxk::LogLevel::ERROR, message)
#define LOG_FATAL(logger, message) LOG_LEVEL(logger, hxk::LogLevel::FATAL, message)

#define LOG_S_EVENT(logger, level)  \
    std::make_shared<hxk::LogEventWrap>(logger, level)->getSS()

#define LOG_S_DEBUG(logger) LOG_S_EVENT(logger, hxk::LogLevel::DEBUG)
#define LOG_S_INFO(logger) LOG_S_EVENT(logger, hxk::LogLevel::INFO)
#define LOG_S_WARN(logger) LOG_S_EVENT(logger, hxk::LogLevel::WARN)
#define LOG_S_ERROR(logger) LOG_S_EVENT(logger, hxk::LogLevel::ERROR)
#define LOG_S_FATAL(logger) LOG_S_EVENT(logger, hxk::LogLevel::FATAL)

#define LOG_FORMAT_LEVEL(logger, level, format, argv...)        \
    {                                                           \
        char* _buf = nullptr;                                   \
        int len = asprintf(&_buf, format, argv);                \
        if(len != -1){                                          \
            LOG_LEVEL(logger, level, std::string(_buf, len));   \
            free(_buf);                                         \
        }                                                       \
    }

#define LOG_FORMAT_DEBUG(logger, format, argv...) LOG_FORMAT_LEVEL(logger, hxk::LogLevel::DEBUG, format, argv)
#define LOG_FORMAT_INFO(logger, format, argv...) LOG_FORMAT_LEVEL(logger, hxk::LogLevel::INFO, format, argv)
#define LOG_FORMAT_WARN(logger, format, argv...) LOG_FORMAT_LEVEL(logger, hxk::LogLevel::WARN, format, argv)
#define LOG_FORMAT_ERROR(logger, format, argv...) LOG_FORMAT_LEVEL(logger, hxk::LogLevel::ERROR, format, argv)
#define LOG_FORMAT_FATAL(logger, format, argv...) LOG_FORMAT_LEVEL(logger, hxk::LogLevel::FATAL, format, argv)


#define GET_ROOT_LOGGER() hxk::LoggerManager_Ptr::GetInstancePtr()->getGlobalLogger()
#define GET_LOGGER(name) hxk::LoggerManager_Ptr::GetInstancePtr()->getLogger(name)


#endif