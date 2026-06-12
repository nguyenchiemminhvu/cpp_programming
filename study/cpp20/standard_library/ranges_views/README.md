# C++20 › `std::ranges::views` — Composable View Adaptors

## Overview
`std::ranges::views` (alias `std::views`) provides a rich library of lazy view
adaptors that compose via `|` to form expressive, zero-overhead data pipelines.

## Key Views
```cpp
namespace rv = std::ranges::views;

auto result = data
    | rv::filter([](int x){ return x % 2 == 0; })
    | rv::transform([](int x){ return x * x; })
    | rv::take(5)
    | rv::reverse;

for (int x : result) std::cout << x << " ";
```

## View Reference
| View | Effect |
|------|--------|
| `rv::filter(pred)` | Keep elements satisfying `pred` |
| `rv::transform(fn)` | Apply `fn` to each element |
| `rv::take(n)` | First `n` elements |
| `rv::drop(n)` | Skip first `n` elements |
| `rv::reverse` | Reverse iteration |
| `rv::iota(a, b)` | Generate `[a, b)` |
| `rv::join` | Flatten nested ranges |
| `rv::split(delim)` | Split on delimiter |
| `rv::keys` / `rv::values` | Map key/value views |

## Lazy Evaluation
Views are **lazy** — no work happens until iteration. Constructing a pipeline
is O(1); elements are produced on-demand.

## Study Checklist
- [ ] Replace a manual `for + if + push_back` with a `filter | transform` pipeline
- [ ] Use `rv::iota` + `rv::transform` to generate a mathematical sequence
- [ ] Compose 4 views and iterate; verify laziness with a counting transform
- [ ] Write a custom range view by inheriting from `std::ranges::view_base`

## References
- [cppreference — Ranges library](https://en.cppreference.com/w/cpp/ranges)
