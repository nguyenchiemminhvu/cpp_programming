#pragma once

#include <type_traits>
#include <cstdint>

namespace castle
{
namespace bit
{

template <typename T>
constexpr std::uint32_t lowest_set_bit_index(T value) noexcept
{
}

template <typename T>
constexpr std::uint32_t highest_set_bit_index(T value) noexcept
{
}

} // namespace bit
} // namespace castle