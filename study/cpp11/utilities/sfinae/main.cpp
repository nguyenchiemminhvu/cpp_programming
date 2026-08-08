#include <iostream>
#include <type_traits>
#include <utility>

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

int main()
{
    process_can_signal(42);          // Calls the integral version
    process_can_signal(3.14);        // Calls the non-integral version
    sample_struct s = build_sample_struct<sample_struct>(42); // Calls the build_sample_struct function

    return 0;
}