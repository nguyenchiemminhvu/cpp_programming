# C++11 › `= default` & `= delete`

## Overview
`= default` requests the compiler to generate a special member function with
its default behaviour. `= delete` explicitly removes a function from overload
resolution, producing clear diagnostics rather than cryptic linker errors.

## `= default`
```cpp
struct Widget {
    Widget() = default;               // use compiler-generated default ctor
    Widget(const Widget&) = default;  // explicitly defaulted copy ctor
    Widget& operator=(Widget&&) = default;
    ~Widget() = default;
};
```
Useful to restore a defaulted function after declaring another special member
(otherwise the compiler suppresses it).

## `= delete`
```cpp
struct NonCopyable {
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

// Block an implicit conversion overload
void process(int x);
void process(double) = delete;   // prevents process(3.14) from compiling
```

## Study Checklist
- [ ] Create a move-only type by `= delete`-ing copy constructor and copy assignment
- [ ] Use `= default` on a destructor to re-enable copy operations after declaring one
- [ ] Block a specific conversion by deleting the overload
- [ ] Compare `= delete` vs pre-C++11 `private` trick — compare compiler error quality

## References
- [cppreference — Explicitly defaulted functions](https://en.cppreference.com/w/cpp/language/function#Explicitly-defaulted_functions)
- *Effective Modern C++*, Scott Meyers — Item 17
