#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool ready = true;

void producer_thread_func()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return ready; });
        std::cout << "Producer thread is producing data..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate some work
        ready = false;
        cv.notify_one();
    }
}

void consumer_thread_func()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !ready; });
        std::cout << "Consumer thread is consuming data..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate some work
        ready = true;
        cv.notify_one();
    }
}

int main()
{
    std::thread producer(producer_thread_func);
    std::thread consumer(consumer_thread_func);

    if (producer.joinable())
        producer.join();
    
    if (consumer.joinable())
        consumer.join();

    return 0;
}