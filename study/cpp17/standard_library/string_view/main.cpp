#include <string>
#include <iostream>
#include <string_view>
#include <cstdint>

int main()
{
    std::string s = "hello world";
    std::string_view sv1 = s; // implicit conversion from std::string to std::string_view
    std::string_view sv2(s.c_str()); // implicit conversion from const char* to std::string_view
    std::string_view sv3(s.data(), s.size()); // implicit conversion from const char* and size to std::string_view

    std::cout << "sv1: " << sv1 << std::endl;
    std::cout << "sv2: " << sv2 << std::endl;
    std::cout << "sv3: " << sv3 << std::endl;

    sv1.remove_prefix(6); // remove the first 6 characters
    std::cout << "sv1 after remove_prefix: " << sv1 << std::endl;

    std::cout << "original string: " << s << std::endl; // original string is unchanged

    return 0;
}
