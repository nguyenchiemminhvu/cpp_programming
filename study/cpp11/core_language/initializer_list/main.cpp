#include <iostream>
#include <vector>
#include <initializer_list>
#include <map>
#include <string>

class sample_array
{
public:
    sample_array(std::initializer_list<int> list)
    {
        for (const auto& element : list)
        {
            data.push_back(element);
        }
    }

    void print()
    {
        std::cout << "Elements in the sample_array: ";
        for (const auto& element : data)
        {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }

private:
    std::vector<int> data;
};

struct key_value
{
    int key;
    std::string value;
};

class config
{
public:
    config(std::initializer_list<key_value> list)
    {
        for (const auto& kv : list)
        {
            data[kv.key] = kv.value;
        }
    }

    void print()
    {
        std::cout << "Key-Value pairs in the config: " << std::endl;
        for (const auto& kv : data)
        {
            std::cout << "Key: " << kv.first << ", Value: " << kv.second << std::endl;
        }
    }

private:
    std::map<int, std::string> data;
};

int main()
{
    std::initializer_list<int> myList = {1, 2, 3, 4, 5};
    std::cout << "Elements in the initializer list: ";
    for (const auto& element : myList)
    {
        std::cout << element << " ";
    }
    std::cout << std::endl;

    sample_array arr = {10, 20, 30, 40, 50};
    arr.print();

    std::initializer_list<int> anotherList = {100, 200, 300};
    std::cout << "Elements in another initializer list: ";
    for (auto it = anotherList.begin(); it != anotherList.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::initializer_list<key_value> keyValueList = {
        { 1, "Hello" },
        { 2, "World" },
        { 3, "!" }
    };

    config cfg = keyValueList;
    cfg.print();

    return 0;
}
