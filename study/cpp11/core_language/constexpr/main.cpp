#include <iostream>

constexpr int square(int n) { return n * n; }

int main()
{
    // compile-time constant expression
    constexpr int x = 5;
    constexpr int y = x * 2;
    constexpr int z = y + 3;
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y << std::endl;
    std::cout << "z: " << z << std::endl;

    // compile-time constant expression with function
    constexpr int a = square(4);
    std::cout << "a: " << a << std::endl;
    std::cout << square(x + y + z) << std::endl;

    int temp = 10;
    // runtime constant expression
    int b = square(temp);
    std::cout << "b: " << b << std::endl;

    return 0;
}
