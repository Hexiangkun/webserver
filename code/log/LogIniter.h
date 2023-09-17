#ifndef LOG_INITER_H
#define LOG_INITER_H

#include "code/config/Config.h"
#include "LogConfig.h"
#include "LoggerManager.h"

namespace hxk
{

struct LogIniter
{
    LogIniter()
    {
        auto log_config_list = hxk::Config::lookUp<std::vector<LoggerConfig>>("logs", {}, "日志器的配置项");
        log_config_list->addListener([](const std::vector<LoggerConfig>& old_val, const std::vector<LoggerConfig>& new_val){
            std::cout << "日志器配置变动，更新日志器" << std::endl;
            LoggerManager_Ptr::GetInstancePtr()->init(new_val);
            //LoggerManager_Ptr::GetInstance()->update_init<std::vector<LogConfig>>(new_val);
        });
    }
};

static LogIniter __log_initer__;    //静态变量，程序开始一直存在
}

#endif