#pragma once

#include <type_traits>
#include <cstdint>
#include <climits>

namespace castle
{
namespace bit
{

// ──────────────────────────────────────────────────────────────
// is_even / is_odd — check the parity of an integer using
// the least-significant bit. Branch-free.
// ──────────────────────────────────────────────────────────────

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

// ──────────────────────────────────────────────────────────────
// is_power_of_two — returns true if the value is an exact
// power of two (has exactly one bit set). Returns false for
// zero and negative values.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, bool>::type
is_power_of_two(T v) noexcept
{
    if (v < 0)
        return false; // Negative numbers are not powers of two

    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(v);
    return uval != 0 && (uval & (uval - 1)) == 0;
}

template <std::size_t N>
constexpr bool is_power_of_two() noexcept
{
    return N != 0 && (N & (N - 1)) == 0;
}

// ──────────────────────────────────────────────────────────────
// next_power_of_two — returns the smallest power of two that
// is greater than or equal to the input value.
// Returns 1 for input 0.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
next_power_of_two(T v) noexcept
{
    if (v == 0)
        return 1;

    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(v);

    --uval; // handle edge case where v is already a power of two
    for (std::size_t i = 1; i < sizeof(T) * CHAR_BIT; i *= 2)
        uval |= uval >> i;

    return static_cast<T>(uval + 1);
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

// ──────────────────────────────────────────────────────────────
// previous_power_of_two — returns the largest power of two
// that is less than or equal to the input value.
// Returns 0 for input 0.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
previous_power_of_two(T v) noexcept
{
    if (v == 0)
        return 0;

    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(v);

    for (std::size_t i = 1; i < sizeof(T) * CHAR_BIT; i *= 2)
        uval |= uval >> i;

    return static_cast<T>(uval - (uval >> 1));
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

// ──────────────────────────────────────────────────────────────
// align_up / align_down — round an address or size to the
// nearest multiple of a power-of-two alignment.
// Ubiquitous in DMA buffer setup, memory-mapped I/O, and
// linker-script math in embedded systems.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
align_up(T value, T alignment) noexcept
{
    // alignment must be a power of two; caller's responsibility.
    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);
    UnsignedT mask = static_cast<UnsignedT>(alignment) - UnsignedT{1U};
    return static_cast<T>((uval + mask) & ~mask);
}

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, T>::type
align_down(T value, T alignment) noexcept
{
    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT uval = static_cast<UnsignedT>(value);
    UnsignedT mask = static_cast<UnsignedT>(alignment) - UnsignedT{1U};
    return static_cast<T>(uval & ~mask);
}

// ──────────────────────────────────────────────────────────────
// is_aligned — returns true if the value is a multiple of the
// given power-of-two alignment.
// ──────────────────────────────────────────────────────────────

template <typename T>
constexpr
typename std::enable_if_t<std::is_integral<T>::value, bool>::type
is_aligned(T value, T alignment) noexcept
{
    using UnsignedT = typename std::make_unsigned<T>::type;
    UnsignedT mask = static_cast<UnsignedT>(alignment) - UnsignedT{1U};
    return (static_cast<UnsignedT>(value) & mask) == UnsignedT{0U};
}

// ──────────────────────────────────────────────────────────────
// sign — returns -1, 0, or +1 indicating the sign of a value.
// Branch-free implementation using comparison operators.
// ──────────────────────────────────────────────────────────────

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