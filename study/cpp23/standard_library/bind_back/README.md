# C++23 › `std::bind_back`

## Overview
C++23 (**P2387**) adds `std::bind_back(f, args…)` in `<functional>` — the
symmetric partner of C++20's `std::bind_front`. It binds the **last** N
arguments of a callable, producing a new callable that forwards the remaining
arguments to the front.

## Signature
```cpp
namespace std {
    template <class F, class... Args>
    constexpr /*unspecified*/ bind_back(F&& f, Args&&... args);
}
```
The result is a callable that, when invoked as `g(x1, x2, ...)`, calls
`f(x1, x2, ..., args...)`.

## Basic Example
```cpp
auto divide = [](double a, double b){ return a / b; };
auto half   = std::bind_back(divide, 2.0);      // divides its argument by 2
double x    = half(7.0);                        // 3.5
```
Compare with `bind_front`:
```cpp
auto reciprocal = std::bind_front(divide, 1.0); // 1.0 divided by its argument
double y        = reciprocal(4.0);              // 0.25
```

## Idiomatic Use With Ranges
`bind_back` is designed to feed range algorithms whose predicates take the
element as the *first* argument:

```cpp
auto lt = [](int a, int b){ return a < b; };

// Take all elements less than 10 — but the predicate here takes (element, bound).
auto keep = v | std::views::filter(std::bind_back(lt, 10));
```

## `bind_front` vs `bind_back`

| Idiom | Call semantics |
|---|---|
| `bind_front(f, a, b)` | `g(x, y) → f(a, b, x, y)` |
| `bind_back(f, a, b)`  | `g(x, y) → f(x, y, a, b)` |

Neither generalises argument reordering — for that use a lambda.

## Move-Only and Perfect Forwarding
- `bind_back` **stores its bound arguments by value** (decayed), so an lvalue
  argument is copied and an rvalue is moved.
- The resulting callable is movable if all captures are movable.
- Invocation forwards the caller's arguments perfectly.

## Comparison With `std::bind`
`std::bind` (C++11) predates lambdas and is discouraged in modern code:
- `std::bind` supports placeholders and reordering — very flexible but slow to
  compile and error-prone.
- `bind_front` / `bind_back` support only the common case (bind a prefix/suffix)
  and compile fast.
- Everything else → use a lambda.

## `constexpr`
Both `bind_front` and `bind_back` are `constexpr` — usable in constant
expressions.

## Study Checklist
- [ ] Rewrite a lambda `[x](auto y){ return f(y, x); }` as `bind_back(f, x)`
- [ ] Use `bind_back` in `views::filter` / `views::transform`
- [ ] Combine `bind_front` and `bind_back` on the same callable
- [ ] Verify the bound argument is *moved* when an rvalue is passed
- [ ] Compare compile time and codegen with an equivalent lambda

## References
- [P2387 — Pipe support for user-defined range adaptors + `bind_back`](https://wg21.link/P2387)
- [cppreference — `std::bind_back`](https://en.cppreference.com/w/cpp/utility/functional/bind_front)
