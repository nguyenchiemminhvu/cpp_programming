#pragma once

#include <type_traits>
#include <cstdint>
#include <climits>

namespace castle
{
namespace bit
{

// ──────────────────────────────────────────────────────────────
// all_bits_mask — returns a value with every bit set to 1.
// Equivalent to ~T{0}.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
all_bits_mask() noexcept
{
    return static_cast<T>(~T{0U});
}

// ──────────────────────────────────────────────────────────────
// single_bit_mask — returns a value with only one bit set.
//   bit_index : zero-based position of the bit to set.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
single_bit_mask(std::uint32_t bit_index) noexcept
{
    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(UnsignedT{1U} << bit_index);
    return static_cast<T>(uval);
}

template <typename T, std::size_t bit_index>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
single_bit_mask() noexcept
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");
    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(UnsignedT{1U} << bit_index);
    return static_cast<T>(uval);
}

// ──────────────────────────────────────────────────────────────
// low_bits_mask — returns a mask with the lowest N bits set.
//   bit_count : number of low-order bits to set.
// E.g. low_bits_mask<uint8_t>(3) → 0b00000111.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
low_bits_mask(std::uint32_t bit_count) noexcept
{
    using UnsignedT = typename std::make_unsigned<T>::type;

    if (bit_count >= sizeof(T) * CHAR_BIT)
        return static_cast<T>(~UnsignedT{0U});

    UnsignedT uval = static_cast<UnsignedT>(UnsignedT{1U} << bit_count) - UnsignedT{1U};
    return static_cast<T>(uval);
}

template <typename T, std::size_t bit_count>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
low_bits_mask() noexcept
{
    static_assert(bit_count <= sizeof(T) * CHAR_BIT, "bit_count is out of range for the type T");
    using UnsignedT = typename std::make_unsigned<T>::type;

    if constexpr (bit_count == sizeof(T) * CHAR_BIT)
    {
        return static_cast<T>(~UnsignedT{0U});
    }
    else
    {
        UnsignedT uval = static_cast<UnsignedT>(UnsignedT{1U} << bit_count) - UnsignedT{1U};
        return static_cast<T>(uval);
    }
}

// ──────────────────────────────────────────────────────────────
// high_bits_mask — returns a mask with the highest N bits set.
//   bit_count : number of high-order bits to set.
// E.g. high_bits_mask<uint8_t>(3) → 0b11100000.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
high_bits_mask(std::uint32_t bit_count) noexcept
{
    using UnsignedT = typename std::make_unsigned<T>::type;

    if (bit_count == 0)
        return static_cast<T>(UnsignedT{0U});

    if (bit_count >= sizeof(T) * CHAR_BIT)
        return static_cast<T>(~UnsignedT{0U});

    UnsignedT uval = static_cast<UnsignedT>(~UnsignedT{0U}) << (sizeof(T) * CHAR_BIT - bit_count);
    return static_cast<T>(uval);
}

template <typename T, std::size_t bit_count>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
high_bits_mask() noexcept
{
    static_assert(bit_count <= sizeof(T) * CHAR_BIT, "bit_count is out of range for the type T");
    using UnsignedT = typename std::make_unsigned<T>::type;

    if constexpr (bit_count == 0)
    {
        return static_cast<T>(UnsignedT{0U});
    }
    else if constexpr (bit_count == sizeof(T) * CHAR_BIT)
    {
        return static_cast<T>(~UnsignedT{0U});
    }
    else
    {
        UnsignedT uval = static_cast<UnsignedT>(~UnsignedT{0U}) << (sizeof(T) * CHAR_BIT - bit_count);
        return static_cast<T>(uval);
    }
}

// ──────────────────────────────────────────────────────────────
// range_mask — returns a mask with a contiguous range of bits
// set, starting at start_bit_index with bit_count bits wide.
// E.g. range_mask<uint8_t>(2, 3) → 0b00011100.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
range_mask(std::uint32_t start_bit_index, std::uint32_t bit_count) noexcept
{
    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(low_bits_mask<UnsignedT>(bit_count) << start_bit_index);
    return static_cast<T>(uval);
}

template <typename T, std::size_t start_bit_index, std::size_t bit_count>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
range_mask() noexcept
{
    static_assert(start_bit_index + bit_count <= sizeof(T) * CHAR_BIT, "start_bit_index + bit_count is out of range for the type T");
    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(low_bits_mask<UnsignedT>(bit_count) << start_bit_index);
    return static_cast<T>(uval);
}

} // namespace bit
} // namespace castle