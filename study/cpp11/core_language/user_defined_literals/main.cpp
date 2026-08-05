#include <iostream>
#include <string>
#include <string_view>

// define distant unit literal, based on meters
constexpr long double operator"" _m(long double x) { return x; }
constexpr long double operator"" _mm(long double x) { return x / 1000; }
constexpr long double operator"" _km(long double x) { return x * 1000; }
constexpr long double operator"" _mile(long double x) { return x * 1609.34; }

// define time unit literal, based on seconds
constexpr int operator"" _s(unsigned long long x) { return x; }
constexpr int operator"" _min(unsigned long long x) { return x * 60; }
constexpr int operator"" _h(unsigned long long x) { return x * 3600; }
constexpr int operator"" _day(unsigned long long x) { return x * 86400; }
constexpr int operator"" _week(unsigned long long x) { return x * 604800; }

// define speed unit literal, based on meters per second
constexpr long double operator"" _mps(long double x) { return x; }
constexpr long double operator"" _kmph(long double x) { return x / 3.6; }

// define temperature unit literal, based on Celsius
constexpr long double operator"" _C(long double x) { return x; }
constexpr long double operator"" _F(long double x) { return (x - 32) * 5.0 / 9.0; }
constexpr long double operator"" _K(long double x) { return x - 273.15; }

// define weight unit literal, based on grams
constexpr long double operator"" _g(long double x) { return x; }
constexpr long double operator"" _kg(long double x) { return x * 1000; }
constexpr long double operator"" _lb(long double x) { return x * 453.592; }

// define volume unit literal, based on liters
constexpr long double operator"" _L(long double x) { return x; }
constexpr long double operator"" _mL(long double x) { return x / 1000; }
constexpr long double operator"" _gal(long double x) { return x * 3.78541; }

// define area unit literal, based on square meters
constexpr long double operator"" _m2(long double x) { return x; }
constexpr long double operator"" _km2(long double x) { return x * 1000000; }
constexpr long double operator"" _acre(long double x) { return x * 4046.86; }

// define angle unit literal, based on degrees
constexpr long double operator"" _deg(long double x) { return x; }
constexpr long double operator"" _rad(long double x) { return x * 180.0 / 3.14159265358979323846; }

// define string literal, based on std::string
std::string operator"" _s(const char* str, size_t) { return std::string(str); }

int main()
{
    long double distance = 5.0_km + 300.0_m;
    std::cout << "Distance: " << distance << " meters" << std::endl;

    long double angle = 90.0_deg;
    std::cout << "Angle: " << angle << " degrees" << std::endl;

    long double radian = 3.14159265358979323846_rad;
    std::cout << "Radian: " << radian << " radians" << std::endl;

    long double speed = 60.0_kmph;
    std::cout << "Speed: " << speed << " meters per second" << std::endl;

    long double temperature = 100.0_C;
    std::cout << "Temperature: " << temperature << " degrees Celsius" << std::endl;

    long double weight = 70.0_kg;
    std::cout << "Weight: " << weight << " grams" << std::endl;

    long double volume = 2.0_L;
    std::cout << "Volume: " << volume << " liters" << std::endl;

    std::string str = "Hello, World!"_s;
    std::cout << "String: " << str << std::endl;

    return 0;
}