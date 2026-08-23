# C++23 › `std::ranges::fold_left` and Friends

## Overview
C++23 (**P2322**) adds proper *fold* (reduction) algorithms to the ranges
library. C++20 had `std::accumulate` (non-range) and `std::reduce`
(parallelism-focused), but no range-native fold. The new family plugs cleanly
into range pipelines and returns the *fold result type* rather than the initial
value's type.

## The Family

| Algorithm | Signature (essence) | Description |
|---|---|---|
| `fold_left(r, init, op)` | `U`  where `U = decay(op(init, r[0]))` | Left-associative fold with an initial value |
| `fold_left_first(r, op)` | `optional<T>` | Left fold using the first element as init; `nullopt` on empty |
| `fold_right(r, init, op)` | `U` | Right-associative fold with an initial value |
| `fold_right_last(r, op)` | `optional<T>` | Right fold using the last element as init |
| `fold_left_with_iter(r, init, op)` | `{ iter, U }` | Returns end iterator alongside the result |
| `fold_left_first_with_iter(r, op)` | `{ iter, optional<T> }` | Same for `_first` variant |

All live in `<algorithm>` under `std::ranges`.

## Basic Examples
```cpp
#include <algorithm>
#include <ranges>

std::vector<int> v{1, 2, 3, 4};

int sum      = std::ranges::fold_left(v, 0, std::plus{});           // 10
int product  = std::ranges::fold_left(v, 1, std::multiplies{});     // 24
auto max_opt = std::ranges::fold_left_first(v, std::ranges::max);   // optional<int>{4}

auto concat = words
            | std::ranges::to<std::vector<std::string>>();  // suppose
std::string s = std::ranges::fold_left(concat, std::string{}, std::plus{});
```

## Why This Is Better Than `std::accumulate`

### Result type follows the operator
```cpp
std::vector<int> v = {1, 2, 3};

// std::accumulate: init is `0` (int) → result is int, floats truncated
auto bad  = std::accumulate(v.begin(), v.end(), 0, [](auto a, int b){ return a + 0.5 + b; });

// std::ranges::fold_left: result deduced from the operator's return type
auto good = std::ranges::fold_left(v, 0, [](auto a, int b){ return a + 0.5 + b; });
static_assert(std::same_as<decltype(good), double>);
```

### Composable with views
```cpp
double avg = std::ranges::fold_left(
    v | std::views::transform([](int x){ return double(x); }),
    0.0, std::plus{}) / v.size();
```

### Proper empty-range semantics
`fold_left_first` returns `std::optional` — no undefined behaviour on empty
input, unlike calling `min_element` and dereferencing.

## Right Folds
```cpp
// Build a right-nested pair chain: (1,(2,(3,init)))
auto tree = std::ranges::fold_right(v, std::string{"nil"},
    [](int x, auto acc){ return "(" + std::to_string(x) + "," + acc + ")"; });
// tree == "(1,(2,(3,nil)))"
```

## Study Checklist
- [ ] Rewrite `std::accumulate` calls using `std::ranges::fold_left`
- [ ] Show a case where the *result type* differs meaningfully between the two
- [ ] Use `fold_left_first` on a potentially empty range
- [ ] Compare `fold_left` vs `fold_right` for a non-associative operator (e.g. subtraction)
- [ ] Fold a `views::transform` pipeline without materialising it first

## References
- [P2322 — `ranges::fold`](https://wg21.link/P2322)
- [cppreference — `std::ranges::fold_left`](https://en.cppreference.com/w/cpp/algorithm/ranges/fold_left)
