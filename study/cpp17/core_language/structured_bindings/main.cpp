#include <iostream>
#include <utility>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <array>
#include <string>

struct point
{
    int x;
    int y;
};

class person
{
public:
    person(std::string name, int age) : name_(name), age_(age)
    {
    }

    std::string get_name() const
    {
        return name_;
    }

    int get_age() const
    {
        return age_;
    }

private:
    std::string name_;
    int age_;
};

struct telematic_data
{
    uint32_t timestamp;
    std::array<int, 3> position;
    uint8_t flags;
};

std::pair<std::string, int> process()
{
    return std::make_pair("Alice", 25);
}

int main()
{
    auto [a, b, c] = std::make_tuple(1, 2, 3);
    std::cout << "a: " << a << ", b: " << b << ", c: " << c << std::endl;

    std::array<int, 3> arr = {4, 5, 6};
    auto [d, e, f] = arr;
    std::cout << "d: " << d << ", e: " << e << ", f: " << f << std::endl;

    point p{7, 8};
    auto [x, y] = p;
    std::cout << "x: " << x << ", y: " << y << std::endl;

    person john("John", 30);
    auto [name, age] = std::pair<std::string, int>(john.get_name(), john.get_age());
    std::cout << "name: " << name << ", age: " << age << std::endl;

    std::unordered_map<std::string, int> map = {{"one", 1}, {"two", 2}, {"three", 3}};
    for (const auto &[key, value] : map)
    {
        std::cout << "key: " << key << ", value: " << value << std::endl;
    }

    auto [name2, age2] = process();
    std::cout << "name2: " << name2 << ", age2: "<< age2 << std::endl;

    telematic_data data{1234567890, {1, 2, 3}, 0b00001111};
    auto [timestamp, position, flags] = data;
    std::cout << "timestamp: " << timestamp << ", position: [" << position[0] << ", " << position[1] << ", " << position[2] << "], flags: " << static_cast<int>(flags) << std::endl;

    return 0;
}
