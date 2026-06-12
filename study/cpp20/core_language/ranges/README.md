# C++20 › Ranges

## Overview
The Ranges library (`<ranges>`) provides a composable, lazy, pipeline-based
approach to sequence processing, replacing imperative iterator-pair algorithms
with declarative, chainable views.

## Core Vocabulary

| Term | Meaning |
|------|---------|
| **Range** | Anything with `begin()` + `end()` |
| **View** | Lazy, non-owning range adaptor; O(1) construction |
| **Range algorithm** | Algorithm in `std::ranges::` that accepts ranges, not iterator pairs |
| **Pipe operator `\|`** | Chains views: `range \| view1 \| view2` |

## Range Algorithms
```cpp
#include <algorithm>
#include <ranges>

std::vector<int> v{3, 1, 4, 1, 5, 9, 2, 6};

// Ranges versions accept containers directly
std::ranges::sort(v);
auto it = std::ranges::find(v, 5);
```

## Views
```cpp
namespace rv = std::ranges::views;  // or std::views (C++20)

auto result = v
    | rv::filter([](int x){ return x % 2 == 0; })
    | rv::transform([](int x){ return x * x; })
    | rv::take(3);

for (int x : result) std::cout << x << " ";
```
Views are **lazy** — no work is done until iteration.

## Key Views
`filter`, `transform`, `take`, `drop`, `reverse`, `join`, `split`,
`enumerate` (C++23), `zip` (C++23), `iota`, `keys`, `values`

## Study Checklist
- [ ] Replace a manual `for` + `if` + `push_back` with `filter` + `transform`
- [ ] Use `rv::iota` to generate a sequence without a container
- [ ] Compose 3 views in a pipeline and iterate lazily
- [ ] Write a custom view adaptor by inheriting from `std::ranges::view_base`
- [ ] Compare performance of ranges pipeline vs equivalent hand-written loop

## References
- [cppreference — Ranges library](https://en.cppreference.com/w/cpp/ranges)
