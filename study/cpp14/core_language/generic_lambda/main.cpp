#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <utility>

enum
{
    EVENT_NONE = 0,
    EVENT_READ = 1 << 0,
    EVENT_WRITE = 1 << 1,
    EVENT_ERROR = 1 << 2
};

struct event
{
    uint32_t id;
    uint32_t mask;
};

class event_logger
{
public:
    void log(const event& e)
    {
        std::cout << "log(const event& e) called" << std::endl;
        std::cout << "Event ID: " << e.id << ", Mask: " << e.mask << std::endl;
    }

    void log(event&& e)
    {
        std::cout << "log(const event&& e) called" << std::endl;
        std::cout << "Event ID: " << e.id << ", Mask: " << e.mask << std::endl;
    }
};

template <typename log_callback_t>
class event_manager
{
public:
    event_manager(log_callback_t cb) : callback(std::move(cb)) {}

    template <typename event>
    void dispatch_event(event&& e)
    {
        callback(std::forward<event>(e));
    }

private:
    log_callback_t callback;
};

int main()
{
    std::vector<int> numbers = { 3, 2, 6, 7, 9, 1}; // random numbers
    std::sort(numbers.begin(), numbers.end(), [](auto a, auto b) { return a < b; }); // sort in ascending order using generic lambda
    for (const auto& num : numbers)
    {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    event_logger logger;
    auto dispatcher = [&logger](auto&& e) noexcept
    {
        logger.log(std::forward<decltype(e)>(e));
    };

    event_manager<decltype(dispatcher)> manager(dispatcher);

    event e1{ 1, EVENT_READ | EVENT_WRITE };
    manager.dispatch_event(e1);
    manager.dispatch_event(std::move(e1));
    manager.dispatch_event(std::remove_reference_t<event>{ 2, EVENT_ERROR });

    return 0;
}
