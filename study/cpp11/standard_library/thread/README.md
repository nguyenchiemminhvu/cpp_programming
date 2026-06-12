# C++11 › `<thread>` — Concurrency Support

## Overview
C++11 introduced a portable threading library (`<thread>`, `<mutex>`,
`<condition_variable>`, `<future>`) that abstracts over POSIX threads and
Win32 threads.

## Core Headers

| Header | Provides |
|--------|---------|
| `<thread>` | `std::thread` |
| `<mutex>` | `std::mutex`, `std::lock_guard`, `std::unique_lock` |
| `<condition_variable>` | `std::condition_variable` |
| `<atomic>` | `std::atomic<T>` |
| `<future>` | `std::async`, `std::future`, `std::promise` |

## `std::thread` Lifecycle
```cpp
std::thread t(callable, args…);
// Must either join() or detach() before t is destroyed
t.join();    // wait for completion
t.detach();  // release; runs independently
```
Destroying a joinable thread calls `std::terminate()`.

## Synchronization Primitives

| Primitive | Use |
|-----------|-----|
| `std::mutex` + `std::lock_guard` | Basic mutual exclusion (RAII) |
| `std::unique_lock` | Deferred/timed locking, condition variables |
| `std::condition_variable` | Wait / notify between threads |
| `std::atomic<T>` | Lock-free operations on scalar types |

## Study Checklist
- [ ] Spawn multiple threads and join them; observe non-deterministic ordering
- [ ] Protect a shared counter with `std::mutex` + `std::lock_guard`
- [ ] Implement a producer-consumer queue using `condition_variable`
- [ ] Use `std::async` + `std::future` for simple task-based parallelism
- [ ] Demonstrate a data race with `std::atomic` vs plain int

## References
- [cppreference — thread](https://en.cppreference.com/w/cpp/thread/thread)
- *C++ Concurrency in Action*, Anthony Williams
