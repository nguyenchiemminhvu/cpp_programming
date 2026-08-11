#include <iostream>
#include <mutex>
#include <vector>
#include <tuple>
#include <utility>

template <typename T, size_t N>
class ring_buffer
{
public:
    constexpr ring_buffer(const T (&data)[N]) noexcept
    {
        for (size_t i = 0; i < N; ++i)
        {
            buffer[i] = data[i];
        }
    }

private:
    T buffer[N];
    size_t head = 0;
    size_t tail = 0;
};

template <typename T, size_t N>
ring_buffer(const T (&)[N]) -> ring_buffer<T, N>; // Deduction guide

int main()
{
    std::mutex mtx;

    // Pre C++17, you had to specify the type of the vector explicitly
    std::pair<int, std::string> p1(1, "one");
    std::vector<int> v1{1, 2, 3, 4, 5};
    std::tuple<int, double, std::string> t1(1, 2.0, "three");
    {
        std::lock_guard<std::mutex> lock(mtx);
    }

    // C++17 introduced Class Template Argument Deduction (CTAD)
    std::pair p2(2, "two"); // The type of p2 is std::pair<int, std::string>
    std::vector v2{1, 2, 3, 4, 5}; // The type of v2 is std::vector<int>
    std::tuple t2(1, 2.0, "three"); // The type of t2 is std::tuple<int, double, std::string>
    uint8_t buffer[5] = {1, 2, 3, 4, 5};
    ring_buffer rb2(buffer);
    {
        std::lock_guard lock(mtx); // The type of lock is std::lock_guard<std::mutex>
    }

    return 0;
}
