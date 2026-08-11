#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

std::shared_mutex mtx;
std::unordered_map<int, int> data;

void reader(int id)
{
    std::shared_lock<std::shared_mutex> lock(mtx);
    std::cout << "Reader " << id << " reading data: ";
    for (const auto& pair : data)
    {
        std::cout << pair.first << ":" << pair.second << " ";
    }
    std::cout << std::endl;
}

void writer(int id, int key, int value)
{
    std::unique_lock<std::shared_mutex> lock(mtx);
    data[key] = value;
    std::cout << "Writer " << id << " wrote data: " << key << ":" << value << std::endl;
}

int main()
{
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i)
    {
        threads.emplace_back(reader, i);
    }

    for (int i = 0; i < 5; ++i)
    {
        threads.emplace_back(writer, i, i, i * 10);
    }

    for (auto& t : threads)
    {
        t.join();
    }

    return 0;
}
