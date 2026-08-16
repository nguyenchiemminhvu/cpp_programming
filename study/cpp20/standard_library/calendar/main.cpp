#include <chrono>
#include <iostream>

int main()
{
    using namespace std::chrono;

    const year_month_day date = 2026y / August / 16;

    std::cout << "Year : " << int(date.year()) << '\n';
    std::cout << "Month: " << unsigned(date.month()) << '\n';
    std::cout << "Day  : " << unsigned(date.day()) << '\n';

    std::cout << "Valid: "
              << std::boolalpha
              << date.ok()
              << '\n';

    return 0;
}