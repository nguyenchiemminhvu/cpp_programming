# C++23 › `std::to_underlying`

## Overview
C++23 (**P1682**) adds `std::to_underlying(E)` in `<utility>` — a trivial
helper that returns the *underlying integer value* of a scoped or unscoped
enumeration. It removes the verbose, error-prone
`static_cast<std::underlying_type_t<E>>(e)` boilerplate.

## Signature
```cpp
namespace std {
    template <class Enum>
    constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept;
}
```

## Basic Usage
```cpp
enum class colour : std::uint8_t { red = 1, green = 2, blue = 4 };

auto raw = std::to_underlying(colour::green);   // std::uint8_t{2}
```
Compared to:
```cpp
auto raw = static_cast<std::underlying_type_t<colour>>(colour::green);   // yikes
```

## Why It's Useful

### Bitmask enums
```cpp
enum class flags : std::uint32_t { none = 0, a = 1, b = 2, c = 4 };

flags mask = flags::a;
if (std::to_underlying(mask) & std::to_underlying(flags::a)) …
```

### Serialising to bytes / network
```cpp
buf << std::to_underlying(op_code);
```

### Interfacing with C APIs
```cpp
c_set_priority(std::to_underlying(priority::high));
```

### `switch` on unmapped integer
```cpp
switch (std::to_underlying(state)) {
    case 0: …
    case 1: …
}
```

## `constexpr` and `noexcept`
Both — usable in constant expressions and never throws.

## Type Safety
The template parameter is constrained to enumeration types by SFINAE (there
is no explicit concept in the standard wording, but the body would be
ill-formed for non-enums). Passing a non-enum produces a clear error.

## Study Checklist
- [ ] Replace all `static_cast<std::underlying_type_t<E>>(e)` in a codebase
- [ ] Use `std::to_underlying` in a `constexpr` context
- [ ] Build a bitmask-manipulation helper set on top of `to_underlying`
- [ ] Serialise an enum into a byte buffer via `to_underlying`
- [ ] Add a `std::formatter` for enums that formats as name-or-number using `to_underlying`

## References
- [P1682 — `std::to_underlying` for enumerations](https://wg21.link/P1682)
- [cppreference — `std::to_underlying`](https://en.cppreference.com/w/cpp/utility/to_underlying)
