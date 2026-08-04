#include <iostream>

void foo_exception()
{
    throw std::runtime_error("Exception thrown from foo_exception");
}

void foo_noexcept() noexcept
{
    std::cout << "foo_noexcept called" << std::endl;
}

void foo_noexcept_with_throw() noexcept
{
    throw std::runtime_error("Exception thrown from foo_noexcept_with_throw");
}

template <typename T>
void foo_noexcept_with_condition(T val) noexcept(std::is_trivial_copiable<T>::value)
{
    std::cout << "foo_noexcept_with_condition called with value: " << val << std::endl;
}

int main()
{
    try
    {
        foo_exception();
    }
    catch (const std::exception& e)
    {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    try
    {
        foo_noexcept();
    }
    catch (const std::exception& e)
    {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    try
    {
        foo_noexcept_with_throw();
    }
    catch (const std::exception& e)
    {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    return 0;
}
