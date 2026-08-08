#include <iostream>
#include <type_traits>

template <typename T>
void analyzeAndSanitize()
{
    std::cout << "Is pointer? " << std::is_pointer<T>::value << "\n";
    std::cout << "Is const? "   << std::is_const<T>::value << "\n";
    using StrippedType = typename std::remove_pointer<T>::type;
    std::cout << "Is stripped type const? " << std::is_const<StrippedType>::value << "\n";
}

int main()
{
    analyzeAndSanitize<const int*>();
    return 0;
}
