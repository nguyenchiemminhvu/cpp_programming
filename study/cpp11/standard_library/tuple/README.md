# C++11 › `std::tuple` — Heterogeneous Fixed-Size Collection

## Overview
`std::tuple<T1, T2, …>` is a fixed-size collection of heterogeneous elements,
generalising `std::pair` to an arbitrary number of types.

## Core Operations

```cpp
#include <tuple>

auto t = std::make_tuple(42, 3.14, std::string{"hello"});

// Access by index (compile-time constant)
int    x = std::get<0>(t);
double y = std::get<1>(t);

// Access by type (C++14; ambiguous if type appears more than once)
// auto s = std::get<std::string>(t);

// Decompose into individual variables
int a; double b; std::string c;
std::tie(a, b, c) = t;
// Ignore elements with std::ignore:
std::tie(a, std::ignore, c) = t;
```

## Comparison & Ordering
Tuples support `==`, `<`, etc. — lexicographic comparison across all elements.

## Use Cases
1. Returning multiple values from a function without defining a struct.
2. Sorting heterogeneous keys: `std::sort` on a `vector<tuple<int,string>>`.
3. `std::tie` for multi-key comparison: `return std::tie(a.x, a.y) < std::tie(b.x, b.y);`

## Study Checklist
- [ ] Return a 3-tuple from a function and unpack it with `std::tie`
- [ ] Sort a `vector` of tuples by second element, then first
- [ ] Use `std::ignore` to discard specific tuple elements
- [ ] Inspect `std::tuple_size` and `std::tuple_element`
- [ ] Compare tuple performance vs a plain struct for multiple return values

## References
- [cppreference — tuple](https://en.cppreference.com/w/cpp/utility/tuple)
- *Effective Modern C++*, Scott Meyers — Item 28 (forwarding with tuples)
