# C++20 › `std::format`

## Overview
`std::format` is a type-safe, locale-independent string formatting library
modelled on Python's `str.format()` and `fmtlib`. It replaces `printf` and
`std::ostringstream` with a readable, efficient, extensible API.

## Syntax
```cpp
#include <format>

std::string s = std::format("Hello, {}!", "world");          // "Hello, world!"
std::string n = std::format("{:>10.2f}", 3.14159);           // "      3.14"
std::string h = std::format("{:#010x}", 255);                 // "0x000000ff"
```

## Format Specification Mini-Language
```
{[arg-id]:[fill][align][sign][#][0][width][.precision][type]}
```
| Specifier | Meaning |
|-----------|---------|
| `>`, `<`, `^` | Right/left/center align |
| `+` | Show sign for positive numbers |
| `#` | Alternate form (`0x` prefix for hex, etc.) |
| `.N` | Precision for floats / max chars for strings |
| `d`, `x`, `o`, `b` | Integer bases |
| `e`, `f`, `g` | Float formats |

## `std::print` and `std::println` (C++23 preview)
```cpp
std::print("x = {}\n", 42);   // writes directly to stdout, no allocation
```

## Custom Type Formatting
```cpp
template<> struct std::formatter<Point> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }
    auto format(const Point& p, auto& ctx) const {
        return std::format_to(ctx.out(), "({}, {})", p.x, p.y);
    }
};
```

## Study Checklist
- [ ] Format a table of numbers with aligned columns using width + fill specifiers
- [ ] Implement a custom `std::formatter` for a user-defined type
- [ ] Compare `std::format` performance vs `printf` vs `ostringstream`
- [ ] Use `std::format_to` to write directly into a buffer without a string allocation
- [ ] Format integers in binary, octal, and hex with and without prefix

## References
- [cppreference — format](https://en.cppreference.com/w/cpp/utility/format)
- [{fmt} library](https://github.com/fmtlib/fmt) — the reference implementation
