# C++11 › Smart Pointers

## Overview
Smart pointers automate dynamic memory management by tying object lifetime to
RAII. They live in `<memory>` and make raw `new`/`delete` largely unnecessary.

## Types

### `std::unique_ptr<T>` — Exclusive ownership
- Non-copyable; movable.
- Zero overhead compared to a raw pointer.
- Use `std::make_unique<T>(args…)` (C++14) for exception-safe construction.
- Use for single-owner resources, factory return types, and PIMPL.

### `std::shared_ptr<T>` — Shared ownership
- Reference-counted; copyable and movable.
- Small but real overhead: one heap allocation for the control block (unless `make_shared`).
- Use `std::make_shared<T>(args…)` — allocates object + control block in one shot.
- Beware of **cyclic references** → memory leak.

### `std::weak_ptr<T>` — Non-owning observer
- Does not affect reference count.
- Must be **locked** (`weak_ptr::lock()`) to obtain a `shared_ptr` before use.
- Breaks `shared_ptr` cycles.

## Ownership Decision Tree
```
Single owner, no sharing  →  unique_ptr
Shared ownership          →  shared_ptr
Observer / cycle breaker  →  weak_ptr
```

## Study Checklist
- [ ] Replace a raw `new`/`delete` pair with `unique_ptr`
- [ ] Demonstrate that `unique_ptr` cannot be copied but can be moved
- [ ] Show `shared_ptr` reference count with `use_count()`
- [ ] Create a cyclic `shared_ptr` structure and confirm leak; break it with `weak_ptr`
- [ ] Compare `make_shared` vs separate `new` + `shared_ptr` constructor (one allocation vs two)

## References
- [cppreference — unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr)
- [cppreference — shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr)
- *Effective Modern C++*, Scott Meyers — Items 18–21
