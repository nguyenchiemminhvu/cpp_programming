#include <iostream>
#include <array>
#include <utility>
#include <functional>
#include <type_traits>

template <typename... Args>
using event_task = std::function<void(typename std::decay<Args>::type... )>;

template <typename... Args>
class event
{
public:
    event(event_task<Args...> task, Args... args)
        : task_(std::move(task)), args_(std::make_tuple(args...))
    {
    }

    ~event() = default;

    void operator()()
    {
        this->invoke(std::make_index_sequence<sizeof...(Args)>());
    }

private:
    template <size_t... Is>
    void invoke(std::index_sequence<Is...>)
    {
        if (task_)
        {
            task_(std::get<Is>(args_)...);
        }
    }

    event_task<Args...> task_;
    std::tuple<Args...> args_;
};

template <typename T, T... Args>
void print_sequence(std::integer_sequence<T, Args...>)
{
    ((std::cout << Args << " "), ...);
    std::cout << std::endl;
}

template <size_t... Is>
void print_idx_sequence(std::index_sequence<Is...>)
{
    ((std::cout << Is << " "), ...);
    std::cout << std::endl;
}

template <typename... T, size_t... Is>
void print_tuple_impl(const std::tuple<T...>& t, std::index_sequence<Is...>)
{
    ((std::cout << std::get<Is>(t) << " "), ...);
    std::cout << std::endl;
}

template <typename... T>
void print_tuple(const std::tuple<T...>& t)
{
    print_tuple_impl(t, std::index_sequence_for<T...>{});
}

template <typename T, std::size_t N, std::size_t... Is>
auto array_to_tuple_impl(const std::array<T, N>& arr, std::index_sequence<Is...>)
{
    return std::make_tuple(arr[Is]...);
}

template <typename T, std::size_t N>
auto array_to_tuple(const std::array<T, N>& arr)
{
    return array_to_tuple_impl(arr, std::make_index_sequence<N>{});
}

int main()
{
    print_sequence(std::make_integer_sequence<int, 5>{}); // Output: 0 1 2 3 4
    print_idx_sequence(std::make_index_sequence<5>{}); // Output: 0 1 2 3 4

    std::array<int, 5> arr = {1, 2, 3, 4, 5};
    auto t = array_to_tuple(arr);
    print_tuple(t); // Output: 1 2 3 4 5

    auto f = [](int a, double b, const std::string& c) {
        std::cout << "a: " << a << ", b: " << b << ", c: " << c << std::endl;
    };

    event<int, double, std::string> e(f, 42, 3.14, "Hello");
    e();

    return 0;
}
