# C++20 › Concepts

## Overview
Concepts are named boolean compile-time predicates that constrain template
parameters. They replace SFINAE + `enable_if` with readable, compiler-enforced
requirements and produce clear error messages.

## Syntax Forms
```cpp
// 1. Concept definition
template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

// 2. requires clause
template<typename T>
    requires Arithmetic<T>
T add(T a, T b) { return a + b; }

// 3. Abbreviated function template (terse syntax)
auto add(Arithmetic auto a, Arithmetic auto b) { return a + b; }

// 4. Concept in requires expression (ad-hoc)
template<typename T>
concept Printable = requires(T x) {
    { std::cout << x } -> std::same_as<std::ostream&>;
};
```

## Standard Concepts (`<concepts>`)
| Concept | Constraint |
|---------|-----------|
| `std::integral<T>` | Integer type |
| `std::floating_point<T>` | Float/double |
| `std::copyable<T>` | Copyable and movable |
| `std::invocable<F, Args…>` | Callable with given args |
| `std::ranges::range<T>` | Has `begin`/`end` |
| `std::totally_ordered<T>` | All comparison operators |

## `requires` Expressions
```cpp
template<typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};
```

## Study Checklist
- [ ] Write a `Sortable` concept and constrain `std::sort` wrapper
- [ ] Replace an `enable_if` overload set with concepts
- [ ] Compare compiler error messages: SFINAE vs concept violation
- [ ] Use a `requires` expression to check for a member function's existence
- [ ] Compose two concepts with `&&` and `||`

## References
- [cppreference — Constraints and concepts](https://en.cppreference.com/w/cpp/language/constraints)
