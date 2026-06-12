# C++11 › Variadic Templates

## Overview
Variadic templates accept **zero or more** template arguments via a parameter
pack (`typename... Ts`), enabling type-safe heterogeneous containers, perfect
forwarding wrappers, and recursive metaprogramming.

## Syntax
```cpp
template<typename... Ts>      // pack declaration
void log(Ts... args);         // function parameter pack

sizeof...(Ts)                 // number of types in the pack
sizeof...(args)               // number of values in the pack
args...                       // pack expansion
```

## C++11 Recursive Expansion Pattern
```cpp
void print() {}              // base case — terminates recursion

template<typename T, typename... Rest>
void print(T first, Rest... rest) {
    std::cout << first << "\n";
    print(rest...);           // recursive call with remaining pack
}
```

## C++17 Fold Expression (preview)
```cpp
template<typename... Ts>
void print(Ts... args) { (std::cout << ... << args); }  // one line
```

## Study Checklist
- [ ] Implement `print(args...)` using C++11 recursion
- [ ] Write `max(a, b, c, ...)` for any number of comparable arguments
- [ ] Use `sizeof...` to adapt behaviour based on pack size
- [ ] Implement a simplified `emplace_back`-style factory using perfect forwarding

## References
- [cppreference — Parameter packs](https://en.cppreference.com/w/cpp/language/parameter_pack)
