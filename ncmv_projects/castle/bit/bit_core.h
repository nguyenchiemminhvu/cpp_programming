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
test(T value, std::uint32_t bit_index) noexcept
{
    return (value >> bit_index) & T{1U} != T{0U};
}

template <typename T, std::size_t bit_index>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, bool>::type
test(T value) noexcept
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");
    return (value >> bit_index) & T{1U} != T{0U};
}

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
set(T value, std::uint32_t bit_index) noexcept
{
    return static_cast<T>(value | (T{1U} << bit_index));
}

template <typename T, std::size_t bit_index>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
set(T value) noexcept
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");
    return static_cast<T>(value | (T{1U} << bit_index));
}

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
clear(T value, std::uint32_t bit_index) noexcept
{
    return static_cast<T>(value & static_cast<T>(~(T{1U} << bit_index)));
}

template <typename T, std::size_t bit_index>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
clear(T value) noexcept
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");
    return static_cast<T>(value & static_cast<T>(~(T{1U} << bit_index)));
}

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
toggle(T value, std::uint32_t bit_index) noexcept
{
    return static_cast<T>(value ^ (T{1U} << bit_index));
}

template <typename T, std::size_t bit_index>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
toggle(T value) noexcept
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");
    return static_cast<T>(value ^ (T{1U} << bit_index));
}

} // namespace bit
} // namespace castle