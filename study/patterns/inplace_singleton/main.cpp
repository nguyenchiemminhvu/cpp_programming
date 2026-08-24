#include <iostream>

#include "singleton.h"

class test_class
{
public:
    test_class(int a, const std::string& b) : m_a(a), m_b(b) {}
    void print() const
    {
        std::cout << "test_class: a = " << m_a << ", b = " << m_b << std::endl;
    }
private:
    int m_a;
    std::string m_b;
};

using test_singleton = singleton<test_class>;

int main()
{
    test_singleton::create(42, "Hello Singleton");
    test_singleton::instance().print();
    test_singleton::destroy();

    return 0;
}