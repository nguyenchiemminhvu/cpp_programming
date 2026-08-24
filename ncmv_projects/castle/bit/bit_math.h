#pragma once

#include <type_traits>

namespace castle
{
namespace bit
{

template <std::size_t N>
constexpr bool is_power_of_two() noexcept
{
    return N != 0 && (N & (N - 1)) == 0;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
constexpr bool is_power_of_two(T v) noexcept
{
    return v != 0 && (v & (v - 1)) == 0;
}

} // namespace bit
} // namespace castle