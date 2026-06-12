# C++11 › Explicit Conversion Operators

## Overview
C++11 adds the `explicit` keyword to conversion operators, preventing implicit
conversions that could lead to subtle bugs while still allowing explicit casts.

## Syntax
```cpp
struct Safe {
    explicit operator bool() const { return valid_; }
    explicit operator int()  const { return value_; }
private:
    bool valid_{};
    int  value_{};
};

Safe s;
if (s) { }                       // OK — contextual conversion to bool
bool b = static_cast<bool>(s);   // OK — explicit cast
bool c = s;                      // ERROR — implicit conversion blocked
```

## The `operator bool` Idiom
Before C++11, implementing a "safe bool" required complex workarounds.
`explicit operator bool()` is the idiomatic solution. Contextual conversions
(used in `if`, `while`, `!`, `&&`, `||`) still work with `explicit`.

## Study Checklist
- [ ] Implement a `Maybe<T>` type with `explicit operator bool()` and `explicit operator T()`
- [ ] Show the pre-C++11 safe-bool idiom and compare
- [ ] Demonstrate that contextual conversions use `explicit` operator bool
- [ ] Show where `explicit` prevents unexpected overload resolution

## References
- [cppreference — Conversion function](https://en.cppreference.com/w/cpp/language/cast_operator)
- *Effective Modern C++*, Scott Meyers — Item 15
