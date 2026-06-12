# C++11 › Type Aliases — `using` vs `typedef`

## Overview
C++11 introduces the `using` syntax for type aliases. It is more readable
than `typedef` and — crucially — supports **template aliases** which
`typedef` cannot express.

## Syntax Comparison
```cpp
// C++03 typedef
typedef std::vector<int>     IntVec;
typedef void (*FnPtr)(int);  // function pointer — hard to read

// C++11 using alias
using IntVec = std::vector<int>;    // cleaner, left-to-right reading
using FnPtr  = void(*)(int);
```

## Template Aliases (typedef cannot do this)
```cpp
template<typename T>
using Vec = std::vector<T>;

Vec<int>    vi;    // std::vector<int>
Vec<double> vd;    // std::vector<double>

// C++14 standard library pattern (_t suffix)
template<typename T>
using decay_t = typename std::decay<T>::type;
```

## Study Checklist
- [ ] Convert all `typedef` declarations in a file to `using` equivalents
- [ ] Write a template alias for `std::pair<T, std::string>` with one bound type
- [ ] Show that `typedef` cannot create a template alias (compiler error)
- [ ] Use `using` to reduce noise in callback / function-pointer declarations

## References
- [cppreference — Type alias](https://en.cppreference.com/w/cpp/language/type_alias)
