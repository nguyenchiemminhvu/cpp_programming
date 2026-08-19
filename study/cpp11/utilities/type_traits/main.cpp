#include <iostream>
#include <type_traits>

template <typename T>
void analyzeAndSanitize()
{
    std::cout << "Is pointer? " << std::is_pointer<T>::value << "\n";
    std::cout << "Is const? "   << std::is_const<T>::value << "\n";
    using StrippedType = typename std::remove_pointer<T>::type;
    std::cout << "Is stripped type const? " << std::is_const<StrippedType>::value << "\n";
}

template <typename T>
void print_decay_type(T&& arg)
{
    using decay_type = typename std::decay<T>::type;
    if (std::is_same<decay_type, int>::value)
    {
        std::cout << "The decayed type is int.\n";
    }
    else if (std::is_same<decay_type, int*>::value)
    {
        std::cout << "The decayed type is int*.\n";
    }
    else if (std::is_same<decay_type, void(*)()>::value)
    {
        std::cout << "The decayed type is void(*)().\n";
    }
    else
    {
        std::cout << "The decayed type is unknown.\n";
    }
}

template <typename T, typename U>
inline std::pair<typename std::decay<T>::type, typename std::decay<U>::type> build_pair(T&& t, U&& u)
{
    return std::pair<typename std::decay<T>::type, typename std::decay<U>::type>(std::forward<T>(t), std::forward<U>(u));
}

int main()
{
    analyzeAndSanitize<const int*>();
    print_decay_type(42);
    print_decay_type(new int(42));
    print_decay_type([]() { std::cout << "Hello, World!\n"; });

    auto p = build_pair(42, new int(42));
    std::cout << "Pair first: " << p.first << ", Pair second: "<< *p.second << "\n";
    return 0;
}
