# C++14 › Binary Literals & Digit Separators

## Overview
C++14 adds two complementary numeric literal features: **binary literals**
(`0b…`) for direct bit-pattern representation, and **digit separators** (`'`)
for human-readable large numbers.

## Binary Literals
```cpp
constexpr uint8_t mask   = 0b1111'0000;   // binary + digit separator
constexpr uint8_t flags  = 0b0000'1010;
constexpr uint8_t result = mask & flags;  // 0b0000'0000
```
The `0b` (or `0B`) prefix denotes a base-2 integer literal.

## Digit Separators
The single-quote `'` can appear between digits in **any** numeric literal
and is silently ignored by the compiler:
```cpp
constexpr long   million   = 1'000'000;
constexpr double avogadro  = 6.022'140'76e23;
constexpr int    rgb       = 0xFF'A0'7F;
constexpr int    bits      = 0b0001'1010'0011'0101;
```

## Study Checklist
- [ ] Define a set of hardware register bit masks using `0b` literals
- [ ] Use digit separators for a monetary value in cents: `int price_cents = 9'999;`
- [ ] Verify the separator has no runtime effect by comparing assembly output
- [ ] Mix binary + digit separator in a flag-register example with `enum class`

## References
- [cppreference — Integer literals](https://en.cppreference.com/w/cpp/language/integer_literal)
