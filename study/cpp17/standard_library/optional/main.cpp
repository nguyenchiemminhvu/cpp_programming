#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <cstdint>

template <typename T>
std::optional<T> get_value_or_default(const std::string& key)
{
    static std::unordered_map<std::string, T> data = {
        {"one", 1},
        {"two", 2},
        {"three", 3}
    };

    if (auto it = data.find(key); it != data.end())
    {
        return it->second;
    }

    return std::nullopt;
}

int main()
{
    auto value1 = get_value_or_default<int>("one");
    if (value1)
    {
        std::cout << "Value for 'one': " << *value1 << std::endl;
    }
    else
    {
        std::cout << "'one' not found." << std::endl;
    }

    auto value2 = get_value_or_default<int>("four");
    if (value2)
    {
        std::cout << "Value for 'four': " << *value2 << std::endl;
    }
    else
    {
        std::cout << "'four' not found." << std::endl;
    }

    return 0;
}
