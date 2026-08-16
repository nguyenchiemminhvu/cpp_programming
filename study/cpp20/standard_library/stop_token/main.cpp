#include <iostream>
#include <thread>
#include <chrono>
#include <stop_token>

int main()
{
    std::stop_source stop_signal;

    std::jthread worker{[token = stop_signal.get_token()]() {
        while (!token.stop_requested()) {
            std::cout << "Working..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        std::cout << "Stop requested. Exiting thread." << std::endl;
    }};

    std::this_thread::sleep_for(std::chrono::seconds(2));
    stop_signal.request_stop();

    return 0;
}