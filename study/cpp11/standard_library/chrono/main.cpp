#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstring>

void test_clocks()
{
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::time_t now_t = std::chrono::system_clock::to_time_t(now);

    // local time
    std::cout << std::ctime(&now_t) << std::endl;

    // utc time
    std::cout << std::asctime(std::gmtime(&now_t)) << std::endl;

    char buffer[64];
    std::memset(buffer, 0, sizeof(buffer));
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_t));
    std::cout << buffer << std::endl;

    // 2 hours later
    std::chrono::hours two_hours(2);
    now += two_hours;

    // local time
    now_t = std::chrono::system_clock::to_time_t(now);
    std::cout << std::ctime(&now_t) << std::endl;

    // utc time
    std::cout << std::asctime(std::gmtime(&now_t)) << std::endl;

    std::memset(buffer, 0, sizeof(buffer));
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_t));
    std::cout << buffer << std::endl;

    using namespace std::chrono_literals;
    now -= 2h; // 2 hours earlier
    // local time
    now_t = std::chrono::system_clock::to_time_t(now);
    std::cout << std::ctime(&now_t) << std::endl;

    // utc time
    std::cout << std::asctime(std::gmtime(&now_t)) << std::endl;

    std::memset(buffer, 0, sizeof(buffer));
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_t));
    std::cout << buffer << std::endl;
}

void test_durations()
{
    std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " s" << std::endl;

    start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Elapsed time: " << elapsed_milliseconds.count() << " ms" << std::endl;

    start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::microseconds(200));
    end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::micro> elapsed_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Elapsed time: " << elapsed_microseconds.count() << " µs" << std::endl;

    start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::nanoseconds(100));
    end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::nano> elapsed_nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::cout << "Elapsed time: " << elapsed_nanoseconds.count() << " ns" << std::endl;
}

int main()
{
    test_clocks();
    test_durations();

    return 0;
}
