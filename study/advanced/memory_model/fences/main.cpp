#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

struct location_data
{
    double latitude;
    double longitude;
    double altitude;
    uint32_t timestamp;
};

std::vector<location_data> location_buffer;
std::atomic<bool> data_ready{false};

void sensor_thread()
{
    while (true)
    {
        for (int i = 0; i < 10; ++i)
        {
            location_data data;
            data.latitude = 37.7749 + i * 0.0001;
            data.longitude = -122.4194 + i * 0.0001;
            data.altitude = 10.0 + i;
            data.timestamp = static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count());
            location_buffer.push_back(data);
        }

        // Use a memory fence to ensure that all writes to the location_buffer are visible before setting data_ready to true
        std::atomic_thread_fence(std::memory_order_release);

        data_ready.store(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void processing_thread()
{
    while (true)
    {
        // relaxed load to check if data is ready, but we will use a memory fence to ensure proper ordering
        while (!data_ready.load(std::memory_order_relaxed))
        {
            std::this_thread::yield();
        }

        // Use a memory fence to ensure that all reads from the location_buffer are done after data_ready is observed as true
        std::atomic_thread_fence(std::memory_order_acquire);

        if (!location_buffer.empty())
        {
            std::cout << "Processing " << location_buffer.size() << " location data points" << std::endl;
            location_buffer.clear();
        }
    }
}

int main()
{
    std::thread sensor(sensor_thread);
    std::thread processor(processing_thread);

    sensor.join();
    processor.join();

    return 0;
}