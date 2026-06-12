# C++17 › Evaluation Order Guarantees

## Overview
C++17 tightens evaluation order rules to prevent undefined behaviour in common
expression patterns that were previously UB or unspecified in C++14.

## Key C++17 Guarantees

### 1. Postfix expressions: left-to-right
```cpp
f(args...)    // f is fully evaluated before any argument
obj.method()  // obj evaluated before method call
ptr->method() // ptr evaluated first
```

### 2. Assignment: right-to-left
```cpp
a = b         // b is evaluated first, then assigned to a
```

### 3. Shift operators: left-to-right
```cpp
s << a << b   // a evaluated before b
```

## What Was UB Before C++17
```cpp
// C++14: undefined behaviour
std::map<int,int> m;
m[0] = m.size();   // was UB; C++17: defined — right side evaluated first

// C++14: unspecified
std::string s = "a";
s += s;            // C++17: defined behaviour
```

## Still Unspecified in C++17
- **Function argument evaluation order** remains unspecified (but not UB for chained calls).
- `a + b`: left or right first is still unspecified.
- `f(++i, i)` is still UB (same variable modified + read without sequencing).

## Study Checklist
- [ ] Show that `m[k] = m.size()` is defined in C++17 but was UB before
- [ ] Demonstrate safe chained `<<` with `std::cout << f() << g()`
- [ ] Show which expression patterns are still unordered in C++17
- [ ] Write a test that catches evaluation-order UB with AddressSanitizer

## References
- [cppreference — Order of evaluation](https://en.cppreference.com/w/cpp/language/eval_order)
