# C++14 › `std::make_unique`

## Overview
`std::make_unique<T>(args…)` is the factory function for `std::unique_ptr<T>`,
plugging the exception-safety gap that existed in C++11 when using
`std::unique_ptr<T>(new T(args…))` directly.

## Why It Matters

### Exception-Safety Problem in C++11
```cpp
// Potentially leaks if the second new throws before unique_ptr is constructed:
process(std::unique_ptr<A>(new A), std::unique_ptr<B>(new B));
```

### The Fix (C++14)
```cpp
// Safe: each make_unique is a complete expression
process(std::make_unique<A>(), std::make_unique<B>());
```

## Forms
```cpp
// Single object
auto p = std::make_unique<Widget>(arg1, arg2);

// Array (size known at runtime)
auto arr = std::make_unique<int[]>(10);  // int[10], zero-initialised

// Note: make_unique does NOT support custom deleters
// Use unique_ptr constructor directly for that.
```

## `make_unique` vs `make_shared`
- `make_unique`: one allocation for the object.
- `make_shared`: one allocation for object + control block (more efficient).
- `make_unique` has no observable overhead over manual `new`.

## Study Checklist
- [ ] Replace all `new Widget` in a codebase with `make_unique`
- [ ] Demonstrate the exception-safety issue without `make_unique`
- [ ] Create a `unique_ptr<T[]>` for a dynamically-sized buffer
- [ ] Compare assembly output of `make_unique` vs `new`

## References
- [cppreference — make_unique](https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique)
- *Effective Modern C++*, Scott Meyers — Item 21
