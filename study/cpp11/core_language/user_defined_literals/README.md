# C++11 › User-Defined Literals

## Overview
User-defined literals (UDLs) allow custom suffixes for literal values, making
domain-specific quantities type-safe and expressive.

## Syntax
```cpp
// Raw literal operator
constexpr long double operator""_km(long double d) { return d * 1000.0; }
constexpr long double operator""_m (long double d) { return d; }

// Integer literal operator
constexpr unsigned long long operator""_MB(unsigned long long n) {
    return n * 1024 * 1024;
}

// Usage
auto dist = 3.5_km;   // 3500.0
auto size = 8_MB;     // 8388608
```

## Standard Library UDLs (C++14+)
```cpp
using namespace std::literals;
auto s  = "hello"s;    // std::string
auto sv = "world"sv;   // std::string_view
auto d  = 300ms;       // std::chrono::milliseconds
```

## Study Checklist
- [ ] Implement `_deg` (degrees to radians) and `_rad` literals
- [ ] Create `_km`, `_m`, `_cm` literals; enforce dimension safety via strong types
- [ ] Implement a binary literal parser as a raw `operator""_b`
- [ ] Note that user suffixes must start with `_`; others are library-reserved

## References
- [cppreference — User-defined literals](https://en.cppreference.com/w/cpp/language/user_literal)
