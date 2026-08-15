#include <iostream>
#include <type_traits>
#include <vector>

template <typename T>
constexpr bool has_member_function_size = requires(T&& t)
{
    t.size();
    t.empty();
};

int return_int()
{
    return 42;
}

constexpr bool must_return_integral = requires
{
    std::is_integral_v<decltype(return_int())>;
};

struct sample_type
{
    using int_type = int;
};

template <typename T>
constexpr bool is_type_available = requires
{
    typename T::int_type;
};

int main()
{
    std::cout << std::boolalpha;
    std::cout << "std::vector<int> has member function size: " << has_member_function_size<std::vector<int>> << std::endl;
    std::cout << "int has member function size: " << has_member_function_size<int> << std::endl;
    std::cout << "return_int() returns integral type: " << must_return_integral << std::endl;
    std::cout << "sample_type has nested type 'int_type': " << is_type_available<sample_type> << std::endl;

    return 0;
}