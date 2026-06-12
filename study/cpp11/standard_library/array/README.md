# C++11 › `std::array<T, N>`

## Overview
`std::array<T, N>` wraps a C-style array with the full STL container interface
(iterators, `size()`, `at()`, range algorithms) while retaining **stack allocation**
and **zero overhead** compared to a raw array.

## vs C Array
```cpp
int c_arr[4] = {1, 2, 3, 4};           // decays to pointer, no .size()
std::array<int, 4> arr{1, 2, 3, 4};    // full container, no decay

arr.size();       // 4 — compile-time constant
arr.at(10);       // throws std::out_of_range
arr[2];           // unchecked, O(1)
arr.data();       // raw pointer when C API is needed
```

## Key Properties
- Stack-allocated — no heap overhead.
- `N` must be a compile-time constant.
- Does **not** decay to a pointer when passed to a function — preserves size info.
- Supports structured bindings (C++17): `auto [a, b, c, d] = arr;`
- Supports `constexpr` construction and access.

## Study Checklist
- [ ] Sort a `std::array<int, 8>` with `std::sort(arr.begin(), arr.end())`
- [ ] Pass `std::array` by reference to a function; observe how size info is preserved
- [ ] Use `std::get<I>(arr)` for compile-time indexed access
- [ ] Initialise and use a `constexpr std::array` as a lookup table

## References
- [cppreference — array](https://en.cppreference.com/w/cpp/container/array)
