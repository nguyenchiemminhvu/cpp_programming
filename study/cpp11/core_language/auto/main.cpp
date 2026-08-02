#include <iostream>
#include <vector>
#include <typeinfo>

int main()
{
    auto x = 42; // x is of type int
    auto y = 3.14; // y is of type double
    auto z = "Hello, World!"; // z is of type const char*
    auto list = {1, 2, 3, 4, 5}; // list is of type std::initializer_list<int>
    // one element in the initializer list, so the type is deduced as std::initializer_list<int>
    auto single_ele_list = {42}; // single_ele_list is of type std::initializer_list<int>

    std::cout << "Type of x: " << typeid(x).name() << std::endl;
    std::cout << "Type of y: " << typeid(y).name() << std::endl;
    std::cout << "Type of z: " << typeid(z).name() << std::endl;
    std::cout << "Type of list: " << typeid(list).name() << std::endl;
    std::cout << "Type of single_ele_list: " << typeid(single_ele_list).name() << std::endl;

    std::vector<int> vec = {1, 2, 3, 4, 5};
    for (auto& elem : vec)
    {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    auto f = [](int a, int b) { return a + b; }; // f is of type lambda function
    std::cout << "f(2, 3) = " << f(2, 3) << std::endl;

    return 0;
}
