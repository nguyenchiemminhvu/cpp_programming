# C++20 › `consteval` — Immediate Functions

## Overview
`consteval` declares a function as an **immediate function**: every call to it
**must** produce a constant expression. Unlike `constexpr` (which *may* run
at compile time), `consteval` *always* runs at compile time — a non-constant
argument is a compile error.

## `constexpr` vs `consteval`
| Keyword | Compile-time | Run-time |
|---------|-------------|---------|
| `constexpr` | Yes (when possible) | Yes (fallback) |
| `consteval` | Always | ERROR |

## Examples
```cpp
consteval int square(int n) { return n * n; }

constexpr int x = square(5);   // OK — constant expression
// int y = square(runtime_val); // ERROR — not a constant expression
```

## Compile-Time Lookup Table Pattern
```cpp
consteval std::array<int, 256> build_lut() {
    std::array<int, 256> lut{};
    for (int i = 0; i < 256; ++i) lut[i] = i * i;
    return lut;
}
static constexpr auto LUT = build_lut();
```

## Study Checklist
- [ ] Write a `consteval` function and verify it errors on a runtime argument
- [ ] Build a compile-time lookup table and use it in a `switch`
- [ ] Explain when `consteval` is preferable to `constexpr` (e.g., security-sensitive compile-time checks)
- [ ] Combine with `std::is_constant_evaluated()` inside a `constexpr` function

## References
- [cppreference — consteval](https://en.cppreference.com/w/cpp/language/consteval)
