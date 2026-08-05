#include <iostream>
#include <string>
#include <functional>
#include <vector>

template <typename... Ts>
void check_size(Ts... args)
{
    std::cout << "Number of parameters: " << sizeof...(Ts) << std::endl;
    std::cout << "Number of arguments: " << sizeof...(args) << std::endl;
}

template <typename T>
void print(T arg)
{
    std::cout << arg << std::endl;
}

template <typename T, typename... Args>
void print(T first, Args... args)
{
    std::cout << first << std::endl;
    if constexpr (sizeof...(args) > 0)
    {
        print(args...);
    }
}

template <typename... Args>
void forwarding(Args&&... args)
{
    auto f = [](auto&&... inner_args) {
        std::cout << "Number of parameters: " << sizeof...(inner_args) << std::endl;
    };
    f(std::forward<Args>(args)...);
}

template <typename... Args>
void square_all(Args... args)
{
    auto squares = { (args * args)... };
    for (const auto& square : squares)
    {
        std::cout << square << " ";
    }
    std::cout << std::endl;
}

template <typename... Args>
class signal
{
public:
    using slot_type = std::function<void(Args...)>;

    void connect(slot_type slot)
    {
        slots_.push_back(slot);
    }

    void emit(Args... args) const
    {
        for (const auto& slot: slots_)
        {
            if (slot)
            {
                slot(args...);
            }
        }
    }

private:
    std::vector<slot_type> slots_;
};

void global_slot_func(int a, double b, const std::string& c)
{
    std::cout << "Global slot function called with: " << a << ", " << b << ", " << c << std::endl;
}

class handler
{
public:
    void member_slot_func(int a, double b, const std::string& c)
    {
        std::cout << "Member slot function called with: " << a << ", " << b << ", " << c << std::endl;
    }
};

int main()
{
    check_size(1, 2.5, "Hello");

    print(1, 2.5, "Hello", 'A', true);

    forwarding(1, 2.5, "Hello", 'A', true);

    square_all(1, 2, 3, 4, 5);

    signal<int, double, std::string> sig;
    sig.connect(global_slot_func);
    sig.emit(42, 3.14, "Hello Signal");

    handler h;
    sig.connect(std::bind(&handler::member_slot_func, &h, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    sig.emit(7, 2.71, "Hello Member Function");

    return 0;
}
