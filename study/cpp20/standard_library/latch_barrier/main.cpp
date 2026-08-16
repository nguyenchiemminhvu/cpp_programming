#include <iostream>
#include <thread>
#include <latch>
#include <barrier>
#include <vector>

void test_latch()
{
    std::latch startup_latch(3);

    std::vector<std::jthread> threads;

    for (int i = 0; i < 3; ++i)
    {
        threads.emplace_back([&startup_latch, i]() {
            std::cout << "Thread " << i << " is doing some work...\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "Thread " << i << " has finished its work.\n";
            startup_latch.count_down();
        });
    }
    startup_latch.wait();

    std::cout << "All threads have finished their work. Proceeding...\n";
}

void test_barrier()
{
    std::barrier sync_barrier(3, []() {
        std::cout << "All threads have reached the barrier. Proceeding...\n";
    });

    while (true)
    {
        std::vector<std::jthread> threads;
        for (int i = 0; i < 3; ++i)
        {
            threads.emplace_back([&sync_barrier, i]() {
                std::cout << "Thread " << i << " is doing some work...\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "Thread " << i << " has reached the barrier.\n";
                sync_barrier.arrive_and_wait();
            });
        }
        threads.clear(); // Clear the threads to allow them to finish before the next iteration
        std::cout << "All threads have completed their work and passed the barrier. Starting next iteration...\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    test_latch();
    test_barrier();

    return 0;
}