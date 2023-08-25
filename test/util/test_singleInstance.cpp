#include "singleInstance.h"

#include <string>
#include <iostream>

struct Goods
{
    std::string m_name;
    double m_price;

    Goods():m_name(""),m_price(0) {};
    Goods(std::string name, double price):m_name(name), m_price(price) {}

};

std::ostream& operator<<(std::ostream& os, const Goods& goods)
{
    os << "Goods name:" << goods.m_name << "\t" << "Goods price:" << goods.m_price << std::endl;
    return os;
}

void TEST_SingleInstance()
{
    Goods *ins = hxk::SingleInstance<Goods>::GetInstance();
    ins->m_name = "pencil";
    ins->m_price = 10;
    std::cout << *ins ;
}

void TEST_SingleInstancePtr()
{
    auto ins = hxk::SingleInstancePtr<Goods>::GetInstancePtr();
    ins->m_name = "pencil";
    ins->m_price = 10;
    std::cout << *ins ;
}


int main()
{
    std::cout << "TEST_SingleInstance begin" << std::endl;
    TEST_SingleInstance();
    std::cout << "TEST_SingleInstance end" << std::endl;
    std::cout << std::endl;

    std::cout << "TEST_SingleInstance begin" << std::endl;
    TEST_SingleInstancePtr();
    std::cout << "TEST_SingleInstance end" << std::endl;
}