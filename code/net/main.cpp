#include <iostream>
#include <string>
#include <memory>

class Person
{
public:
    Person(){}
    Person(const std::string name, const int age) :m_name(name), m_age(age), m_level(m_age/10){}
    void show()
    {
        std::cout << "name is " << m_name << ",age is " << m_age;
        std::cout << ",level is " << m_level << std::endl;
    }
private:
    std::string  m_name;

    int          m_age;
        int          m_level;
};


 
void test()
{
    Person  me("lucas", 35);
    me.show();
}



int main() {
    std::shared_ptr<int> sharedPtr = std::make_shared<int>(42);

    // 将 std::shared_ptr 的地址保存在 void* 变量中
    void* voidPtr = static_cast<void*>(&sharedPtr);

    // 将 void* 转换回 std::shared_ptr
    std::shared_ptr<int>* convertedSharedPtr = static_cast<std::shared_ptr<int>*>(voidPtr);

    // 使用转换后的 std::shared_ptr
    std::cout << (*convertedSharedPtr) << std::endl;  // 输出: 42

    return 0;
}

