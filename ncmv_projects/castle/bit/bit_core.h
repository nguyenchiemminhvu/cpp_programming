#pragma once

#include <type_traits>
#include <cstdint>
#include <climits>

namespace castle
{
namespace bit
{

// ──────────────────────────────────────────────────────────────
// test — check whether a specific bit is set (1) in a value.
//   bit_index : zero-based position of the bit to test.
// Returns true if the bit is set, false otherwise.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, bool>::type
test(T value, std::uint32_t bit_index) noexcept
{
    if (bit_index >= sizeof(T) * CHAR_BIT)
        return false;

    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);

    return ((uval >> bit_index) & UnsignedT{1U}) != UnsignedT{0U};
}

template <typename T, std::size_t bit_index>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, bool>::type
test(T value) noexcept
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");

    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);

    return ((uval >> bit_index) & UnsignedT{1U}) != UnsignedT{0U};
}

// ──────────────────────────────────────────────────────────────
// set — set a specific bit to 1 in a value.
//   bit_index : zero-based position of the bit to set.
// Returns the modified value with the target bit set.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
set(T value, std::uint32_t bit_index) noexcept
{
    if (bit_index >= sizeof(T) * CHAR_BIT)
        return value;

    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);

    return static_cast<T>(uval | (UnsignedT{1U} << bit_index));
}

template <typename T, std::size_t bit_index>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
set(T value) noexcept
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");

    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);

    return static_cast<T>(uval | (UnsignedT{1U} << bit_index));
}

// ──────────────────────────────────────────────────────────────
// clear — clear a specific bit to 0 in a value.
//   bit_index : zero-based position of the bit to clear.
// Returns the modified value with the target bit cleared.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
clear(T value, std::uint32_t bit_index) noexcept
{
    if (bit_index >= sizeof(T) * CHAR_BIT)
        return value;

    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);

    return static_cast<T>(uval & static_cast<UnsignedT>(~(UnsignedT{1U} << bit_index)));
}

template <typename T, std::size_t bit_index>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
clear(T value) noexcept
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");

    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);

    return static_cast<T>(uval & static_cast<UnsignedT>(~(UnsignedT{1U} << bit_index)));
}

// ──────────────────────────────────────────────────────────────
// toggle — flip a specific bit in a value (0→1, 1→0).
//   bit_index : zero-based position of the bit to toggle.
// Returns the modified value with the target bit inverted.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
toggle(T value, std::uint32_t bit_index) noexcept
{
    if (bit_index >= sizeof(T) * CHAR_BIT)
        return value;

    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);

    return static_cast<T>(uval ^ (UnsignedT{1U} << bit_index));
}

template <typename T, std::size_t bit_index>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
toggle(T value) noexcept
{
    static_assert(bit_index < sizeof(T) * CHAR_BIT, "bit_index is out of range for the type T");

    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);

    return static_cast<T>(uval ^ (UnsignedT{1U} << bit_index));
}

} // namespace bit
} // namespace castle