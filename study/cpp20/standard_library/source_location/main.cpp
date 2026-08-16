#include <iostream>
#include <source_location>
#include <utility>

template <typename... Args>
void print_impl(
    const std::source_location& location,
    Args&&... args)
{
    std::cout
        << "File: " << location.file_name()
        << ", Line: " << location.line()
        << ", Function: " << location.function_name()
        << '\n';

    (std::cout << ... << std::forward<Args>(args)) << '\n';
}

#define PRINT(...) \
    print_impl(std::source_location::current(), __VA_ARGS__)

void foo()
{
    PRINT("Hello from foo!");
}

void bar()
{
    PRINT("Hello from bar!");
}

int main()
{
    foo();
    bar();

    return 0;
}