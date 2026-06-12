# C++11 › Range-Based `for` Loop

## Overview
`for (declaration : range)` iterates over any range — arrays, STL containers,
initializer-lists, or custom types providing `begin()` / `end()` — without manual
iterator bookkeeping.

## Syntax
```cpp
for (auto& element : container)      { … }  // by reference — no copy, allows mutation
for (const auto& element : container) { … }  // const reference — no copy, read-only
for (auto element : container)        { … }  // copy per element
for (int x : {1, 2, 3})              { … }  // temporary initializer-list range
```

## Rules
- The range expression is evaluated **once** before the loop begins.
- Modifying the container's **size** (insert/erase) inside the loop is undefined behaviour.
- Custom types must provide `begin()` and `end()` (free or member functions).
- `auto&&` (forwarding reference) is the most general — works with proxied containers too.

## Study Checklist
- [ ] Iterate a `std::vector<int>` using `const auto&`; verify no copies occur
- [ ] Mutate elements via `auto&` and observe the effect on the original container
- [ ] Iterate a C-style array: `int arr[5]; for (auto x : arr)`
- [ ] Write a minimal custom range type (provide `begin()`/`end()` free functions)
- [ ] Demonstrate the UB of erasing inside a range-for; fix with index-based loop

## References
- [cppreference — Range-for](https://en.cppreference.com/w/cpp/language/range-for)
