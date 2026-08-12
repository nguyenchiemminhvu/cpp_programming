#include <iostream>
#include <mutex>

std::mutex mtx;

auto get_status()
{
    std::lock_guard<std::mutex> lock(mtx);
    return 1;
}

int main()
{
    if (auto status = get_status(); status == 1)
    {
        std::cout << "Status is 1" << std::endl;
    }
    else
    {
        std::cout << "Status is not 1" << std::endl;
    }

    int status = get_status();
    if (std::lock_guard<std::mutex> lock(mtx); status == 1)
    {
        std::cout << "Status is 1" << std::endl;
    }
    else
    {
        std::cout << "Status is not 1" << std::endl;
    }

    return 0;
}
