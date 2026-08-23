# C++23 › Static `operator()` and Static `operator[]`

## Overview
- **P1169** — `operator()` may be `static`
- **P2589** — `operator[]` may be `static`

Prior to C++23, every call to a functor cost one implicit `this` pointer,
even when the functor was **stateless**. For stateless algorithm objects
(e.g. `std::ranges::less`), the pointer is dead weight — the compiler cannot
always optimise it away, especially across ABI boundaries. C++23 lets you
declare the call operator `static`, matching the semantics of a free function.

## Syntax
```cpp
struct less_by_abs {
    static bool operator()(int a, int b) noexcept {
        return std::abs(a) < std::abs(b);
    }
};

std::ranges::sort(vec, less_by_abs{});   // still callable as an object
```
Static operators may be called with an object *or* through the class:
```cpp
less_by_abs{}(1, -2);   // OK
less_by_abs::operator()(1, -2);   // OK
```

## Rules
- A static `operator()` / `operator[]` **cannot** be `const`, `virtual`,
  or ref-qualified.
- The class must not also declare a non-static `operator()` — no overloading
  between static and non-static call operators.
- Lambdas: C++23 also allows `[]() static { … }` for stateless lambdas.
  ```cpp
  auto f = []() static { return 42; };
  ```

## Why This Matters
Standard library function objects (`std::ranges::less`, `std::ranges::plus`, …)
are the primary beneficiaries. In performance-critical generic code, a
`static operator()` removes the hidden `this` argument from the calling
convention, enabling better inlining and register allocation.

## Static Lambda Restrictions
- No captures allowed (`static` implies stateless).
- Cannot be combined with `mutable` (nothing to mutate).

## Study Checklist
- [ ] Convert a stateless functor into one with `static operator()`
- [ ] Verify the generated assembly no longer passes an implicit `this`
- [ ] Write a `static operator[]` on a lookup-table type
- [ ] Try `auto f = []() static { … };` and confirm captures are rejected
- [ ] Contrast with regular non-static `operator()` — when *must* it stay non-static?

## References
- [P1169 — `static operator()`](https://wg21.link/P1169)
- [P2589 — `static operator[]`](https://wg21.link/P2589)
- [cppreference — function call operator](https://en.cppreference.com/w/cpp/language/operators#Function_call_operator)
