#include <iostream>
#include <unordered_set>
#include <unordered_map>

void test_unordered_set()
{
    std::unordered_set<int> uset = {1, 2, 3, 4, 5};

    for (const auto& value : uset)
    {
        std::cout << value << std::endl;
    }
}

void test_unordered_map()
{
    std::unordered_map<int, std::string> umap;
    umap[1] = "one";
    umap[2] = "two";
    umap[3] = "three";

    for (const auto& pair : umap)
    {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
}

void test_unordered_multiset()
{
    std::unordered_multiset<int> umultiset = {1, 2, 2, 3, 3, 3};

    for (const auto& value : umultiset)
    {
        std::cout << value << std::endl;
    }
}

void test_unordered_multimap()
{
    std::unordered_multimap<int, std::string> umultimap;
    umultimap.insert({1, "one"});
    umultimap.insert({2, "two"});
    umultimap.insert({2, "deux"});
    umultimap.insert({3, "three"});

    for (const auto& pair : umultimap)
    {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
}

int main()
{
    test_unordered_set();
    test_unordered_map();
    test_unordered_multiset();
    test_unordered_multimap();

    return 0;
}
