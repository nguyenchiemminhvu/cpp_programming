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

    int local_val = 300;
    auto f = [local_val]() { return local_val; }; // f is a lambda that captures local_val by value and return value
    decltype(auto) g = [&local_val]() -> int& { return local_val; }; // g is a lambda that captures local_val by reference and returns a reference to it

    auto c = f(); // c is of type int, gets a copy of local_val
    c = 400; // modifies c, not local_val
    std::cout << "local_val: " << local_val << std::endl; // prints 300

    decltype(auto) d = g(); // d is of type int&, gets a reference to local_val
    d = 500; // modifies local_val through reference
    std::cout << "local_val: " << local_val << std::endl; // prints 500

    return 0;
}