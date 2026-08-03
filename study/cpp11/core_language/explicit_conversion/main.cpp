#include <iostream>

// without explicit conversion
class unsafe_number
{
public:
    unsafe_number(int v) : value(v) {}
    // operator int() const { return value; } // assume forget imlpement int() conversion
    operator bool() const { return value != 0; }

private:
    int value;
};

class safe_number
{
public:
    explicit safe_number(int v) : value(v) {}

    // explicit operator int() const { return value; } // explicit conversion to int
    explicit operator bool() const { return value != 0; } // explicit conversion to bool

private:
    int value;
};

class explicit_constructor
{
public:
    explicit explicit_constructor(int v) : value(v) {}
    int get_value() const { return value; }
};

int main()
{
    unsafe_number n(42);
    int i = n; // implicit conversion to int
    std::cout << "i: " << i << std::endl;
    if (n) // implicit conversion to bool
    {
        std::cout << "n is non-zero" << std::endl;
    }

    safe_number m(42);
    int j = m; // error: no implicit conversion to int
    if (static_cast<bool>(m)) // explicit conversion to bool
    {
        std::cout << "m is non-zero" << std::endl;
    }

    safe_number k(0.0);

    return 0;
}