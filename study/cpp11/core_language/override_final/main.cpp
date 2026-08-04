#include <iostream>

class base
{
public:
    virtual void f() { std::cout << "base::f()" << std::endl; }
    virtual void g() { std::cout << "base::g()" << std::endl; }
    virtual void h() { std::cout << "base::h()" << std::endl; }
};

class derived : public base
{
public:
    void f() override { std::cout << "derived::f()" << std::endl; }
    void g() final { std::cout << "derived::g()" << std::endl; }
    virtual void h() { std::cout << "derived::h()" << std::endl; }
};

class final_derived final : public derived
{
public:
    void f() override { std::cout << "final_derived::f()" << std::endl; }
    // void g() override { std::cout << "final_derived::g()" << std::endl; } // Error: cannot override final function
    void h() override { std::cout << "final_derived::h()" << std::endl; }
};

// class error_derived : public final_derived
// {
// public:
//     // void f() override { std::cout << "error_derived::f()" << std::endl; } // Error: cannot override final class
// };

int main()
{
    base* b = new final_derived();
    b->f(); // Calls final_derived::f()
    b->g(); // Calls derived::g()
    b->h(); // Calls final_derived::h()

    delete b;
    return 0;
}
