#include <iostream>
#include <string>

int val = 0;

int f()
{
    return ++val;
}

int add(int a, int b)
{
    return a + b;
}

int main()
{
    std::cout << add(f(), f()) << std::endl;
    std::cout << f() << " " << f() << std::endl;

    return 0;
}