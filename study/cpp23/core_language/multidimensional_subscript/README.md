# C++23 › Multidimensional Subscript Operator

## Overview
C++23 (**P2128**) removes the ancient restriction that `operator[]` may take
**exactly one** argument. A user-defined `operator[]` may now take **any number
of arguments** (including zero), matching what `operator()` has always allowed.
The primary motivator is `std::mdspan` and general matrix / tensor libraries.

## Syntax
```cpp
struct matrix {
    std::vector<double> data;
    std::size_t rows, cols;

    double& operator[](std::size_t r, std::size_t c) {
        return data[r * cols + c];
    }
    double  operator[](std::size_t r, std::size_t c) const {
        return data[r * cols + c];
    }
};

matrix m{ .data = std::vector<double>(9), .rows = 3, .cols = 3 };
m[1, 2] = 42.0;                  // ✅ two-argument subscript
double x = m[1, 2];
```

## Key Rules
- The classic comma operator meaning inside `[]` is **deprecated** in C++20
  and **repurposed** in C++23. Parenthesise if you really need the comma
  operator: `a[(i, j)]` still calls the single-argument version with `j`.
- Works for zero-argument `operator[]()` too — useful for iterator-like
  proxies.
- Overloadable with any number of parameters, and may be templated / take
  packs:
  ```cpp
  template <typename... Is>
  auto& operator[](this auto&& self, Is... indices) { … }
  ```

## Integration With `std::mdspan`
```cpp
std::mdspan<double, std::extents<std::size_t, 3, 3>> view(m.data.data());
view[1, 2] = 3.14;   // native multi-index — no more view(1, 2) workaround
```

## Migration Concern
Pre-C++23 code sometimes wrote `arr[i, j]` **assuming** the comma operator,
producing `arr[j]`. Compilers now warn (`-Wcomma-subscript`) — the standard
requires this diagnostic. Fix by rewriting to `arr[(i, j)]` or
(preferably) refactor to a true multidimensional API.

## Study Checklist
- [ ] Write a `matrix<T, R, C>` with `operator[](std::size_t, std::size_t)`
- [ ] Add a deducing-`this` templated `operator[]` that supports any number of indices
- [ ] Reproduce the deprecation warning: `int a[3]; auto x = a[1, 2];`
- [ ] Compare row-major and column-major indexing in a single class
- [ ] Prototype a `std::mdspan`-style layout policy and use `[]` with it

## References
- [P2128 — Multidimensional subscript operator](https://wg21.link/P2128)
- [cppreference — subscript operator](https://en.cppreference.com/w/cpp/language/operator_member_access#Built-in_subscript_operator)
