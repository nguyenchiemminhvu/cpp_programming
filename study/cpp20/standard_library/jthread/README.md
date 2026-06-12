# C++20 › `std::jthread` — Joining Thread with Stop Token

## Overview
`std::jthread` is a thread wrapper that **automatically joins** in its destructor
(unlike `std::thread` which calls `std::terminate()`) and supports cooperative
cancellation via `std::stop_token`.

## `std::jthread` vs `std::thread`
| | `std::thread` | `std::jthread` |
|-|---------------|----------------|
| Destructor | `terminate()` if joinable | Requests stop + joins |
| Cancellation | Manual flag | `stop_token` built-in |
| Header | `<thread>` | `<thread>` |

## Basic Usage
```cpp
#include <thread>

std::jthread worker([](std::stop_token st) {
    while (!st.stop_requested()) {
        do_work();
    }
});

// Request cancellation explicitly (destructor also does this)
// worker.request_stop();
// jthread destructor calls request_stop() + join()
```

## `std::stop_source` / `std::stop_token` / `std::stop_callback`
```cpp
std::stop_source source;
std::stop_token  token = source.get_token();

std::stop_callback cb{token, []{ cleanup_resources(); }};

source.request_stop();   // fires the callback; sets stop_requested()
```

## Study Checklist
- [ ] Replace a `std::thread` + manual `join()` with `std::jthread`
- [ ] Implement a cancellable worker loop using `stop_token`
- [ ] Register a `stop_callback` and verify it fires on `request_stop()`
- [ ] Show that `jthread` destructor safely cancels and joins without `std::terminate`

## References
- [cppreference — jthread](https://en.cppreference.com/w/cpp/thread/jthread)
