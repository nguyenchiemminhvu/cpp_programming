# C++23 › `size_t` and `ptrdiff_t` Literal Suffixes

## Overview
C++23 (**P0330**) adds two new integer literal suffixes:

| Suffix | Type | Signedness |
|---|---|---|
| `z` / `Z` | `std::ptrdiff_t` (or a matching signed integer) | signed |
| `uz` / `UZ` / `Zu` / `ZU` | `std::size_t` | unsigned |

Before C++23, writing a portable `size_t` literal required `std::size_t{5}` or
`static_cast<std::size_t>(5)`. Now:
```cpp
for (auto i = 0uz; i < v.size(); ++i) { … }
```

## Rules
- `z` on its own → signed `std::ptrdiff_t`.
- Combined with `u` (any order, any case) → unsigned `std::size_t`.
- No effect on floating-point or character literals.
- Available for decimal, octal, hex, and binary literals: `0uz`, `0xffuz`,
  `0b1010uz`.

## Why You Care
### `auto` + range-for indices
```cpp
// ❌ pre-C++23: `auto i = 0` deduces `int`, then compared with `size()` warns/traps
for (auto i = 0; i < v.size(); ++i) { … }

// ✅ C++23
for (auto i = 0uz; i < v.size(); ++i) { … }
```

### `std::ssize`
Pairs perfectly with `std::ssize` (C++20):
```cpp
for (auto i = 0z; i < std::ssize(v); ++i) { … }
```

### API design
Prevents accidental narrowing when returning literal sizes:
```cpp
constexpr auto default_capacity() { return 64uz; }   // returns size_t
```

## Study Checklist
- [ ] Rewrite an index loop over `v.size()` using `auto` + `0uz`
- [ ] Confirm `-Wsign-compare` warnings disappear
- [ ] Verify `sizeof(0uz) == sizeof(std::size_t)` and `sizeof(0z) == sizeof(std::ptrdiff_t)`
- [ ] Try binary / hex forms: `0xFFuz`, `0b1_1010uz` (also uses digit separator)
- [ ] Combine with `std::ssize` for signed range-based indexing

## References
- [P0330 — Literal suffix for (signed) `size_t`](https://wg21.link/P0330)
- [cppreference — integer literals](https://en.cppreference.com/w/cpp/language/integer_literal)
