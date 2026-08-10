#include <iostream>

template <typename T>
auto add(T a, T b)
{
    return a + b;
}

// compiler error: return type deduction failed because the return types are different
// auto multi_return_multi_type(int val)
// {
//     if (val < 0)
//     {
//         return float(val); // returns float
//     }
//     if (val > 0)
//     {
//         return double(val); // returns double
//     }
//     return 0; // returns int
// }

// return reference
template <typename T>
decltype(auto) get_reference(T& value)
{
    return (value);
}

int main()
{
    int x = 5;
    int y = 10;
    std::cout << "Sum: " << add(x, y) << std::endl;

    int val = 42;
    int& ref = get_reference(val);

    ref = 100; // Modify the original value through the reference
    std::cout << "Modified value: " << val << std::endl;
    return 0;
}
