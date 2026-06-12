# C++17 › `std::scoped_lock`

## Overview
`std::scoped_lock<M...>` is an RAII lock guard that acquires multiple mutexes
simultaneously using a **deadlock-avoidance algorithm**, replacing the error-prone
manual ordering required with `std::lock` + `adopt_lock`.

## Syntax
```cpp
#include <mutex>

std::mutex mtx1, mtx2;

// Acquire both atomically, deadlock-free
std::scoped_lock lock{mtx1, mtx2};   // CTAD deduces scoped_lock<mutex, mutex>
// both unlocked on destruction
```

## vs C++11 Multi-Lock (verbose)
```cpp
// C++11 — deadlock-free but verbose
std::lock(mtx1, mtx2);
std::lock_guard l1{mtx1, std::adopt_lock};
std::lock_guard l2{mtx2, std::adopt_lock};
```

## Single-Mutex Use
`std::scoped_lock` works with a single mutex too, as a drop-in replacement
for `std::lock_guard`:
```cpp
std::scoped_lock lock{mtx};   // equivalent to lock_guard
```

## Study Checklist
- [ ] Fix a classic ABBA deadlock using `scoped_lock`
- [ ] Benchmark `scoped_lock` vs `lock_guard` for single-mutex overhead
- [ ] Demonstrate that the argument order to `scoped_lock` doesn't matter
- [ ] Show the C++11 `std::lock` + `adopt_lock` equivalent for comparison

## References
- [cppreference — scoped_lock](https://en.cppreference.com/w/cpp/thread/scoped_lock)
