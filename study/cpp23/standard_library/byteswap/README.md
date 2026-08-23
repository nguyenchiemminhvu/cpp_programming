# C++23 › `std::byteswap`

## Overview
C++23 (**P1272**) adds `std::byteswap(T)` in `<bit>` — a `constexpr`
reverse-byte-order function for any integer type. It standardises what every
project used to hand-roll or borrow from compiler builtins
(`__builtin_bswap*`, `_byteswap_ulong`, `htonl`).

## Signature
```cpp
namespace std {
    template <std::integral T>
    constexpr T byteswap(T value) noexcept;
}
```

## Usage
```cpp
#include <bit>

std::uint32_t x = 0x12345678;
auto swapped = std::byteswap(x);   // 0x78563412
```
Works with `std::uint16_t`, `uint32_t`, `uint64_t`, `int64_t`, and even
extended integer types where supported.

## `constexpr`
`byteswap` is `constexpr`, so it composes with compile-time endianness
conversion tables:
```cpp
static_assert(std::byteswap<std::uint16_t>(0x00FFu) == 0xFF00u);
```

## Endianness Conversion Idiom
```cpp
template <std::integral T>
constexpr T to_big_endian(T x) noexcept {
    if constexpr (std::endian::native == std::endian::big) return x;
    else                                                   return std::byteswap(x);
}
```
Pairs perfectly with `std::endian` (C++20).

## Constraints
- The template parameter must satisfy `std::integral` — no floats, no
  `bool`, no character types with 1-byte width (they would be a no-op, and
  the standard forbids `bool`/`char`).
- Behaviour is well-defined for signed types (implementation performs the
  byte reversal on the object representation).

## Common Pitfalls
- **Networking code**: prefer `std::byteswap` + `std::endian` over `htonl` /
  `ntohl` in new code — portable, `constexpr`, no `<arpa/inet.h>` include.
- **Structs**: `byteswap` swaps *one integer at a time*; a struct with multiple
  fields needs per-field swaps.
- **Bit-fields**: not directly supported — extract to a plain integer first.

## Study Checklist
- [ ] Write `to_big_endian` / `to_little_endian` generic helpers
- [ ] Use `std::byteswap` in a `constexpr` context — assert the result
- [ ] Replace `htonl` / `ntohl` in a small networking snippet
- [ ] Verify byte-count on `int16`, `int32`, `int64`
- [ ] Read a big-endian file header on a little-endian host and vice-versa

## References
- [P1272 — Byteswapping for fun and profit](https://wg21.link/P1272)
- [cppreference — `std::byteswap`](https://en.cppreference.com/w/cpp/numeric/byteswap)
