# C++23 › Formatting Ranges

## Overview
C++23 (**P2286**) extends `std::format` to natively format **ranges**,
**tuples**, and **pair**s — no more manual loops, no more custom formatters
for common containers. Combined with `std::print`, the result is a
`Python`-quality debug-print experience.

## Basic Usage
```cpp
#include <format>
#include <vector>
#include <map>

std::vector<int>     v = {1, 2, 3};
std::map<std::string, int> m = {{"a", 1}, {"b", 2}};

std::print("{}\n", v);   // [1, 2, 3]
std::print("{}\n", m);   // {"a": 1, "b": 2}
```

## Range-Formatting Options

### Explicit range specifier `?`
Debug-format each element (adds quotes to strings, escapes special chars):
```cpp
std::vector<std::string> words{"a", "b\n", "c"};
std::print("{::}\n",  words);     // [a, b
                                  // , c]
std::print("{::?}\n", words);     // ["a", "b\n", "c"]
```
The colon inside the format spec separates the *range spec* from the *element
spec*.

### Range spec
Before the colon, describe how the range is printed:
```cpp
std::print("{:n}\n", v);           // 1, 2, 3       (no brackets)
std::print("{}\n",   v);           // [1, 2, 3]
```

### Custom brackets
For sets: `{...}`; for tuples: `(..., ..., ...)`. Overridable via user
`std::formatter` if needed.

## Tuple and Pair Formatting
```cpp
std::tuple<int, std::string, double> t{1, "hi", 3.14};
std::print("{}\n", t);             // (1, "hi", 3.14)
```

## Nested Containers
Recursive formatting works automatically:
```cpp
std::vector<std::vector<int>> grid = {{1,2,3},{4,5,6}};
std::print("{}\n", grid);          // [[1, 2, 3], [4, 5, 6]]
```

## Per-Element Format Specifier
```cpp
std::vector<int> nums = {1, 16, 255};
std::print("{::#04x}\n", nums);    // [0x01, 0x10, 0xff]
```
Everything after the second `:` is applied to each element.

## Related C++23 Formatter Additions
- **`std::formatter<std::pair>` / `<std::tuple>`** — see above.
- **`std::formatter<std::stacktrace>`** — see the `stacktrace/` sibling.
- **`std::formatter<std::thread::id>`** — for logging thread IDs.
- **`std::formatter<T*>`** improvements — sensible pointer formatting.
- **Escaped `char` / `string` (`?` presentation type)** — for debug output.

## Custom Types
Any type modelled as a range (has `begin()` / `end()`) is automatically
formattable via the range formatter. You can *opt out* by specialising:
```cpp
template <> struct std::formatter<my_range> : std::range_formatter<my_range> {};
```
or write a manual `formatter` if you want a custom textual representation.

## Interaction With `std::print`
`std::print("{}\n", v)` is the shortest fully-formatted range dump — no
allocation compared to `std::cout << std::format(...)`.

## Availability
- GCC 14+, Clang 17+ (libc++ partial), MSVC 19.37+ (VS 2022 17.7)

## Study Checklist
- [ ] Print a `std::vector<int>` and a `std::map<K,V>` using `std::print("{}", …)`
- [ ] Use the `?` element-debug specifier on a `vector<string>`
- [ ] Use `{:n}` to omit brackets; use `{::#04x}` for per-element hex
- [ ] Format a `std::tuple` and a `std::pair`
- [ ] Format a range of `std::optional` — observe the debug output

## References
- [P2286 — Formatting ranges](https://wg21.link/P2286)
- [cppreference — Ranges formatter](https://en.cppreference.com/w/cpp/utility/format/range_formatter)
