# C++14 › Variable Templates

## Overview
Variable templates allow a variable to be parameterised on a type or value,
providing a clean syntax for typed constants and enabling the `_v` suffix
convention used throughout `<type_traits>`.

## Syntax
```cpp
template<typename T>
constexpr T pi = T(3.14159265358979323846L);

pi<float>;        // float  precision
pi<double>;       // double precision
pi<long double>;  // maximum precision
```

## The `_v` Convention
C++14 introduced `_v` aliases for `<type_traits>`:
```cpp
// C++11 — verbose
std::is_integral<T>::value

// C++14 — variable template alias
std::is_integral_v<T>
```

## Non-Type Template Parameter
```cpp
template<int N>
constexpr int factorial_v = N * factorial_v<N - 1>;

template<>
constexpr int factorial_v<0> = 1;

static_assert(factorial_v<5> == 120);
```

## Study Checklist
- [ ] Define `pi<T>`, `e<T>`, and `sqrt2<T>` variable templates
- [ ] Replace all `::value` accesses in a file with `_v` aliases
- [ ] Write a non-type variable template for Fibonacci numbers
- [ ] Use a variable template inside a `static_assert`

## References
- [cppreference — Variable templates](https://en.cppreference.com/w/cpp/language/variable_template)
