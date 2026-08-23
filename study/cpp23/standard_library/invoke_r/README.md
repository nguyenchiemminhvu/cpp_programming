# C++23 › `std::invoke_r`

## Overview
C++23 (**P2136**) adds `std::invoke_r<R>(f, args…)` in `<functional>` — a
version of `std::invoke` that **explicitly specifies the return type** and
performs an implicit conversion of the result to `R`.

## Signature
```cpp
namespace std {
    template <class R, class F, class... Args>
    constexpr R invoke_r(F&& f, Args&&... args)
        noexcept(std::is_nothrow_invocable_r_v<R, F, Args...>);
}
```

## Basic Usage
```cpp
double sum(int a, int b) { return a + b; }

int x = std::invoke_r<int>(sum, 1, 2);      // narrows double → int
void v = std::invoke_r<void>(sum, 1, 2);    // discards the result
```
The important cases are `R = void` (discard any return value) and `R = SomeBase`
(implicit derived-to-base conversion).

## Why Not Just Cast?
```cpp
static_cast<R>(std::invoke(f, args…));
```
Almost right, but:
- Doesn't handle `R = void` uniformly (a `void` static_cast of a `void`
  expression is fine, but templated code needs an ugly `if constexpr`).
- Doesn't participate in overload resolution — a caller that expects
  `is_invocable_r_v<R, F, Args…>` semantics may accept `f` whose return type
  is convertible to `R`, and `invoke_r` codifies that contract.

## Concept Companion — `std::invocable_r`
`std::invoke_r` pairs with `std::invocable_r<R, F, Args…>` (a `<concepts>`
extension in C++23) that checks whether the callable can be invoked and its
result implicitly converted to `R`.

```cpp
template <std::invocable_r<int, int, int> F>
int reduce_pair(F f, int a, int b) {
    return std::invoke_r<int>(f, a, b);
}
```

## Common Uses

### Type-erased callbacks with a fixed return type
```cpp
struct sink { std::move_only_function<int(std::string_view)> emit; };

sink s{ .emit = [](std::string_view x) -> long { return x.size(); } };
int n = std::invoke_r<int>(s.emit, "hello");   // long → int
```

### `std::apply`-style with explicit return
```cpp
auto result = std::invoke_r<double>(std::plus{}, 1, 2);   // 3.0 double
```

### Void result adaption
```cpp
template <class F, class... A>
void run_and_ignore(F&& f, A&&... a) {
    std::invoke_r<void>(std::forward<F>(f), std::forward<A>(a)...);
}
```

## `constexpr`, `noexcept`
- `constexpr` when the underlying operation is.
- `noexcept` iff `is_nothrow_invocable_r_v<R, F, Args…>`.

## Study Checklist
- [ ] Replace `static_cast<R>(std::invoke(…))` idioms with `std::invoke_r<R>`
- [ ] Use `std::invoke_r<void>` to discard a callable's return
- [ ] Constrain a template with `std::invocable_r`
- [ ] Confirm implicit conversions are applied (int → double, derived → base)
- [ ] Verify `noexcept` propagation

## References
- [P2136 — invoke_r](https://wg21.link/P2136)
- [cppreference — `std::invoke_r`](https://en.cppreference.com/w/cpp/utility/functional/invoke)
