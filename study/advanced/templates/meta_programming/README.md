````markdown
# Advanced › Template Meta Programming

## Overview
Template Meta Programming (TMP) uses the C++ template system as a
**compile-time computation engine**. Types and constant values become the
"data", and template instantiation becomes the "execution". The compiler
evaluates the program before runtime, producing zero-overhead abstractions,
type-safe generic code, and static assertions.

## Core Ideas
- **Types as values** — `int`, `std::vector<T>`, `void(*)()` are all
  first-class citizens at compile time.
- **Instantiation as evaluation** — specialising a template *is* a function call.
- **Recursion instead of loops** — recursive instantiation until a base
  specialisation stops it.
- **Branching via specialisation / SFINAE / `if constexpr`**.

## Classic Compile-Time Computation
```cpp
// Factorial via recursive instantiation
template<unsigned N>
struct Factorial { static constexpr unsigned value = N * Factorial<N - 1>::value; };

template<>
struct Factorial<0> { static constexpr unsigned value = 1; };

static_assert(Factorial<5>::value == 120);
```

## Type Traits (Introspection)
```cpp
template<typename T> struct is_pointer            : std::false_type {};
template<typename T> struct is_pointer<T*>        : std::true_type  {};

template<typename T> struct remove_const          { using type = T; };
template<typename T> struct remove_const<const T> { using type = T; };
```
- Prefer inheriting from `std::true_type` / `std::false_type`.
- Provide `_t` and `_v` aliases (C++14/17) to reduce `typename ::type` noise.

## SFINAE — Substitution Failure Is Not An Error
```cpp
template<typename T,
         typename = std::enable_if_t<std::is_integral_v<T>>>
T half(T x) { return x / 2; }
```
Ill-formed substitutions **remove** a candidate instead of causing a hard
error, enabling overload selection based on type properties.

## Modern Alternatives (C++17 / 20)
- `if constexpr` — compile-time branching inside a single function.
- **Concepts** — replace verbose SFINAE with readable constraints.
- `constexpr` / `consteval` functions — imperative compile-time code.
- Fold expressions — variadic reduction without recursion.

```cpp
template<typename... Ts>
constexpr auto sum(Ts... xs) { return (xs + ... + 0); }   // fold expression

template<std::integral T>                                 // concept
constexpr T square(T x) { return x * x; }
```

## TypeList Pattern
```cpp
template<typename...> struct TypeList {};

template<typename List> struct Length;
template<typename... Ts>
struct Length<TypeList<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)> {};
```

## Guidelines
- Prefer **`constexpr`** functions over recursive struct templates.
- Prefer **concepts** over `enable_if` when C++20 is available.
- Keep TMP code isolated behind clean aliases (`_t`, `_v`) — users should
  never see the machinery.
- Watch for **template instantiation depth** and **compile-time cost**.
- Diagnostics matter: use `static_assert` with clear messages.

## Study Checklist
- [ ] Implement `Fibonacci<N>` two ways: recursive template and `constexpr` function
- [ ] Write `IsOneOf<T, Ts...>` returning `true` if `T` matches any of `Ts...`
- [ ] Reimplement `std::conditional_t` from scratch
- [ ] Convert an `enable_if`-based API to a C++20 concept-based API
- [ ] Build a compile-time `TypeList` with `PushBack`, `Front`, `Contains`
- [ ] Measure compile-time cost of recursive vs fold-expression versions

## References
- [cppreference — Template metaprogramming](https://en.cppreference.com/w/cpp/language/templates)
- [cppreference — SFINAE](https://en.cppreference.com/w/cpp/language/sfinae)
- [cppreference — Type traits](https://en.cppreference.com/w/cpp/header/type_traits)
- [cppreference — Constraints and concepts](https://en.cppreference.com/w/cpp/language/constraints)

````

