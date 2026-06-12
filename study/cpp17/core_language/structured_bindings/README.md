# C++17 › Structured Bindings

## Overview
Structured bindings (`auto [a, b, c] = expr;`) decompose an aggregate, tuple,
or map element into named variables in a single declaration.

## Syntax
```cpp
auto [var1, var2, …] = expression;
auto& [var1, var2]   = expression;  // bind by reference
const auto& [x, y]   = expression;  // const reference
```

## Supported Types
| Type | Example |
|------|---------|
| `std::pair` | `auto [key, val] = *map.begin();` |
| `std::tuple` | `auto [a, b, c] = make_tuple(1, 2.0, "x");` |
| Plain `struct` / `class` (all public members) | `auto [x, y] = Point{1, 2};` |
| C-style array | `int arr[3]{}; auto [a, b, c] = arr;` |

## Before vs After C++17
```cpp
// C++11/14 — verbose
for (const auto& kv : map)
    process(kv.first, kv.second);

// C++17 — expressive
for (const auto& [key, value] : map)
    process(key, value);
```

## Study Checklist
- [ ] Use structured bindings to iterate a `std::map`
- [ ] Decompose a `std::tuple` return value with structured bindings
- [ ] Bind a custom struct by reference and mutate a member
- [ ] Show the underlying mechanism via `std::tuple_size` and `std::get`
- [ ] Explain why you cannot use `auto [a] = f();` when `f` returns a reference

## References
- [cppreference — Structured bindings](https://en.cppreference.com/w/cpp/language/structured_binding)
