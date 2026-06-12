# C++17 › Inline Variables

## Overview
The `inline` specifier can now be applied to variables (not just functions),
allowing a variable to be defined in a header file and included in multiple
translation units without violating the One Definition Rule (ODR).

## The ODR Problem (Before C++17)
```cpp
// constants.h
const int MAX = 100;   // OK (internal linkage due to const)
int counter = 0;       // ODR violation if included in multiple .cpp files
```

## C++17 Solution
```cpp
// constants.h
inline int counter = 0;          // one definition, external linkage
inline constexpr int MAX = 100;  // preferred for constants
```

## `inline constexpr` — The Idiomatic C++17 Pattern
```cpp
// Before C++17: static member constant needed out-of-line definition in .cpp
struct Config {
    static const int BUF_SIZE = 256;  // declaration
    // config.cpp: const int Config::BUF_SIZE;  // required definition
};

// C++17: inline constexpr eliminates the .cpp entry
struct Config {
    static inline constexpr int BUF_SIZE = 256;
};
```

## Study Checklist
- [ ] Define a header-only counter using `inline int`; include it in two TUs and verify one instance
- [ ] Replace a `static const` member needing a separate `.cpp` definition with `inline constexpr`
- [ ] Compare `inline constexpr` with `constexpr` in a namespace (C++17: namespace-scope `constexpr` implies `inline`)
- [ ] Show the linker error that `inline` solves

## References
- [cppreference — inline specifier](https://en.cppreference.com/w/cpp/language/inline)
