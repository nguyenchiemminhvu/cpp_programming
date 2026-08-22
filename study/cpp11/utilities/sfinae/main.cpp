#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type process_can_signal(T value)
{
    std::cout << "Processing integral value: " << value << std::endl;
}

template <typename T>
typename std::enable_if<!std::is_integral<T>::value, void>::type process_can_signal(T value)
{
    std::cout << "Processing non-integral value: " << value << std::endl;
}

template <typename T, typename = void>
struct has_size_method : std::false_type {};

template <typename T>
struct has_size_method<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};

struct sample_struct
{
    int value;
};

template <typename T>
typename std::enable_if<std::is_same<T, sample_struct>::value, sample_struct>::type build_sample_struct(int val)
{
    sample_struct s;
    s.value = val;
    std::cout << "Building sample_struct with value: " << s.value << std::endl;
    return s;
}

template <typename T>
typename std::enable_if<has_size_method<T>::value, void>::type get_size(T&& obj)
{
    std::cout << "Size: " << obj.size() << std::endl;
}

int main()
{
    process_can_signal(42);          // Calls the integral version
    process_can_signal(3.14);        // Calls the non-integral version
    sample_struct s = build_sample_struct<sample_struct>(42); // Calls the build_sample_struct function

    std::vector<int> vec = {1, 2, 3, 4, 5};
    get_size(vec);                   // Calls the get_size function for a type with size() method

    // get_size(s);                     // This will not compile since sample_struct does not have a size() method

    return 0;
}