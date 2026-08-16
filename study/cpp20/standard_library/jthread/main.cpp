#include <iostream>
#include <thread>
#include <chrono>

void worker_thread(std::stop_token stoken)
{
    while (!stoken.stop_requested())
    {
        std::cout << "Worker thread is running..." << std::endl;

        for (int i = 0; i < 5; ++i)
        {
            if (stoken.stop_requested())
            {
                std::cout << "Worker thread received stop request." << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    std::cout << "Worker thread is stopping..." << std::endl;
}

int main()
{
    {
        std::jthread jt(worker_thread);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Main thread is exiting..." << std::endl;

    return 0;
}
