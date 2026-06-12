# C++20 › `std::stop_token`, `std::stop_source` & `std::stop_callback`

## Overview
The stop token API provides a **cooperative cancellation** mechanism that lets
threads signal each other to stop gracefully, without forcible termination.
It integrates directly with `std::jthread`.

## Core Types
| Type | Role |
|------|------|
| `std::stop_source` | Issues the stop request |
| `std::stop_token` | Queried by the stoppable operation |
| `std::stop_callback<F>` | Fires `F` when stop is requested |

## Basic Pattern
```cpp
#include <stop_token>
#include <thread>

std::stop_source source;

std::jthread worker{[token = source.get_token()]() {
    while (!token.stop_requested()) {
        do_work();
    }
    cleanup();
}};

// Trigger cancellation from elsewhere
source.request_stop();   // sets the flag; worker sees it on next check
```

## `stop_callback` — Interrupt Blocking Operations
```cpp
std::stop_callback cb{token, [&cv]{ cv.notify_all(); }};
// When stop is requested, notify_all wakes the blocked thread
cv.wait(lock, [&]{ return done || token.stop_requested(); });
```

## `std::jthread` Integration
`std::jthread` automatically provides a `stop_source`; its destructor calls
`request_stop()` + `join()`.

## Study Checklist
- [ ] Build a cancellable work loop using `stop_token`
- [ ] Use `stop_callback` to wake a `condition_variable`-blocked thread
- [ ] Show the lifecycle: source creates token; token is queryable; callback fires once
- [ ] Compare `jthread` + `stop_token` vs raw `thread` + manual `atomic<bool>` flag

## References
- [cppreference — stop_token](https://en.cppreference.com/w/cpp/thread/stop_token)
