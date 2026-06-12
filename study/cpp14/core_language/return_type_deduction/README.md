# C++14 › Return Type Deduction for Normal Functions

## Overview
C++14 extends `auto` return-type deduction from lambdas to regular functions,
eliminating the need for trailing return types in most cases.

## Syntax
```cpp
// C++11 trailing return (still valid and sometimes necessary)
auto multiply(int a, int b) -> int { return a * b; }

// C++14 deduced return type
auto multiply(int a, int b) { return a * b; }  // deduced: int

// Works for templates too
template<typename T, typename U>
auto add(T a, U b) { return a + b; }  // deduced: decltype(a + b)
```

## Rules
1. All `return` statements in the function must deduce to the **same type**.
2. The function body must be **visible** to the call site for deduction to work (inline in headers or fully defined before use).
3. Recursive functions need at least one non-recursive `return` before the recursive one.
4. Use `decltype(auto)` to preserve references and cv-qualifiers (unlike plain `auto`).

## `auto` vs `decltype(auto)`
```cpp
int x = 0;
auto        f() { return x; }          // returns int (copy)
decltype(auto) g() { return (x); }     // returns int& (reference!)
```

## Study Checklist
- [ ] Convert several trailing-return-type functions to `auto`-deduced
- [ ] Show a compilation error when two `return` statements deduce different types
- [ ] Use `decltype(auto)` to return a reference from a getter template
- [ ] Write a recursive `auto`-return function (e.g., Fibonacci)

## References
- [cppreference — Function return type deduction](https://en.cppreference.com/w/cpp/language/function#Return_type_deduction)
