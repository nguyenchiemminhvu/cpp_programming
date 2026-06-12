# C++11 › `constexpr` — Compile-Time Evaluation

## Overview
`constexpr` marks functions and variables as candidates for evaluation at
compile time, enabling zero-cost constants and compile-time computation
that previously required template metaprogramming.

## C++11 Rules (strict)
- A `constexpr` function body must be a **single `return` statement**.
- No local variables, loops, or multiple statements (relaxed in C++14).
- All arguments must themselves be constant expressions when used in a constant context.

## Vocabulary

| Construct | Meaning |
|-----------|---------|
| `constexpr int N = 42;` | Compile-time integer constant |
| `constexpr int square(int x) { return x * x; }` | Compile-time-eligible function |
| `static_assert(square(5) == 25)` | Proves evaluation happens at compile time |
| `constexpr` vs `const` | `const` = cannot be changed; `constexpr` = evaluated at compile time (also implies `const` for variables) |

## Use Cases
1. Array sizes: `int arr[square(4)];`
2. Template non-type arguments: `std::array<int, square(4)>`
3. Replacing `#define` magic numbers with typed, scoped constants

## Study Checklist
- [ ] Write a `constexpr` factorial function (using C++11 recursion)
- [ ] Use the result as a template argument and array size
- [ ] Compare generated assembly: `constexpr` function vs run-time function
- [ ] Explain why `constexpr` does NOT guarantee compile-time evaluation in all contexts

## References
- [cppreference — constexpr](https://en.cppreference.com/w/cpp/language/constexpr)
- *Effective Modern C++*, Scott Meyers — Item 15
