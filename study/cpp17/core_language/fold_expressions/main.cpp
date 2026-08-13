#include <iostream>
#include <type_traits>
#include <utility>

template <typename... Args>
auto left_fold_minus(Args... args)
{
    return (... - args);
}

template <typename... Args>
auto right_fold_minus(Args... args)
{
    return (args - ...);
}

void test_left_fold_minus()
{
    std::cout << "Left fold minus: " << left_fold_minus(10, 5, 2) << std::endl;
}

void test_right_fold_minus()
{
    std::cout << "Right fold minus: " << right_fold_minus(10, 5, 2) << std::endl;
}

template <typename... Args>
bool any_integer(Args... args)
{
    return (std::is_integral_v<Args> || ...);
}

template <typename... Args>
bool all_integer(Args... args)
{
    return (std::is_integral_v<Args> && ...);
}

class logger
{
public:
    template <typename... Args>
    void cpp14_log(Args... args)
    {
        cpp14_log_impl(std::forward<Args>(args)...);
    }

    template <typename... Args>
    void cpp17_log(Args... args)
    {
        ((std::cout << args << " "), ...) << std::endl;
    }

    template <typename... Args>
    void cpp17_log2(Args... args)
    {
        (std::cout << ... << std::forward<Args>(args)) << std::endl;
    }

private:
    template <typename T, typename... Args>
    void cpp14_log_impl(T&& first, Args&&... args)
    {
        std::cout << std::forward<T>(first);
        if constexpr (sizeof...(args) > 0)
        {
            std::cout << " ";
            cpp14_log_impl(std::forward<Args>(args)...);
        }
    }

    template <typename T>
    void cpp14_log_impl(T&& first)
    {
        std::cout << std::forward<T>(first) << std::endl;
    }
};

template <typename Func, typename... Args>
void for_each_arg(Func&& func, Args&&... args)
{
    (static_cast<void>(func(std::forward<Args>(args))), ...);
}

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename T>
struct is_std_tuple : std::false_type {};

template <typename... Ts>
struct is_std_tuple<std::tuple<Ts...>> : std::true_type {};

template <typename T>
inline constexpr bool is_std_tuple_v =
    is_std_tuple<remove_cvref_t<T>>::value;

template <typename Func, typename... Args>
void for_each_tuple_arg(Func&& func, Args&&... args)
{
    static_assert(
        (is_std_tuple_v<Args> && ...),
        "Every argument after func must be a std::tuple"
    );

    (
        [&func](auto&& tuple)
        {
            std::apply(
                [&func](auto&&... tuple_args)
                {
                    (static_cast<void>(func(std::forward<decltype(tuple_args)>(tuple_args))), ...);
                },
                std::forward<decltype(tuple)>(tuple)
            );
        }(std::forward<Args>(args)),
        ...
    );
}

int main()
{
    test_left_fold_minus();
    test_right_fold_minus();

    std::cout << "Any integer: " << any_integer(1, 2, 3.5) << std::endl;
    std::cout << "All integer: " << all_integer(1, 2, 3) << std::endl;

    logger log;
    log.cpp14_log("C++14 log:", 1, 2, 3);
    log.cpp17_log("C++17 log:", 1, 2, 3);
    log.cpp17_log2("C++17 log2: ", 1, " ", 2, " ", 3);

    for_each_arg([](auto&& arg) { std::cout << arg << " "; }, 1, 2, 3, 4, 5);
    std::cout << std::endl;

    for_each_tuple_arg(
        [](auto&& arg) { std::cout << arg << " "; },
        std::make_tuple(1, 2, 3),
        std::make_tuple(4, 5, 6)
    );
    std::cout << std::endl;

    return 0;
}
