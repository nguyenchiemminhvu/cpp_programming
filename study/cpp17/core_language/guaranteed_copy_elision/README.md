# C++17 › Guaranteed Copy Elision

## Overview
C++17 mandates **guaranteed copy elision** for prvalue initialisation: when
a prvalue is used to initialise an object of the same type, no copy or move
constructor is invoked — the object is constructed directly in place.

## What Changed
```cpp
struct NoCopy {
    NoCopy() = default;
    NoCopy(const NoCopy&) = delete;   // not copyable
    NoCopy(NoCopy&&)      = delete;   // not movable
};

// C++14: ERROR — move ctor is deleted (even with NRVO, not guaranteed)
// C++17: OK — guaranteed elision, no constructor invoked
NoCopy make() { return NoCopy{}; }
NoCopy obj = make();   // C++17: direct construction, zero copies
```

## Prvalue vs Materialisation
In C++17, a **prvalue** does not create a temporary — it is a recipe for
constructing an object. Materialisation to a real object (xvalue) only
happens when necessary (see `prvalue_materialization`).

## Return Value Optimisation (RVO) — Mandatory
| Scenario | C++17 Status |
|----------|-------------|
| Return a prvalue: `return T{}` | Mandatory elision |
| Named RVO (`return named_var`): `return x` | Optional (NRVO) — allowed, not required |

## Study Checklist
- [ ] Create a non-copyable, non-movable type; verify C++17 can return it by value
- [ ] Show that NRVO (named variable return) is still optional
- [ ] Count constructor invocations with/without C++17 copy elision
- [ ] Explain why `return {args}` is always mandatory elision

## References
- [cppreference — Copy elision](https://en.cppreference.com/w/cpp/language/copy_elision)
