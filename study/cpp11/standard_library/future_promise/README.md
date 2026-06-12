# C++11 › `std::future`, `std::promise` & `std::async`

## Overview
The `<future>` header provides task-based asynchronous programming: launch a
task, retrieve its result (or exception) later via a `std::future<T>`.

## Core Primitives

### `std::async` — High-Level Task Launch
```cpp
auto fut = std::async(std::launch::async, []{ return compute(); });
int result = fut.get();   // blocks until complete; rethrows exceptions
```

### `std::promise` / `std::future` — Manual Channel
```cpp
std::promise<int> p;
std::future<int>  f = p.get_future();
std::thread t([&p]{ p.set_value(42); });
std::cout << f.get();   // 42
t.join();
```

### `std::packaged_task<R(Args...)>` — Wrapped Callable
```cpp
std::packaged_task<int(int)> task([](int x){ return x * x; });
auto fut = task.get_future();
std::thread{std::move(task), 5}.detach();
std::cout << fut.get();  // 25
```

## `std::shared_future<T>`
A copyable future that can be waited on by multiple threads.

## Launch Policies
| Policy | Behaviour |
|--------|-----------|
| `std::launch::async` | Runs in a new thread |
| `std::launch::deferred` | Lazy evaluation — runs on `get()` in the calling thread |

## Study Checklist
- [ ] Use `std::async` to parallelise independent computations; combine results
- [ ] Implement a thread handoff using `promise` / `future`
- [ ] Handle exceptions through futures: `set_exception` + `rethrow_exception`
- [ ] Show the difference between `launch::async` and `launch::deferred`
- [ ] Use `shared_future` to broadcast a result to multiple consumer threads

## References
- [cppreference — future](https://en.cppreference.com/w/cpp/thread/future)
- [cppreference — async](https://en.cppreference.com/w/cpp/thread/async)
