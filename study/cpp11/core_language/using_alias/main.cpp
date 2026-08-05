#include <iostream>
#include <string>
#include <unordered_map>

using hash_table_int = std::unordered_map<int, int>;
using hash_table_str = std::unordered_map<std::string, int>;

template <typename T>
using hash_table = std::unordered_map<T, int>;

struct key_value
{
    int key;
    int value;
};

using pair = key_value;
using config_table = std::unordered_map<std::string, pair>;

int main()
{
    hash_table_int ht_int;
    ht_int[1] = 10;
    std::cout << "ht_int[1]: " << ht_int[1] << std::endl;

    hash_table_str ht_str;
    ht_str["key"] = 20;
    std::cout << "ht_str[\"key\"]: " << ht_str["key"] << std::endl;

    hash_table<double> ht_double;
    ht_double[3.14] = 30;
    std::cout << "ht_double[3.14]: " << ht_double[3.14] << std::endl;

    return 0;
}