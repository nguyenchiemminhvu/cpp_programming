#include <iostream>
#include <type_traits>
#include <vector>
#include <ranges>

int main()
{
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto res = vec | std::views::transform([](int val) { return val * 2; });

    for (auto val : res)
    {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    return 0;
}
