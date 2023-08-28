#ifndef CONFIG_H
#define CONFIG_H

#include "code/util/lexical_cast.h"
#include "code/util/lock.h"
#include <string>
#include <memory>
#include <algorithm>
#include <iostream>

namespace hxk
{

class ConfigVarBase
{
public:
    using _ptr = std::shared_ptr<ConfigVarBase>;

    ConfigVarBase(const std::string& name, const std::string& description):m_name(name), m_description(description)
    {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }
    virtual ~ConfigVarBase() = default;

    const std::string& getName() const
    {
        return m_name;
    }

    const std::string& getDescription() const
    {
        return m_description;
    }

    virtual std::string toString() const = 0;
    virtual bool fromString(const std::string& source) = 0;
private:
    std::string m_name; //配置项名称
    std::string m_description;  //配置项描述
};

template<typename T, typename ToStringFN = LexicalCast<T, std::string>, typename FromStringFN = LexicalCast<std::string, T>>
class ConfigVar : public ConfigVarBase
{
public:
    using _ptr = std::shared_ptr<ConfigVar>;
    using onChangeCB = std::function<void(const T& old_val, const T& new_val)>;

    ConfigVar(const std::string& name, const std::string& description, const T& value)
            :ConfigVarBase(name, description), m_value(value)
    {

    }
    T getValue() const
    {
        ReadScopedLock lock(&m_lock);
        return m_value;
    }

    void setValue(const T value)
    {
        {
            ReadScopedLock lock(&m_lock);
            if(value == m_value) {
                return;
            }
            T old_val = m_value;
            for(const auto& item : m_callback_map){
                item.second(old_val, value);
            }
        }
        WriteScopedLock lock(&m_lock);
        m_value = value;
    }

    std::string toString() const override
    {
        try
        {
            return ToStringFN()(getValue());
        }
        catch(const std::exception& e)
        {
            std::cerr << "ConfigVar::tostring exception"
                      << e.what() 
                      << " convert: "
                      << typeid(m_value).name()
                      << " to string"
                      << std::endl;
        }
        return "<error>";
    }

    bool fromString(const std::string& source) override
    {
        try
        {
            setValue(FromStringFN()(source));
            return true;
        }
        catch(const std::exception& e)
        {
            std::cerr << "ConfigVar::fromstring exception"
                      << e.what() 
                      << " convert: "
                      << " string to"
                      << typeid(m_value).name()
                      << std::endl;
        }
        return false;
    }

    uint64_t addListener(onChangeCB cb)
    {
        static uint64_t s_cb_id = 0;
        WriteScopedLock lock(&m_lock);
        m_callback_map[s_cb_id++] = cb;
        return s_cb_id;
    }

    void delListener(uint64_t key)
    {
        WriteScopedLock lock(&m_lock);
        m_callback_map.erase(key);
    }

    onChangeCB getListener(uint64_t key)
    {
        ReadScopedLock lock(&m_lock);
        if(m_callback_map.count(key)) {
            return m_callback_map[key];
        }
        else {
            return nullptr;
        }
    }

    void clearListener()
    {
        ReadScopedLock lock(&m_lock);
        m_callback_map.clear();
    }

private:
    T m_value;
    mutable RWLock m_lock;
    std::map<uint64_t, onChangeCB> m_callback_map;
};


class Config
{
public:
    using ConfigVarMap = std::map<std::string, ConfigVarBase::_ptr>;

    static ConfigVarBase::_ptr lookUp(const std::string& name)
    {
        ReadScopedLock lock(&getRWLock());
        ConfigVarMap& m_data = getConfigVarMap();
        if(m_data.count(name)) {
            return m_data[name];
        }
        else {
            return nullptr;
        }
    }

    template<typename T>
    static typename ConfigVar<T>::_ptr lookUp(const std::string& name)
    {
        auto base_ptr = lookUp(name);
        if(!base_ptr) {
            return nullptr;
        }

        auto ptr = std::dynamic_pointer_cast<ConfigVar<T>>(base_ptr);
        if(!ptr) {
            std::cerr << "Config::lookUp<T> exception, can't convert to ConfigVar<T>" << std::endl;
            throw std::bad_cast();
        }
        return ptr;
    }

    //创建或查找配置项
    template<typename T>
    static typename ConfigVar<T>::_ptr lookUp(const std::string& name, const T& val, const std::string& description = "")
    {
        auto find_ptr = lookUp<T>(name);
        if(find_ptr) {
            return find_ptr;
        }
        if(name.find_first_not_of("qwertyuiopasdfghjklzxcvbnm0123456789._") != std::string::npos) {
            std::cerr << "Config::lookUp<T> exception, Parameters can only start with an alphanumeric dot or underscore." << std::endl;
            throw std::invalid_argument(name);
        }

        auto ptr = std::make_shared<ConfigVar<T>>(name, description, val);
        WriteScopedLock lock(&getRWLock());
        getConfigVarMap()[name] = ptr;
        return ptr;
    }

    static void loadFromYaml(const YAML::Node& root)
    {
        std::vector<std::pair<std::string, YAML::Node>> node_list;
        travelNode(root, "", node_list);
        for(const auto& node : node_list) {
            std::string key = node.first;
            if(key.empty()) {
                continue;
            }
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);

            auto ptr = lookUp(key);

            if(ptr) {
                std::stringstream ss;
                ss << node.second;
                ptr->fromString(ss.str());
            }
        }
    }

private:
    static RWLock& getRWLock()
    {
        static RWLock rw_lock;
        return rw_lock;
    }

    static ConfigVarMap& getConfigVarMap()
    {
        static ConfigVarMap m_data;
        return m_data;
    }

    static void travelNode(const YAML::Node& node, const std::string& name, std::vector<std::pair<std::string, YAML::Node>>& node_list)
    {
        auto iter = std::find_if(node_list.begin(), node_list.end(),
                    [&name](const std::pair<std::string, YAML::Node>& item){
                        return item.first == name;
                    });
        if(iter != node_list.end()) {
            iter->second = node;
        }
        else {
            node_list.emplace_back(std::make_pair(name, node));
        }
        if(node.IsMap()) {
            for(auto item : node) {
                travelNode(item.second, name.empty() ? item.first.Scalar() : name + "." + item.first.Scalar(), node_list);
            }
        }
        if(node.IsSequence()) {
            for(size_t i = 0; i < node.size(); i++) {
                travelNode(node[i], name + "." + std::to_string(i), node_list);
            }
        }
    }
};


inline extern std::ostream& operator<<(std::ostream& out, const ConfigVarBase& cvb)
{
    {
    out << cvb.getName() << ": " << cvb.toString();
    return out;
}
}
}

#endif