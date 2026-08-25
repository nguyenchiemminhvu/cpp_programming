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
typename std::enable_if<std::is_integral<T>::value, T>::type
extract_lowest_set_bit(T value) noexcept
{
    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);

    return static_cast<T>(uval & -uval);
}

template <typename T>
constexpr
typename std::enable_if<std::is_integral<T>::value, T>::type
extract_highest_set_bit(T value) noexcept
{
    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);

    if (uval == 0) return 0;

    for (std::size_t i = 1; i < sizeof(T) * CHAR_BIT; i *= 2)
        uval |= uval >> i;

    return static_cast<T>(uval - (uval >> 1));
}

} // namespace bit
} // namespace castle