# Advanced › Atomic Operations (`std::atomic<T>`)

## Overview
`std::atomic<T>` provides lock-free, thread-safe access to a value without
a mutex, via hardware compare-and-swap instructions. It is the foundation
of lock-free data structures.

## Core Operations
```cpp
std::atomic<int> counter{0};

counter.fetch_add(1);                // atomic increment
counter.store(0);                    // atomic write
int v = counter.load();              // atomic read
counter.exchange(new_val);           // swap, return old

// Compare-and-swap (the fundamental primitive)
int expected = 5;
bool swapped = counter.compare_exchange_strong(expected, 10);
// if counter == expected → set to 10, return true
// else → update expected to actual value, return false
```

## Specialisations
- `std::atomic<bool>`, `std::atomic<T*>` (pointer arithmetic)
- `std::atomic_flag` — minimal, always lock-free, `test_and_set()`
- C++20: `std::atomic<shared_ptr<T>>`, `std::atomic_ref<T>`

## Lock-Free Guarantee
`atomic::is_lock_free()` returns true if hardware instructions are used;
false if a hidden mutex is used (typically for large types).

## Study Checklist
- [ ] Implement a thread-safe counter with `fetch_add`; verify no races
- [ ] Write a CAS loop to implement a lock-free stack push
- [ ] Use `compare_exchange_weak` in a spin loop; explain spurious failure
- [ ] Measure `atomic<int>` vs `mutex + int` throughput under contention

## References
- [cppreference — atomic](https://en.cppreference.com/w/cpp/atomic/atomic)
- *C++ Concurrency in Action*, Anthony Williams — Chapter 5
