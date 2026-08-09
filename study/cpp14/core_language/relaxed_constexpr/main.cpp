#include <iostream>

constexpr int factorial_cpp11(int n)
{
    return n <= 1 ? 1 : (n * factorial_cpp11(n - 1));
}

constexpr int factorial_cpp14(int n)
{
    // with relaxed constexpr, we can use local variables and loops
    int res = 1;
    for (int i = 2; i <= n; ++i)
    {
        res *= i;
    }
    return res;
}

int main()
{
    std::cout << "Factorial of 5 (C++11): " << factorial_cpp11(5) << std::endl;
    std::cout << "Factorial of 5 (C++14): " << factorial_cpp14(5) << std::endl;

    constexpr int fact5_cpp11 = factorial_cpp11(5);
    constexpr int fact5_cpp14 = factorial_cpp14(5);

    // compile-time assertions to verify correctness
    static_assert(fact5_cpp11 == 120, "Factorial of 5 (C++11) should be 120");
    static_assert(fact5_cpp14 == 120, "Factorial of 5 (C++14) should be 120");

    return 0;
}
