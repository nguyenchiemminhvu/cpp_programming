#include <iostream>
#include <type_traits>
#include <utility>

template <typename T>
void inspect_reference(T&& arg)
{
    if (std::is_lvalue_reference<decltype(arg)>::value)
    {
        std::cout << "arg is an lvalue reference." << std::endl;
    }
    else if (std::is_rvalue_reference<decltype(arg)>::value)
    {
        std::cout << "arg is an rvalue reference." << std::endl;
    }
    else
    {
        std::cout << "arg is neither an lvalue nor an rvalue reference." << std::endl;
    }
}

int main()
{
    int val = 42;
    inspect_reference(val);          // lvalue
    inspect_reference(*&val);        // lvalue
    inspect_reference(std::move(val)); // rvalue
    inspect_reference(42);           // rvalue

    return 0;
}