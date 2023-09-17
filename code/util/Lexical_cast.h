#ifndef LEXICAL_CAST_H
#define LEXICAL_CAST_H

#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <vector>

/// @brief std::string 和 std::vector/std::map/std::list/std::set 之间的相互转换
/// @brief 如果需要其他类型，则可以自己实现对应的转换
namespace hxk
{

/// @brief 使用boost::lexical_cast将Source类型转换为Target类型
/// @tparam Source 源类型
/// @tparam Target 目标类型
template<class Source, class Target>
class LexicalCast
{
public:
    Target operator()(const Source& source)
    {
        return boost::lexical_cast<Target>(source);
    }
};

/// @brief 将std::string(yaml::Sequence)转换为std::vector<T>
/// @tparam Target 目标类型
template<typename Target>
class LexicalCast<std::string, std::vector<Target>>
{
public:
    std::vector<Target> operator()(const std::string& source)
    {
        YAML::Node node = YAML::Load(source);
        std::vector<Target> res;
        if(node.IsSequence()){
            std::stringstream ss;
            for(const auto& iter : node) {
                ss.str("");
                ss << iter;
                res.emplace_back(LexicalCast<std::string, Target>()(ss.str()));
            }
        }
        return res;
    }
};

template<typename T>
class LexicalCast<std::vector<T>, std::string>
{
public:
    std::string operator()(const std::vector<T>& source)
    {
        YAML::Node node;
        for(const auto& item : source) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(item)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/// @brief 将std::string(yaml::Sequence)转换为std::vector<T>
/// @tparam Target 目标类型
template<typename T>
class LexicalCast<std::string, std::list<T>>
{
public:
    std::list<T> operator()(const std::string& source)
    {
        YAML::Node node = YAML::Load(source);
        std::list<T> res;
        if(node.IsSequence()){
            std::stringstream ss;
            for(const auto& item : node) {
                ss.str("");
                ss << item;
                res.emplace_back(LexicalCast<std::string, T>()(ss.str()));
            }
        }
        return res;
    }
};

template<typename T>
class LexicalCast<std::list<T>, std::string>
{
public:
    std::string operator()(const std::list<T>& source)
    {
        YAML::Node node;
        for(const auto& item : source) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(item)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class LexicalCast<std::string, std::map<std::string, T>>
{
public:
    std::map<std::string, T> operator()(const std::string& source)
    {
        YAML::Node node = YAML::Load(source);
        std::map<std::string, T> res;
        if(node.IsMap()) {
            std::stringstream ss;
            for(const auto item : node) {
                ss.str("");
                ss << item.second;
                res.insert(std::make_pair(item.first.as<std::string>(), LexicalCast<std::string, T>()(ss.str())));
            }
        }
        return res;
    }
};

template<typename T>
class LexicalCast<std::map<std::string, T>, std::string>
{
public:
    std::string operator()(const std::map<std::string, T>& source)
    {
        YAML::Node node;
        for(const auto& item : source) {
            node[item.first] = YAML::Load(LexicalCast<T, std::string>()(item.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class LexicalCast<std::string, std::set<T>>
{
public:
    std::set<T> operator()(const std::string& source)
    {
        YAML::Node node = YAML::Load(source);
        std::set<T> res;
        if(node.IsSequence()) {
            std::stringstream ss;
            for(const auto& item : node) {
                ss.str("");
                ss << item;
                res.insert(LexicalCast<std::string, T>()(ss.str()));
            }
        }
        return res;
    }
};


template<typename T>
class LexicalCast<std::set<T>, std::string>
{
public:
    std::string operator()(const std::set<T>& source)
    {
        YAML::Node node;
        for(const auto& item : source) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(item)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

}

#endif
