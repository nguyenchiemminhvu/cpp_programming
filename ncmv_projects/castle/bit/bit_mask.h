#pragma once

#include <type_traits>
#include <cstdint>

namespace castle
{
namespace bit
{

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
all_bits_mask() noexcept
{
    return static_cast<T>(~T{0U});
}

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
single_bit_mask(std::uint32_t bit_index) noexcept
{
    return static_cast<T>(T{1U} << bit_index);
}

template <typename T, std::size_t bit_index>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
single_bit_mask() noexcept
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");
    return static_cast<T>(T{1U} << bit_index);
}

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
low_bits_mask(std::uint32_t bit_count) noexcept
{
    return static_cast<T>(T{1U} << bit_count) - T{1U};
}

template <typename T, std::size_t bit_count>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
low_bits_mask() noexcept
{
    static_assert(bit_count <= sizeof(T) * CHAR_BIT, "bit_count is out of range for the type T");
    return static_cast<T>(T{1U} << bit_count) - T{1U};
}

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
high_bits_mask(std::uint32_t bit_count) noexcept
{
    return static_cast<T>(~(T{1U} << (sizeof(T) * CHAR_BIT - bit_count)) + T{1U});
}

template <typename T, std::size_t bit_count>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
high_bits_mask() noexcept
{
    static_assert(bit_count <= sizeof(T) * CHAR_BIT, "bit_count is out of range for the type T");
    return static_cast<T>(~(T{1U} << (sizeof(T) * CHAR_BIT - bit_count)) + T{1U});
}

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
range_mask(std::uint32_t start_bit_index, std::uint32_t bit_count) noexcept
{
    return static_cast<T>(low_bits_mask<T>(bit_count) << start_bit_index);
}

template <typename T, std::size_t start_bit_index, std::size_t bit_count>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
range_mask() noexcept
{
    static_assert(start_bit_index + bit_count <= sizeof(T) * CHAR_BIT, "start_bit_index + bit_count is out of range for the type T");
    return static_cast<T>(low_bits_mask<T>(bit_count) << start_bit_index);
}

} // namespace bit
} // namespace castle