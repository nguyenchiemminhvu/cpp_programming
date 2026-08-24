#pragma once

#include <type_traits>
#include <cstdint>

namespace castle
{
namespace bit
{

// SWAR (SIMD Within A Register) popcount. Branch-free, constexpr, no lookup tables.

constexpr std::uint32_t popcount(std::uint8_t value) noexcept
{
    std::uint32_t x = value;
    x -= (x >> 1) & 0x55U;
    x = (x & 0x33U) + ((x >> 2) & 0x33U);
    x = (x + (x >> 4)) & 0x0FU;
    return x;
}

constexpr std::uint32_t popcount(std::uint16_t value) noexcept
{
    std::uint32_t x = value;
    x -= (x >> 1) & 0x5555U;
    x = (x & 0x3333U) + ((x >> 2) & 0x3333U);
    x = (x + (x >> 4)) & 0x0F0FU;
    return (x * 0x0101U) >> 8;
}

constexpr std::uint32_t popcount(std::uint32_t value) noexcept
{
    std::uint32_t x = value;
    x -= (x >> 1) & 0x55555555U;
    x = (x & 0x33333333U) + ((x >> 2) & 0x33333333U);
    x = (x + (x >> 4)) & 0x0F0F0F0FU;
    return (x * 0x01010101U) >> 24;
}

constexpr std::uint32_t popcount(std::uint64_t value) noexcept
{
    std::uint64_t x = value;
    x -= (x >> 1) & 0x5555555555555555ULL;
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return static_cast<std::uint32_t>((x * 0x0101010101010101ULL) >> 56);
}

// Generic dispatcher: routes any integral (including signed / char / bool) to the
// matching unsigned-fixed-width overload above via its underlying bit pattern.
template <typename T>
constexpr
typename std::enable_if<std::is_integral<T>::value, T>::type
popcount(T value) noexcept
{
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8, "popcount: unsupported integral size");

    return
        sizeof(T) == 1 ? popcount(static_cast<std::uint8_t>(value))  :
        sizeof(T) == 2 ? popcount(static_cast<std::uint16_t>(value)) :
        sizeof(T) == 4 ? popcount(static_cast<std::uint32_t>(value)) :
                         popcount(static_cast<std::uint64_t>(value));
}

} // namespace bit
} // namespace castle