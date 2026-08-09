#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

int main()
{
    auto ptr = std::make_unique<int>(42);
    auto f = [p = std::move(ptr)]() {
        std::cout << *p << std::endl;
    };
    f();

    if (ptr)
    {
        std::cout << "ptr is not null" << std::endl;
    }
    else
    {
        std::cout << "ptr is null" << std::endl;
    }

    int factor = 2;
    auto f_double = [mul = factor](int x) {
        return x * mul;
    };

    std::vector<int> numbers = {1, 2, 3, 4, 5};
    std::transform(numbers.begin(), numbers.end(), numbers.begin(), f_double);
    for (const auto& num : numbers)
    {
        std::cout << num << " ";
    }

    return 0;
}