# C++11 › `decltype` — Declared Type

## Overview
`decltype(expr)` yields the **exact declared type** of an expression, including
references and cv-qualifiers, without evaluating it. Complements `auto` in
generic and template code.

## Key Concepts

| Expression | Result |
|------------|--------|
| `decltype(x)` where `x` is `int` | `int` |
| `decltype(x)` where `x` is `int&` | `int&` |
| `decltype((x))` (double parentheses) | `int&` — lvalue expression rule |
| `decltype(auto)` (C++14) | deduces like `decltype`, not like `auto` |

### Difference from `auto`
```
int x = 0;
auto      a = x;   // int   — drops reference
decltype(x) b = x; // int   — same here
auto&     c = x;   // int&
decltype((x)) d = x; // int& — lvalue expression
```

## Use Cases
1. **Trailing return types** — `auto f(T a, U b) -> decltype(a + b)`
2. **Generic forwarding** — preserve the value category of a forwarded expression
3. **Type inspection** — query types at compile time without `typeid`

## Study Checklist
- [ ] Predict the `decltype` for at least five different expressions
- [ ] Explain the difference between `decltype(x)` and `decltype((x))`
- [ ] Write a trailing-return-type function template using `decltype`
- [ ] Compare `auto` vs `decltype(auto)` return deduction (preview of C++14)

## References
- [cppreference — decltype](https://en.cppreference.com/w/cpp/language/decltype)
- *Effective Modern C++*, Scott Meyers — Item 3
