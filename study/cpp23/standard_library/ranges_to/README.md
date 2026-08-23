# C++23 › `std::ranges::to<Container>`

## Overview
C++23 (**P1206**) adds `std::ranges::to<Container>` — the *materialiser* that
was conspicuously missing from C++20 ranges. Feed it any range and get a
container of the requested type.

## Basic Usage
```cpp
#include <ranges>
#include <vector>
#include <string>

auto squares = std::views::iota(1, 6)
             | std::views::transform([](int i){ return i * i; })
             | std::ranges::to<std::vector<int>>();
//                              ^^^^^^^^^^^^^^^ target container type
```
Every range-based pipeline can now end in a single expression that produces an
owning container.

## Available Overloads

### Fully specified target type
```cpp
auto v = rng | std::ranges::to<std::vector<int>>();
```

### Container template with deduced element type
```cpp
auto v = rng | std::ranges::to<std::vector>();      // deduces vector<int>
auto s = rng | std::ranges::to<std::set>();
```
Uses the range's value type.

### Free-function form
```cpp
auto v = std::ranges::to<std::vector>(rng);          // equivalent, no pipe
```

### Extra constructor arguments
```cpp
auto v = rng | std::ranges::to<std::vector<int>>(alloc);   // pass allocator
auto s = rng | std::ranges::to<std::set<int, std::greater<>>>();
```

## Nested Containers
Works recursively when the elements are themselves ranges:
```cpp
auto rows_of_words = lines
    | std::views::transform([](auto&& l){ return split(l, ' '); })
    | std::ranges::to<std::vector<std::vector<std::string>>>();
```
Each inner range is also materialised.

## Associative & Ordered Containers
```cpp
auto by_id = people
           | std::views::transform([](auto&& p){ return std::pair{p.id, p}; })
           | std::ranges::to<std::flat_map<int, person>>();
```

## Implementation Notes (why it's smart)
`ranges::to` prefers, in order:
1. Direct construction from `(begin, end)`.
2. Direct construction from the range object (tag-dispatch through
   `from_range_t`).
3. Default-construct then `insert` / `push_back` per element.
4. Reserve when the size is known — no unnecessary reallocations.

## Pitfalls
- `views::filter` produces a range with an **input** category; you cannot
  make a `std::array` from an unknown-size range.
- Materialising a very large range copies every element — sometimes what you
  wanted was a *view*, not a container. Use `to` only when ownership is needed.

## Study Checklist
- [ ] Materialise a `views::iota | transform | filter` pipeline into a `std::vector`
- [ ] Convert a range of pairs into a `std::map` / `std::flat_map`
- [ ] Pass an allocator through `ranges::to`
- [ ] Materialise nested ranges into a `vector<vector<T>>`
- [ ] Compare with the pre-C++23 pattern `vector v(rng.begin(), rng.end())`

## References
- [P1206 — `ranges::to`: a function to convert any range to a container](https://wg21.link/P1206)
- [cppreference — `std::ranges::to`](https://en.cppreference.com/w/cpp/ranges/to)
