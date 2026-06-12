# C++11 › `thread_local` Storage

## Overview
`thread_local` is a storage class specifier declaring that a variable has
**thread-local duration**: each thread gets its own independent copy,
initialised the first time the thread accesses it and destroyed when the
thread exits.

## Syntax
```cpp
thread_local int counter = 0;   // each thread has its own counter

void increment() {
    ++counter;   // no synchronisation needed — per-thread copy
}
```

## Scope & Restrictions
- Applicable to: namespace-scope, block-scope (`static` implied), and `static` class members.
- **Cannot** be applied to non-static class data members.
- Initialisation is **lazy** per thread — occurs on first access.

## Use Cases
- Per-thread caches, logging buffers, error state
- Thread-specific random number generators (PRNG state)
- Thread-local allocators or arena buffers

## Study Checklist
- [ ] Create a `thread_local` counter; spawn 5 threads and verify each has its own count
- [ ] Use `thread_local` for a per-thread `std::mt19937` PRNG
- [ ] Measure `thread_local` access overhead vs `global + mutex` vs `atomic`
- [ ] Show that `thread_local static` inside a function is lazily initialised per thread

## References
- [cppreference — Storage class specifiers (thread_local)](https://en.cppreference.com/w/cpp/language/storage_duration#Thread_storage_duration)
