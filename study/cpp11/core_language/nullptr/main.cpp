#include <iostream>

void foo(int x)
{
    std::cout << "foo(int): " << x << std::endl;
}

void foo(char* x)
{
    std::cout << "foo(char*): " << x << std::endl;
}

int main()
{
    foo(NULL); // Calls foo(int) because NULL is typically defined as 0
    foo(nullptr); // Calls foo(char*) because nullptr is a null pointer constant
    return 0;
}
