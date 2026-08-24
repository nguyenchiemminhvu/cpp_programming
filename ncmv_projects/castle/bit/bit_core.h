#pragma once

#include <cstdint>
#include <climits>

namespace castle
{
namespace bit
{

template <typename T>
constexpr bool test(T value, uint8_t bit_index)
{
    return (value >> bit_index) & T{1U} != T{0U};
}

template <typename T, std::size_t bit_index>
constexpr bool test(T value)
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");
    return (value >> bit_index) & T{1U} != T{0U};
}

template <typename T>
constexpr T set(T value, uint8_t bit_index)
{
    return static_cast<T>(value | (T{1U} << bit_index));
}

template <typename T, std::size_t bit_index>
constexpr T set(T value)
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");
    return static_cast<T>(value | (T{1U} << bit_index));
}

template <typename T>
constexpr T clear(T value, uint8_t bit_index)
{
    return static_cast<T>(value & static_cast<T>(~(T{1U} << bit_index)));
}

template <typename T, std::size_t bit_index>
constexpr T clear(T value)
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");
    return static_cast<T>(value & static_cast<T>(~(T{1U} << bit_index)));
}

template <typename T>
constexpr T toggle(T value, uint8_t bit_index)
{
    return static_cast<T>(value ^ (T{1U} << bit_index));
}

template <typename T, std::size_t bit_index>
constexpr T toggle(T value)
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");
    return static_cast<T>(value ^ (T{1U} << bit_index));
}

} // namespace bit
} // namespace castle