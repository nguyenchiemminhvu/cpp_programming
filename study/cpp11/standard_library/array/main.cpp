#include <iostream>
#include <array>

int main()
{
    std::array<int, 5> arr = {1, 2, 3, 4, 5};
    for (const auto& elem : arr)
    {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    std::cout << "Size of array: " << arr.size() << std::endl;
    std::cout << "First element: " << arr.front() << std::endl;
    std::cout << "Last element: " << arr.back() << std::endl;

    arr.fill(0);
    std::cout << "Array after fill: ";
    for (const auto& elem : arr)
    {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    arr.fill(1);
    std::cout << "Array after fill with 1: ";
    for (const auto& elem : arr)
    {
        std::cout << elem << " ";
    }
    std::cout << std::endl;

    std::array<int, 5> arr2 = {6, 7, 8, 9, 10};
    arr.swap(arr2);
    std::cout << "Array after swap: ";
    for (const auto& elem : arr)
    {
        std::cout << elem << " ";
    }

    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    return 0;
}
