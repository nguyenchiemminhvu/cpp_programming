#include <iostream>
#include <functional>
#include <cstdint>
#include <vector>

using func_int_param_t = void(int);
using fptr_int_param_t = void(*)(int);

void foo(int status)
{
    std::cout << "foo called with status: " << status << std::endl;
}

void examine_simple_function()
{
    std::function<func_int_param_t> func = foo;
    func(42);

    void(*fptr)(int) = foo;
    func = fptr;
    func(24);

    auto f = [](int status) { std::cout << "lambda called with status: " << status << std::endl; };
    func = f;
    func(12);
}

struct can_frame
{
    uint32_t can_id;
    uint8_t payload[8];
};

using can_handler_t = std::function<void(const can_frame&)>;

class can_router
{
public:
    void subscribe(can_handler_t handler)
    {
        subscribers_.push_back(handler);
    }

    void dispatch(const can_frame& frame) const
    {
        for (const auto& handler : subscribers_)
        {
            handler(frame);
        }
    }

private:
    std::vector<can_handler_t> subscribers_;
};

class can_diagnostic
{
public:
    void diagnose(const char* interface_name, const can_frame& frame)
    {
        std::cout << "Diagnosing frame on interface " << interface_name << ": can_id = " << frame.can_id << std::endl;
    }
};

int main()
{
    examine_simple_function();

    can_router router;
    can_diagnostic diagnostic;

    router.subscribe([&diagnostic](const can_frame& frame) {
        diagnostic.diagnose("can0", frame);
    });

    router.subscribe(std::bind(&can_diagnostic::diagnose, &diagnostic, "can1", std::placeholders::_1));

    can_frame frame{1, {0, 1, 2, 3, 4, 5, 6, 7}};
    router.dispatch(frame);

    return 0;
}
