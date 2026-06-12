# C++11 › Mutex & Lock Types

## Overview
C++11 provides a family of mutex types and RAII lock guards to implement
mutual exclusion safely and without resource leaks.

## Mutex Types
| Type | Use |
|------|-----|
| `std::mutex` | Non-recursive, non-timed exclusive mutex |
| `std::recursive_mutex` | Can be locked multiple times by the same thread |
| `std::timed_mutex` | Supports `try_lock_for` / `try_lock_until` |
| `std::recursive_timed_mutex` | Recursive + timed |

## Lock Guards (RAII)
| Guard | Behaviour |
|-------|-----------|
| `std::lock_guard<M>` | Locks on construction, unlocks on destruction — no manual control |
| `std::unique_lock<M>` | Deferred/timed locking, manual `lock()`/`unlock()`, movable |
| `std::scoped_lock<M...>` | C++17 — locks multiple mutexes deadlock-free |

## Examples
```cpp
std::mutex mtx;

// Simplest — prefer this for basic RAII
{ std::lock_guard lock{mtx}; modify_data(); }

// When you need condition_variable or timed locking
{ std::unique_lock lock{mtx}; cv.wait(lock, predicate); }

// Deadlock-free multi-mutex lock (C++17)
{ std::scoped_lock lock{mtx1, mtx2}; /* safe */ }
```

## Deadlock Prevention
- Always acquire multiple locks in the same order, OR use `std::scoped_lock`.
- Use `std::lock(m1, m2)` + `std::adopt_lock` for C++11-compatible multi-lock.

## Study Checklist
- [ ] Implement a thread-safe counter using `mutex` + `lock_guard`
- [ ] Show that `recursive_mutex` allows re-entrant locking; `mutex` deadlocks
- [ ] Use `unique_lock` with `try_lock_for` for bounded waiting
- [ ] Demonstrate the classic ABBA deadlock and fix it with `scoped_lock`

## References
- [cppreference — mutex](https://en.cppreference.com/w/cpp/header/mutex)
