#include <iostream>

class base
{
public:
    base() = default;
    base(const base&) = delete; // delete copy constructor
    base& operator=(const base&) = delete; // delete copy assignment operator

    virtual void foo() = delete; // delete member function
    // void bar() = default; // error: default member function
};

class derived : public base
{
public:
    derived() = default;
    derived(const derived&) = delete; // delete copy constructor
    derived& operator=(const derived&) = delete; // delete copy assignment operator
};

int main()
{
    return 0;
}
