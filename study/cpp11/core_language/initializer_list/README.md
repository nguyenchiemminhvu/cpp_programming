# C++11 › `std::initializer_list<T>`

## Overview
`std::initializer_list<T>` is a lightweight view over a compiler-generated
array of `T` created by a brace-enclosed list. It enables containers and
functions to accept `{1, 2, 3}` syntax uniformly.

## Core Usage
```cpp
// Constructor
std::vector<int> v{1, 2, 3, 4};   // calls vector(initializer_list<int>)

// Function parameter
void print(std::initializer_list<int> vals) {
    for (int x : vals) std::cout << x << " ";
}
print({10, 20, 30});
```

## Important Rules
1. `initializer_list` is a **view** — it does not own the array.
2. If a constructor taking `initializer_list<T>` exists, `{…}` **always** prefers
   it over other constructors — even when it requires narrowing.
   ```cpp
   std::vector<int>{1, 2}  // 2-element vector {1, 2}
   std::vector<int>(1, 2)  // 1-element vector {2}  ← different!
   ```
3. `auto x = {1, 2, 3};` deduces `std::initializer_list<int>` (C++11/14 trap).

## Study Checklist
- [ ] Add an `initializer_list` constructor to a custom `Stack<T>` class
- [ ] Show the constructor preference rule: `vector<int>{1, 2}` vs `vector<int>(1, 2)`
- [ ] Demonstrate the `auto` deduction trap: `auto x = {1};` is not `int`
- [ ] Iterate an `initializer_list` parameter with a range-for

## References
- [cppreference — initializer_list](https://en.cppreference.com/w/cpp/utility/initializer_list)
- *Effective Modern C++*, Scott Meyers — Item 7
