#include <iostream>
#include <atomic>
#include <thread>

struct vehicle_state
{
    int speed;
    int rpm;
};

std::atomic<int> processed_frames{0};

vehicle_state state{};
std::atomic<bool> data_ready{false};

void publish_state()
{
    state.speed = 100;
    state.rpm = 3000;

    // what happens behind the order release:
    // 1. The compiler ensures that all writes to shared variables (state.speed and state.rpm) are completed before the store to data_ready.
    // 2. The CPU ensures that all writes to shared variables are visible to other threads before the store to data_ready.
    // This means that when another thread reads data_ready and sees it as true,
    // it is guaranteed to see the updated values of state.speed and state.rpm as well.
    data_ready.store(true, std::memory_order_release);

    // what happens behind the order relaxed:
    // 1. The compiler does not enforce any ordering constraints on the writes to shared variables
    // 2. The CPU does not enforce any ordering constraints on the writes to shared variables
    processed_frames.fetch_add(1, std::memory_order_relaxed);
}

void consume_state()
{
    // what happens behind the order acquire:
    // 1. The compiler ensures that all reads of shared variables (state.speed and state.rpm) are completed after the load of data_ready.
    // 2. The CPU ensures that all reads of shared variables are visible to this thread after the load of data_ready.
    while (!data_ready.load(std::memory_order_acquire))
    {
        std::this_thread::yield(); // Yield to other threads while waiting
    }

    std::cout << "Speed: " << state.speed << ", RPM: " << state.rpm << std::endl;
}

int main()
{
    std::thread producer(publish_state);
    std::thread consumer(consume_state);

    producer.join();
    consumer.join();

    return 0;
}