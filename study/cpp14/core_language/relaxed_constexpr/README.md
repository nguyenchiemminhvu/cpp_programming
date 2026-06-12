# C++14 › Relaxed `constexpr` Functions

## Overview
C++14 lifts the severe C++11 restriction that a `constexpr` function body
must be a single `return` statement. Functions can now contain most ordinary
C++ constructs as long as they can be evaluated at compile time.

## What C++14 Unlocks

| Feature | C++11 | C++14 |
|---------|-------|-------|
| Multiple statements | No | Yes |
| Local variables | No | Yes |
| `if` / `switch` | No | Yes |
| Loops (`for`, `while`) | No | Yes |
| Multiple `return` statements | No | Yes |
| Mutation of local variables | No | Yes |

Still **not** allowed in `constexpr` (until C++20): `try`/`catch`, `goto`, most `new`/`delete`.

## Example
```cpp
// C++11: forced into single-return recursion
constexpr int factorial11(int n) {
    return n <= 1 ? 1 : n * factorial11(n - 1);
}

// C++14: natural iterative version
constexpr int factorial14(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i)
        result *= i;
    return result;
}
static_assert(factorial14(6) == 720);
```

## Study Checklist
- [ ] Rewrite a recursive C++11 `constexpr` as an iterative C++14 version
- [ ] Implement compile-time string length with a `for` loop
- [ ] Build a compile-time lookup table using `constexpr` and `std::array`
- [ ] Show that calling a `constexpr` function with a non-const argument still works at run time

## References
- [cppreference — constexpr](https://en.cppreference.com/w/cpp/language/constexpr)
