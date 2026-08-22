#include <iostream>
#include <type_traits>

template <typename T>
struct custom_decay
{
    using type = typename custom_decay<typename std::decay<T>::type>::type;
};

template <>
struct custom_decay<char*>
{
    using type = std::string;
};

template <>
struct custom_decay<const char*>
{
    using type = std::string;
};

template <size_t N>
struct custom_decay<char[N]>
{
    using type = std::string;
};

template <size_t N>
struct custom_decay<const char[N]>
{
    using type = std::string;
};

template <typename T>
using custom_decay_t = typename custom_decay<T>::type;

template <typename T>
custom_decay_t<T> build_custom_decay(T&& t)
{
    return custom_decay_t<T>(std::forward<T>(t));
}

template <typename T, typename U>
inline std::pair<typename std::decay<T>::type, typename std::decay<U>::type> build_pair(T&& t, U&& u)
{
    return std::pair<typename std::decay<T>::type, typename std::decay<U>::type>(std::forward<T>(t), std::forward<U>(u));
}

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

template <typename T>
void print_copyable(T&& arg)
{
    std::cout << "Is trivially copyable? " << std::is_trivially_copyable<typename std::decay<T>::type>::value << "\n";
}

struct can_copy
{
    can_copy() = default;
    can_copy(const can_copy&) = default;
    can_copy& operator=(const can_copy&) = default;

    unsigned char bytes[16];
};

struct cannot_copy
{
    cannot_copy() = default;
    cannot_copy(const cannot_copy&) = delete;
    cannot_copy& operator=(const cannot_copy&) = delete;

    std::string s; // because it has dynamic size
    unsigned char* bytes_ptr;
};

template <typename T>
void smart_copy(const T* src, T* dst, std::size_t count)
{
    if constexpr (std::is_trivially_copyable<T>::value)
    {
        std::memcpy(dst, src, count * sizeof(T));
    }
    else
    {
        for (std::size_t i = 0; i < count; ++i)
        {
            new (&dst[i]) T(src[i]); // placement new to copy construct
        }
    }
}

template <bool condition, typename T = void>
struct my_enable_if {};

template <typename T>
struct my_enable_if<true, T> { using type = T; };

template <typename T, typename = typename my_enable_if<std::is_integral<T>::value, T>::type>
void print_integral(T value)
{
    std::cout << "Integral value: " << value << "\n";
}

// this way is better than using std::enable_if in the function signature:
// - it keeps the function signature cleaner
// - it allows for better error messages when the condition is not met
// - it allows overloading
template <typename T>
typename my_enable_if<std::is_floating_point<T>::value, void>::type print_floating_point(T value)
{
    std::cout << "Floating point value: " << value << "\n";
}

int main()
{
    analyzeAndSanitize<const int*>();
    print_decay_type(42);
    print_decay_type(new int(42));
    print_decay_type([]() { std::cout << "Hello, World!\n"; });

    auto p = build_pair(42, new int(42));
    std::cout << "Pair first: " << p.first << ", Pair second: "<< *p.second << "\n";

    auto cd = build_custom_decay("Hello, World!");
    static_assert(std::is_same<decltype(cd), std::string>::value, "Custom decay failed");
    std::cout << "Custom decayed value: " << cd.c_str() << "\n";

    print_copyable(can_copy{});
    print_copyable(cannot_copy{});

    print_integral(42);
    // print_integral(3.14); // This will cause a compile-time error due to SFINAE

    return 0;
}
