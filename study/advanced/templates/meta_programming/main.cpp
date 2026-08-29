#include <iostream>
#include <type_traits>

template <typename Tag, std::size_t I, typename... Rest>
struct index_of_impl;

template <typename Tag, std::size_t I, typename First, typename... Rest>
struct index_of_impl<Tag, I, First, Rest...>
{
    static constexpr std::size_t value = std::is_same<First, Tag>::value ? I : index_of_impl<Tag, I + 1, Rest...>::value;
};

template <typename Tag, std::size_t I, typename... Rest>
struct index_of_impl<Tag, I, Tag, Rest...>
{
    static constexpr std::size_t value = I;
};

template <typename Tag, typename... Types>
static constexpr std::size_t index_of() noexcept
{
    return index_of_impl<Tag, 0, Types...>::value;
}

// ------------------------------

template <typename... Types>
struct no_duplicates;

template <>
struct no_duplicates<> : std::true_type {};

template <typename Head, typename... Tail>
struct no_duplicates<Head, Tail...>
{
    static constexpr bool value = (!std::is_same<Head, Tail>::value && ...) && no_duplicates<Tail...>::value;
};

// -----------------------------

template <typename T, typename... Types>
struct is_one_of
{
    static constexpr bool value = ((std::is_same<T, Types>::value) || ...);
};

int main()
{
    std::cout << index_of<int, double, char, int>() << std::endl;

    std::cout << std::boolalpha << no_duplicates<int, double, char>::value << std::endl;
    std::cout << std::boolalpha << no_duplicates<int, double, int>::value << std::endl;

    std::cout << std::boolalpha << is_one_of<int, double, char, int>::value << std::endl;
    std::cout << std::boolalpha << is_one_of<float, double, char, int>::value << std::endl;

    return 0;
}