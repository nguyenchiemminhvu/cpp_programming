#include <iostream>
#include <cstdint>

int global_val = 42;

int& get_ref()
{
    return global_val;
}

int main()
{
    auto a = get_ref(); // a is of type int
    decltype(auto) b = get_ref(); // b is of type int&

    a = 100; // modifies a, not global_val
    std::cout << "global_val: " << global_val << std::endl; // prints 42

    b = 200; // modifies global_val through reference
    std::cout << "global_val: " << global_val << std::endl; // prints 200

    return 0;
}