# C++11 › `std::condition_variable`

## Overview
`std::condition_variable` enables threads to wait until a condition is satisfied,
implementing the **monitor pattern** and avoiding busy-waiting.

## Core API
```cpp
#include <condition_variable>
#include <mutex>

std::mutex              mtx;
std::condition_variable cv;
bool                    data_ready = false;

// Producer thread
{
    std::lock_guard lock{mtx};
    data_ready = true;
}
cv.notify_one();   // or notify_all()

// Consumer thread
{
    std::unique_lock lock{mtx};
    cv.wait(lock, []{ return data_ready; });  // predicate guards spurious wakes
    process();
}
```

## Spurious Wakeups
`cv.wait(lock)` may wake without `notify` being called. **Always** use the
predicate overload: `cv.wait(lock, predicate)`.

## `wait_for` / `wait_until`
```cpp
if (cv.wait_for(lock, 100ms, [&]{ return ready; }))
    process();    // condition met
else
    handle_timeout();
```

## `std::condition_variable_any`
Works with any `BasicLockable` (not just `std::mutex`), at a slight overhead.

## Study Checklist
- [ ] Implement a thread-safe bounded queue using `condition_variable`
- [ ] Show the spurious-wakeup bug and fix it with the predicate form
- [ ] Use `notify_all` for a broadcasting wake-up pattern
- [ ] Implement a countdown barrier using `condition_variable` + a counter

## References
- [cppreference — condition_variable](https://en.cppreference.com/w/cpp/thread/condition_variable)
- *C++ Concurrency in Action*, Anthony Williams
