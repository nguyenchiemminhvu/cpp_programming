# C++17 › Class Template Argument Deduction (CTAD)

## Overview
CTAD allows the compiler to deduce the template arguments of a class template
from its constructor arguments, eliminating verbose `make_*` helpers and
explicit template argument lists in many contexts.

## Before vs After
```cpp
// C++11/14 — explicit or make_ helper
std::pair<int, double> p1{1, 2.5};
auto p2 = std::make_pair(1, 2.5);

// C++17 — CTAD
std::pair p3{1, 2.5};            // deduces pair<int, double>
std::vector v{1, 2, 3};          // deduces vector<int>
std::tuple  t{1, 2.0, "hello"};  // deduces tuple<int, double, const char*>
std::lock_guard lg{my_mutex};    // deduces lock_guard<std::mutex>
```

## Custom Deduction Guides
When constructors alone are insufficient, provide explicit deduction guides:
```cpp
template<typename T>
struct Wrapper { T value; Wrapper(T v) : value(v) {} };

template<typename T>
Wrapper(T) -> Wrapper<T>;    // deduction guide

Wrapper w{42};    // deduces Wrapper<int>
```

## Study Checklist
- [ ] Remove `make_pair` / `make_tuple` from a file; let CTAD deduce types
- [ ] Write a custom class with a non-trivial deduction guide
- [ ] Show where CTAD fails: `std::vector{1, 2.0}` — type conflict
- [ ] Explain: `std::vector<int> v(3)` vs `std::vector v{3}` — what does each produce?

## References
- [cppreference — CTAD](https://en.cppreference.com/w/cpp/language/class_template_argument_deduction)
