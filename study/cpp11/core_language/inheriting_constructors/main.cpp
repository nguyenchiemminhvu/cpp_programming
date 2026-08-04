#include <iostream>

class base
{
public:
    base(int x) {}
    base(int x, double y) {}
    base(std::string name) {}
};

class derived : public base
{
public:
    using base::base; // Inherit constructors from base class

    int extra; // danger, this member is not initialized by the inherited constructors
    int default_extra = 0; // This member will be initialized to 0 by default
};

int main()
{
    derived d1(10); // Calls base(int x)
    derived d2(10, 3.14); // Calls base(int x, double y)
    derived d3("Hello"); // Calls base(std::string name)
    return 0;
}