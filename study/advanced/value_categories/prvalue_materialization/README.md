# Advanced › Prvalue Materialization (Temporary Materialization)

## Overview
**Temporary materialization** is the implicit conversion of a **prvalue** to
an **xvalue** — it occurs when an actual temporary object must be created.
C++17 makes this process explicit and guarantees copy elision for direct
prvalue initialisation.

## The C++17 Model
In C++17, a prvalue is not an object — it is a **recipe** for constructing one.
Materialization to an xvalue (temporary) happens only when an actual object
is needed (e.g., binding to a reference, accessing a member, throwing).

## When Materialisation Occurs
```cpp
// Binding a prvalue to a const/rvalue reference → materialise
const int& r = 42;        // 42 materialises into a temporary; r binds to it
int&&      x = compute(); // prvalue materialises to xvalue; x binds

// Accessing a member of a prvalue → materialise
int n = std::string{"hello"}.size();  // temporary string created, then destroyed

// Throwing a prvalue → materialise into the exception object
throw std::runtime_error{"fail"};
```

## vs C++14 Semantics
In C++14, prvalues immediately created temporaries. In C++17 they don't until
necessary — enabling guaranteed copy elision (see `guaranteed_copy_elision`).

## Temporary Lifetime Extension
A temporary bound to a `const &` or `&&` has its lifetime extended to match
the reference's scope (with exceptions for members, function parameters, etc.).

## Study Checklist
- [ ] Show that `T obj = T{}` constructs directly without a temporary in C++17
- [ ] Show that `const T& r = T{}` does materialise a temporary (r binds to it)
- [ ] Trace the lifetime of a temporary in `f(T{})` — when is it destroyed?
- [ ] Explain when temporary lifetime extension applies

## References
- [cppreference — Implicit conversions (temporary materialization)](https://en.cppreference.com/w/cpp/language/implicit_conversion#Temporary_materialization)
