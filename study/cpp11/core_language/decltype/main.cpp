#include <iostream>

template <typename T1, typename T2>
auto add(T1 a, T2 b) -> decltype(std::forward<T1>(a) + std::forward<T2>(b))
{
    return a + b; // return type is deduced to be the type of a + b
}

class HasFooMethod
{
public:
    int foo() { return 42; }
};

class NoFooMethod
{
public:
    int bar() { return 0; }
};

template <typename T>
auto check_foo(T& obj) -> decltype(obj.foo(), void())
{
    std::cout << "Has foo method" << std::endl;
}

int main()
{
    int val = 0;
    decltype(val) val2 = 1; // val2 is of type int
    decltype((val)) val3 = val; // val3 is of type int&
    val3 = 2; // val is now 2
    std::cout << "val: " << val << std::endl; // prints "val: 2"

    decltype(val3) val4 = val; // val4 is of type int&
    val4 = 3; // val is now 3
    std::cout << "val: " << val << std::endl; // prints "val: 3"

    auto f = [](float a, float b) -> decltype(a + b) {
        return a + b; // return type is deduced to be float
    };

    std::cout << "f(1.0f, 2.0f): " << f(1.0f, 2.0f) << std::endl; // prints "f(1.0f, 2.0f): 3"
    std::cout << "add(1, 2.5): " << add(1, 2.5) << std::endl; // prints "add(1, 2.5): 3.5"

    bool is_float = std::is_same_v<decltype(f(1.0f, 2.0f)), float>; // isFloat is true
    std::cout << "is_float: " << is_float << std::endl; // prints "is_float: 1"

    HasFooMethod obj1;
    NoFooMethod obj2;
    check_foo(obj1); // prints "Has foo method"
    // check_foo(obj2); // Uncommenting this line will cause a compilation error because NoFooMethod does not have a foo() method

    return 0;
}
