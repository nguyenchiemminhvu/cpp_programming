#pragma once

#include <type_traits>
#include <cstdint>
#include <climits>

namespace castle
{
namespace bit
{

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, bool>::type
is_even(T v) noexcept
{
    return (v & 1) == 0;
}

template <std::size_t N>
constexpr bool is_even() noexcept
{
    return (N & 1) == 0;
}

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, bool>::type
is_odd(T v) noexcept
{
    return (v & 1) != 0;
}

template <std::size_t N>
constexpr bool is_odd() noexcept
{
    return (N & 1) != 0;
}

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, bool>::type
is_power_of_two(T v) noexcept
{
    return v != 0 && (v & (v - 1)) == 0;
}

template <std::size_t N>
constexpr bool is_power_of_two() noexcept
{
    return N != 0 && (N & (N - 1)) == 0;
}

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
next_power_of_two(T v) noexcept
{
    if (v == 0)
        return 1;

    --v; // handle edge case where v is already a power of two
    for (std::size_t i = 1; i < sizeof(T) * CHAR_BIT; i *= 2)
        v |= v >> i;

    return v + 1;
}

template <std::size_t N>
constexpr std::size_t next_power_of_two() noexcept
{
    if (N == 0)
        return 1;

    std::size_t v = N - 1;
    for (std::size_t i = 1; i < sizeof(std::size_t) * CHAR_BIT; i *= 2)
        v |= v >> i;

    return v + 1;
}

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
previous_power_of_two(T v) noexcept
{
    if (v == 0)
        return 0;

    for (std::size_t i = 1; i < sizeof(T) * CHAR_BIT; i *= 2)
        v |= v >> i;

    return v - (v >> 1);
}

template <std::size_t N>
constexpr std::size_t previous_power_of_two() noexcept
{
    if (N == 0)
        return 0;

    std::size_t v = N;
    for (std::size_t i = 1; i < sizeof(std::size_t) * CHAR_BIT; i *= 2)
        v |= v >> i;

    return v - (v >> 1);
}

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, int>::type
sign(T v) noexcept
{
    return (v > 0) - (v < 0);
}

template <std::size_t N>
constexpr int sign() noexcept
{
    return (N > 0) - (N < 0);
}

} // namespace bit
} // namespace castle