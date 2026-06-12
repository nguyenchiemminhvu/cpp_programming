# C++17 › Fold Expressions

## Overview
Fold expressions apply a binary operator across a parameter pack in a single
expression, replacing the C++11 recursive template pattern with a one-liner.

## Syntax (four forms)
| Form | Expansion | |
|------|-----------|---|
| `(pack op ...)` | `e1 op (e2 op e3)` | Unary right fold |
| `(... op pack)` | `(e1 op e2) op e3` | Unary left fold |
| `(pack op ... op init)` | with initial value | Binary right fold |
| `(init op ... op pack)` | with initial value | Binary left fold |

## Examples
```cpp
template<typename... Ts>
auto sum(Ts... vals) { return (vals + ...); }

template<typename... Ts>
void print(Ts... vals) { (std::cout << ... << vals); }  // left fold over <<

template<typename... Ts>
bool all_positive(Ts... vals) { return ((vals > 0) && ...); }

// Comma-fold — call a function for each element
template<typename F, typename... Ts>
void for_each(F f, Ts... vals) { (f(vals), ...); }
```

## Study Checklist
- [ ] Implement `sum`, `product`, and `all_of` using fold expressions
- [ ] Show `(pack - ...)` vs `(... - pack)` produce different results for subtraction
- [ ] Use `,`-operator fold to call a side-effecting function per pack element
- [ ] Compare C++11 recursive variadic vs C++17 fold for a `print` function

## References
- [cppreference — Fold expressions](https://en.cppreference.com/w/cpp/language/fold)
