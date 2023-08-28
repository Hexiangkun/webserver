#include "code/config/config.h"


auto config_system_port = hxk::Config::lookUp<int>("system.port", 6666);

auto config_test_list = hxk::Config::lookUp<std::vector<std::string>>(
        "test_list", std::vector<std::string>{"vector","string"});

auto config_test_linklist = hxk::Config::lookUp<std::list<std::string>>(
    "test_linklist", std::list<std::string>{"list", "string"});
auto config_test_map = hxk::Config::lookUp<std::map<std::string, std::string>>(
    "test_map", std::map<std::string, std::string>{
                    std::make_pair("map1", "srting"),
                    std::make_pair("map2", "srting"),
                    std::make_pair("map3", "srting")});
auto config_test_set = hxk::Config::lookUp<std::set<int>>(
    "test_set", std::set<int>{10, 20, 30});


void test_ConfigVarToString()
{
    std::cout << *config_system_port << std::endl;
    std::cout << *config_test_list << std::endl;
    std::cout << *config_test_linklist << std::endl;
    std::cout << *config_test_map << std::endl;
    std::cout << *config_test_set << std::endl;
    // std::cout << *config_test_user_type << std::endl;
    // std::cout << *config_test_user_type_list << std::endl;
}

void test_loadConfig(const std::string& path)
{
    YAML::Node config;
    try
    {
        config = YAML::LoadFile(path);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    std::cout << config << std::endl;
    hxk::Config::loadFromYaml(config);
}

//自定义类的解析
struct Goods
{
public:
    std::string name;
    double price;
public:
    std::string toString() const
    {
        std::stringstream ss;
        ss << "**" << name << "** $" <<price;
        return ss.str();
    }

    bool operator==(const Goods& rhs) const
    {
        return name == rhs.name &&  price == rhs.price;
    }


};

std::ostream& operator<<(std::ostream& out, const Goods& goods)
{
    out << goods.toString();
    return out;
}

namespace hxk
{
template<>
class LexicalCast<std::string, Goods>
{
public:
    Goods operator()(const std::string& source)
    {
        auto node = YAML::Load(source);
        Goods g;
        if(node.IsMap()){
            g.name = node["name"].as<std::string>();
            g.price = node["price"].as<double>();
        }
        return g;
    }
};

template<>
class LexicalCast<Goods,std::string>
{
public:
    std::string operator()(const Goods& g)
    {
        YAML::Node node;
        node["name"] = g.name;
        node["price"] = g.price;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};
}

void test_SpecialConfigVarTostring()
{
    auto config_test_user_type = hxk::Config::lookUp<Goods>("user.goods",Goods{});
    auto config_test_user_type_list  = hxk::Config::lookUp<std::vector<Goods>>("user.goods_list",std::vector<Goods>{});
    
    std::cout << *config_test_user_type << std::endl;
    std::cout << *config_test_user_type_list << std::endl;
}

int main()
{
    //test_loadConfig("./config.yaml");
    //test_ConfigVarToString();
    test_SpecialConfigVarTostring();
    return 0;
}