# C++20 › Template Lambdas

## Overview
C++20 extends lambda syntax to support **explicit template parameters**
(`[]<typename T>(T x){}`), providing full template power inside lambdas —
named type parameters, non-type template params, and `requires` constraints.

## vs C++14 Generic Lambda (`auto` param)
| | C++14 `auto` param | C++20 template lambda |
|-|--------------------|----------------------|
| Type name in body | `decltype(x)` | `T` directly |
| Constraints | `static_assert` / SFINAE | `requires` clause / concept |
| Non-type template params | No | Yes |

## Examples
```cpp
// Named type parameter
auto identity = []<typename T>(T x) -> T { return x; };

// With concept constraint
auto integral_only = []<std::integral T>(T x) { return x * 2; };

// Non-type template parameter
auto nth_element = []<std::size_t N>(auto const& arr) { return arr[N]; };
nth_element.template operator()<2>(std::array{10, 20, 30, 40});  // 30

// Variadic template lambda
auto print_all = []<typename... Ts>(Ts&&... args) {
    (std::cout << ... << args);
};
```

## Study Checklist
- [ ] Access the deduced type `T` directly in the lambda body without `decltype`
- [ ] Add a `requires` clause to restrict a template lambda to integral types
- [ ] Write a lambda that accepts `std::array<T, N>` and uses both `T` and `N`
- [ ] Compare compile-error quality: `auto` param vs template lambda constraint violation

## References
- [cppreference — Lambda expressions (C++20)](https://en.cppreference.com/w/cpp/language/lambda)
