#pragma once

#include <type_traits>
#include <cstdint>

namespace castle
{
namespace bit
{

template <typename T>
constexpr T rotate_left(T value, std::uint32_t shift) noexcept
{
    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);
    constexpr std::uint32_t bit_count = sizeof(T) * CHAR_BIT;
    shift %= bit_count;
    uval = (uval << shift) | (uval >> (bit_count - shift));
    return static_cast<T>(uval);
}

template <typename T>
constexpr T rotate_right(T value, std::uint32_t shift) noexcept
{
    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);
    constexpr std::uint32_t bit_count = sizeof(T) * CHAR_BIT;
    shift %= bit_count;
    uval = (uval >> shift) | (uval << (bit_count - shift));
    return static_cast<T>(uval);
}

} // namespace bit
} // namespace castle