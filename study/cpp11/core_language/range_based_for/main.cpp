#include <iostream>
#include <vector>

int main()
{
    std::vector<int> vec = {1, 2, 3, 4, 5};

    for (int value : vec)
    {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    for (auto& value : vec)
    {
        value *= 2; // Modify the elements in the vector
    }

    for (int value : vec)
    {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    return 0;
}
