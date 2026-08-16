#include <iostream>
#include <vector>
#include <array>
#include <span>

void process_buffer(std::span<const int> buffer)
{
    for (const auto& item : buffer)
    {
        std::cout << item << " ";
    }

    std::cout << std::endl;

    auto first_half = buffer.first(buffer.size() / 2);
    auto second_half = buffer.subspan(buffer.size() / 2);

    for (const auto& item : first_half)
    {
        std::cout << item << " ";
    }

    std::cout << std::endl;

    for (const auto& item : second_half)
    {
        std::cout << item << " ";
    }

    std::cout << std::endl;
}

int main()
{
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::array<int, 5> arr = {6, 7, 8, 9, 10};

    std::cout << "Processing vector: ";
    process_buffer(vec);
    process_buffer(arr);

    return 0;
}
