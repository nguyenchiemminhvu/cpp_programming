# C++11 › `override` & `final`

## Overview
`override` and `final` are context-sensitive keywords that improve the safety
and readability of class hierarchies. The compiler enforces their semantics
without breaking code that uses `override` or `final` as identifiers.

## `override`
Declares that a virtual function is **intended** to override a base class
virtual function. The compiler errors if no matching base function exists.
```cpp
struct Base { virtual void f(int); };

struct Derived : Base {
    void f(int) override;    // OK — overrides Base::f(int)
    void f(double) override; // ERROR — Base::f(double) does not exist
};
```

## `final`
- On a **class**: prevents further derivation.
- On a **virtual function**: prevents further overriding.
```cpp
struct Leaf final : Base { … };       // cannot be subclassed
virtual void g() final;               // cannot be overridden further
```

## Study Checklist
- [ ] Add `override` to every overriding function in a class hierarchy; observe compile errors for mismatches
- [ ] Show that a typo in the function signature without `override` silently creates a new function
- [ ] Mark a class `final` and verify the compiler prevents derivation
- [ ] Discuss devirtualisation: `final` classes/functions allow the compiler to inline virtual calls

## References
- [cppreference — override](https://en.cppreference.com/w/cpp/language/override)
- [cppreference — final](https://en.cppreference.com/w/cpp/language/final)
- *Effective Modern C++*, Scott Meyers — Item 12
