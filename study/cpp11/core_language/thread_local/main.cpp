#include <iostream>
#include <vector>
#include <thread>

thread_local int local_counter = 0;

void increment_counter()
{
    local_counter++;
    std::cout << "Thread ID: " << std::this_thread::get_id() << ", Local Counter: " << local_counter << std::endl;
}

int main()
{
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i)
    {
        threads.emplace_back(increment_counter);
    }

    for (auto& t : threads)
    {
        t.join();
    }

    std::cout << "Main Thread ID: " << std::this_thread::get_id() << ", Local Counter: " << local_counter << std::endl;

    return 0;
}