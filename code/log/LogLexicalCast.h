#ifndef LOG_LEXICALCAST_H
#define LOG_LEXICALCAST_H

#include "lexical_cast.h"
#include "LogConfig.h"

namespace hxk
{

template<>
class LexicalCast<std::string, std::vector<LoggerConfig>>
{
public:
    std::vector<LoggerConfig> operator()(const std::string& source)
    {
        YAML::Node node = YAML::Load(source);
        std::vector<LoggerConfig> res;
        if(node.IsSequence()) {
            for(const auto& item : node)
            {
                LoggerConfig logger_cfg;
                logger_cfg.name = item["name"] ? item["name"].as<std::string>() : "";
                logger_cfg.level = item["level"] ? (LogLevel::LEVEL)item["level"].as<int>() : LogLevel::UNKONWN;
                logger_cfg.formatter = item["formatter"] ? item["formatter"].as<std::string>() : "";

                if(item["appender"] && item["appender"].IsSequence()) {
                    for(const auto& app_item : item["appender"]) {
                        LogAppenderConfig app_cfg;
                        app_cfg.type = app_item["type"] ? (LogAppenderConfig::Type)(app_item["type"].as<int>()) : LogAppenderConfig::STDOUT;
                        app_cfg.filepath = app_item["file"] ? app_item["file"].as<std::string>() : "";
                        app_cfg.level = app_item["level"] ? (LogLevel::LEVEL)app_item["level"].as<int>() : logger_cfg.level;
                        app_cfg.formatter = app_item["formatter"] ? app_item["formatter"].as<std::string>() : logger_cfg.formatter;
                        logger_cfg.appenders.emplace_back(app_cfg);
                    }
                }
                res.emplace_back(logger_cfg);
            }
        }
        return res;
    }
};

template<>
class LexicalCast<std::vector<LoggerConfig>, std::string>
{
public:
    std::string operator()(const std::vector<LoggerConfig>& source)
    {
        YAML::Node res;
        for(const auto& item: source)
        {
            YAML::Node node;
            node["name"] = item.name;
            node["level"] = (int)item.level;
            node["formatter"] = item.formatter;

            YAML::Node app_node_list;
            for(const auto& app_item : item.appenders) {
                YAML::Node app_node;
                app_node["type"] = (int)app_item.type;
                app_node["file"] = app_item.filepath;
                app_node["level"] = (int)app_item.level;
                app_node["formatter"] = app_item.formatter;
                app_node_list.push_back(app_node);
            }
            node["appender"] = app_node_list;
            res.push_back(node);
        }
        std::stringstream ss;
        ss << res;
        return ss.str();
    }
};

}


#endif