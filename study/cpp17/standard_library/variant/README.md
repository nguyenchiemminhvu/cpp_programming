# C++17 › `std::variant<T1, T2, …>`

## Overview
`std::variant` is a **type-safe tagged union**. It holds exactly one value at a
time from a fixed set of types, eliminating raw `union` and associated undefined
behaviour from type-punning.

## Core API
```cpp
#include <variant>

std::variant<int, double, std::string> v = 42;

// Access
int x = std::get<int>(v);             // throws std::bad_variant_access if wrong type
int* px = std::get_if<int>(&v);       // returns nullptr if wrong type
std::size_t idx = v.index();          // 0=int, 1=double, 2=string

// Assignment
v = 3.14;    // now holds double (index = 1)
v = "hello"; // now holds string (index = 2)
```

## `std::visit` — Pattern Matching
```cpp
std::visit([](auto&& val) {
    using T = std::decay_t<decltype(val)>;
    if constexpr (std::is_same_v<T, int>)       std::cout << "int: " << val;
    else if constexpr (std::is_same_v<T, double>) std::cout << "double: " << val;
    else                                          std::cout << "string: " << val;
}, v);
```

## `std::monostate` — Default-Constructible Empty State
Use `std::variant<std::monostate, T1, T2>` when the variant must be
default-constructible but `T1`/`T2` are not.

## Study Checklist
- [ ] Replace a `union` + enum discriminant with `std::variant`
- [ ] Implement a simple expression tree using `std::variant` and `std::visit`
- [ ] Use `std::get_if` for safe non-throwing access
- [ ] Handle `std::bad_variant_access` when using `std::get` incorrectly
- [ ] Benchmark `std::visit` overhead vs virtual dispatch

## References
- [cppreference — variant](https://en.cppreference.com/w/cpp/utility/variant)
