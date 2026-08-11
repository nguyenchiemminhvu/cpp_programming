# C++14 › `std::shared_mutex` — Shared/Exclusive Locking

## Overview
`std::shared_mutex` (in `<shared_mutex>`) implements the **readers-writer lock**
pattern: multiple readers can hold the lock simultaneously, but a writer
requires exclusive access.

## Lock Modes

| Lock type | Class | Effect |
|-----------|-------|--------|
| Exclusive (write) | `std::unique_lock<shared_mutex>` or `std::lock_guard` | Blocks all other readers and writers |
| Shared (read) | `std::shared_lock<shared_mutex>` | Allows concurrent readers; blocks writers |

```cpp
#include <shared_mutex>

std::shared_mutex rw_mutex;
std::unordered_map<int, std::string> cache;

// Reader
{
    std::shared_lock lock(rw_mutex);   // multiple threads can hold this
    auto it = cache.find(key);
}

// Writer
{
    std::unique_lock lock(rw_mutex);   // exclusive; waits for all readers
    cache[key] = value;
}
```

## `std::shared_timed_mutex`
Also added in C++14; supports timed `try_lock_for` / `try_lock_until` in both
shared and exclusive modes.

## Study Checklist
- [ ] Implement a thread-safe cache using `shared_mutex`
- [ ] Benchmark readers-writer lock vs plain `mutex` under read-heavy workload
- [ ] Show writer starvation under a readers-heavy scenario and discuss mitigations
- [ ] Use `std::shared_lock` (C++14) vs `std::lock_guard` and explain the difference

## References
- [cppreference — shared_mutex](https://en.cppreference.com/w/cpp/thread/shared_mutex)
