#include <iostream>
#include "inplace_function.h"
#include "inplace_observer.h"

struct small_context_functor
{
    int count = 0;

    void operator()()
    {
        ++count;
        std::cout << "Counter called: " << count << std::endl;
    }
};

struct large_context_functor
{
    int count = 0;
    std::array<int, 100> data; // Large context

    void operator()()
    {
        ++count;
        std::cout << "Large counter called: " << count << std::endl;
    }
};

void test_inplace_function()
{
    inplace_function<void(int)> func = [](int x) { std::cout << "Lambda called with: " << x << std::endl; };
    func(42);

    inplace_function<void(int)> func2 = func;
    func2(100);

    func = std::move(func2);
    func(200);

    small_context_functor counter;
    inplace_function<void()> func_counter = counter;
    func_counter();

    small_context_functor context;
    inplace_function<small_context_functor()> func3 = [context]() { return context; };
    func3();

    // large_context_functor large_context;
    // inplace_function<large_context_functor()> func4 = [large_context]() { return large_context; };
    // func4();
}

void test_inplace_observer()
{
    inplace_observer<3, void(int)> observer;

    auto callback1 = [](int x) { std::cout << "Callback 1 called with: " << x << std::endl; };
    auto callback2 = [](int x) { std::cout << "Callback 2 called with: " << x << std::endl; };
    auto callback3 = [](int x) { std::cout << "Callback 3 called with: " << x << std::endl; };

    auto inplace_callback1 = inplace_function<void(int)>(callback1);
    auto inplace_callback2 = inplace_function<void(int)>(callback2);
    auto inplace_callback3 = inplace_function<void(int)>(callback3);

    auto result = observer.subscribe(std::move(inplace_callback1));
    if (result != inplace_observer_detail::inplace_observer_error::ok)
    {
        std::cout << "Failed to subscribe callback 1" << std::endl;
    }

    result = observer.subscribe(std::move(inplace_callback2));
    if (result != inplace_observer_detail::inplace_observer_error::ok)
    {
        std::cout << "Failed to subscribe callback 2" << std::endl;
    }

    result = observer.subscribe(std::move(inplace_callback3));
    if (result != inplace_observer_detail::inplace_observer_error::ok)
    {
        std::cout << "Failed to subscribe callback 3" << std::endl;
    }

    observer.notify(42);
    observer.notify(100);

    std::cout << "Observer size: " << observer.size() << std::endl;

    auto callback4 = [](int x) { std::cout << "Callback 4 called with: " << x << std::endl; };
    auto inplace_callback4 = inplace_function<void(int)>(callback4);
    result = observer.subscribe(std::move(inplace_callback4));
    if (result == inplace_observer_detail::inplace_observer_error::listener_full)
    {
        std::cout << "Failed to subscribe callback 4: Observer is full." << std::endl;
    }

    observer.clear();
    std::cout << "Observer size after clear: " << observer.size() << std::endl;

    inplace_observer_detail::subscription sub1;
    auto callback5 = [](int x) { std::cout << "Callback 5 called with: " << x << std::endl; };
    auto inplace_callback5 = inplace_function<void(int)>(callback5);
    result = observer.subscribe(std::move(inplace_callback5), &sub1);
    if (result != inplace_observer_detail::inplace_observer_error::ok)
    {
        std::cout << "Failed to subscribe callback 5 with subscription: " << static_cast<int>(result) << std::endl;
    }

    observer.notify(123);

    result = observer.unsubscribe(sub1);
    if (result != inplace_observer_detail::inplace_observer_error::ok)
    {
        std::cout << "Failed to unsubscribe callback 5" << std::endl;
    }

    std::cout << "Observer size after unsubscribe: " << observer.size() << std::endl;
}

int main()
{
    // test_inplace_function();
    test_inplace_observer();

    return 0;
}