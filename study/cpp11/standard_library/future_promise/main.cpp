#include <iostream>
#include <future>
#include <thread>
#include <chrono>

void fetch_data(std::promise<int> p)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    p.set_value(42);
}

int main()
{
    std::promise<int> p;
    std::future<int> f = p.get_future();
    std::thread t(fetch_data, std::move(p));
    t.join();
    std::cout << "Fetched data: " << f.get() << std::endl;

    f = std::async(std::launch::async, []() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return 24;
    });
    std::cout << "Fetched data from async: " << f.get() << std::endl;

    return 0;
}