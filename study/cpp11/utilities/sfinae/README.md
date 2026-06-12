# C++11 › SFINAE — Substitution Failure Is Not An Error

## Overview
SFINAE is a template substitution rule: when template argument substitution
produces an ill-formed type/expression, the specialization is silently discarded
rather than causing a compilation error.

## `std::enable_if` Pattern
```cpp
// Enable only for integral types
template<typename T,
         typename = std::enable_if_t<std::is_integral_v<T>>>
T square(T x) { return x * x; }

// Two-overload dispatch
template<typename T>
auto process(T x) -> std::enable_if_t<std::is_integral_v<T>,   T> { return x * 2; }
template<typename T>
auto process(T x) -> std::enable_if_t<std::is_floating_point_v<T>, T> { return x * 1.5; }
```

## Detection Idiom (`std::void_t`)
```cpp
template<typename T, typename = void>
struct has_serialize : std::false_type {};

template<typename T>
struct has_serialize<T, std::void_t<decltype(std::declval<T>().serialize())>>
    : std::true_type {};
```

## SFINAE vs Modern Alternatives
| Technique | Standard | Readability | Error messages |
|-----------|----------|-------------|----------------|
| SFINAE | C++11 | Poor | Cryptic |
| `if constexpr` | C++17 | Good | Clear |
| Concepts | C++20 | Excellent | Excellent |

## Study Checklist
- [ ] Write 3 overloads using `enable_if` for integral, float, and other types
- [ ] Implement `has_begin<T>` using the detection idiom with `void_t`
- [ ] Show the compiler error when SFINAE fails on all overloads
- [ ] Refactor an `enable_if` function to `if constexpr`, then to a concept

## References
- [cppreference — SFINAE](https://en.cppreference.com/w/cpp/language/sfinae)
