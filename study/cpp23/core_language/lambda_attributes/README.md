# C++23 › Attributes on Lambdas

## Overview
C++23 (**P2173**) allows attributes on the **call operator** of a lambda,
placed after the parameter list. Previously, attributes on a lambda applied
to the *type* of the closure (rarely useful) — now they apply to the
generated `operator()` and behave like attributes on a normal function.

## Syntax
```cpp
auto parse = [](std::string_view s) [[nodiscard]] -> std::optional<int> {
    // ...
    return std::stoi(std::string{s});
};

auto handler = [] [[noreturn]] (int code) {
    std::exit(code);
};
```
For a lambda without parameters, the parameter list is required to place
attributes:
```cpp
auto stop = []() [[noreturn]] { std::terminate(); };
```

## Supported Attributes (typical use cases)
| Attribute | Effect on lambda |
|---|---|
| `[[nodiscard]]` | Warn if the return value is ignored |
| `[[noreturn]]` | Mark the call operator as never returning |
| `[[deprecated("…")]]` | Warn on invocation |
| `[[gnu::hot]]`, `[[msvc::forceinline]]` | Vendor-specific hints on the operator |

## Placement Rules
```
[capture]<template-params>(params) specifiers [[attributes]] -> ret { body }
```
Attributes go **after** any specifiers (`mutable`, `constexpr`, `consteval`,
`static`, `noexcept`) and **before** the trailing return type.

## Pitfall — Attribute Before Parameter List
Historic syntax `[] [[…]] () { … }` attaches the attribute to the closure
**type**. That is different from C++23's placement and rarely useful.

## Study Checklist
- [ ] Attach `[[nodiscard]]` to a validator lambda and confirm the warning fires
- [ ] Write a `[[noreturn]]` failure-report lambda used inside a switch
- [ ] Verify the placement order relative to `mutable` and `-> ret`
- [ ] Compare with the older attribute-on-closure-type syntax

## References
- [P2173 — Attributes on lambda expressions](https://wg21.link/P2173)
- [cppreference — lambda expressions](https://en.cppreference.com/w/cpp/language/lambda)
