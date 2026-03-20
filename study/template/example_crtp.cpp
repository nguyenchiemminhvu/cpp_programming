/**
 * CRTP (Curiously Recurring Template pattern) is a C++ programming technique in which a class template derives from itself, using the derived class as a template parameter. This allows the base class to access members of the derived class, enabling static polymorphism and code reuse without the overhead of virtual functions.
 */

#include <iostream>

template <typename T>
class base
{
public:
    void virtual_function()
    {
        static_cast<T*>(this)->virtual_function();
    }
};

class derived : public base<derived>
{
public:
    void virtual_function()
    {
        std::cout << "Derived class implementation" << std::endl;
    }
};

class another_derived : public base<another_derived>
{
public:
    void virtual_function()
    {
        std::cout << "Another derived class implementation" << std::endl;
    }
};

int main()
{
    base<derived> *b = new derived();
    b->virtual_function(); // Calls derived class implementation
    delete b;

    base<another_derived> *a = new another_derived();
    a->virtual_function(); // Calls another derived class implementation
    delete a;

    return 0;
}