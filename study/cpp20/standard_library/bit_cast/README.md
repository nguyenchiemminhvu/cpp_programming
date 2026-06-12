# C++20 › `std::bit_cast`

## Overview
`std::bit_cast<To>(from)` reinterprets the object representation of `from` as
type `To`, providing a **safe, well-defined** alternative to the `memcpy`-pun
pattern and the UB-inducing `reinterpret_cast` on value types.

## vs `reinterpret_cast`
```cpp
float f = 1.0f;

// UB — strict aliasing violation:
// uint32_t i = *reinterpret_cast<uint32_t*>(&f);

// Well-defined (C++20):
uint32_t i = std::bit_cast<uint32_t>(f);
```

## Requirements
- `sizeof(To) == sizeof(From)` — compile error otherwise.
- Both types must be **trivially copyable**.
- `constexpr`-evaluatable (unlike `memcpy`-based punning).

## Common Use Cases
1. Inspect the IEEE 754 bit pattern of a `float` / `double`.
2. Serialise/deserialise primitives to/from byte buffers.
3. Type-safe punning for SIMD intrinsics and hardware register access.

## Study Checklist
- [ ] Extract the sign, exponent, and mantissa bits of `1.0f` using `bit_cast<uint32_t>`
- [ ] Verify that `bit_cast` is `constexpr` via `static_assert`
- [ ] Show the compile error when `sizeof(To) != sizeof(From)`
- [ ] Compare generated assembly of `bit_cast` vs `memcpy`-pun (should be identical)

## References
- [cppreference — bit_cast](https://en.cppreference.com/w/cpp/numeric/bit_cast)
