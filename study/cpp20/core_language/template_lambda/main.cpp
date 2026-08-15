#include <iostream>
#include <type_traits>
#include <array>

int main()
{
    auto f_add = []<typename T> (T a, T b) {
        T result = a + b; // can declare a variable of type T inside lambda
        return result;
    };

    auto f_add_all = []<typename... Args> (Args... args) {
        return (args + ...);
    };

    auto f_ring_buf = []<typename T, std::size_t N> (std::array<T, N> arr)
                        requires (N > 0)
    {
        return arr.data();
    };

    std::cout << f_add(1, 2) << std::endl; // prints 3
    std::cout << f_add_all(1, 2, 3, 4, 5) << std::endl; // prints 15

    std::array<int, 5> arr{};
    auto *p_buf = f_ring_buf(arr);
    for (std::size_t i = 0; i < 5; ++i)
    {
        p_buf[i] = static_cast<int>(i);
    }

    return 0;
}
