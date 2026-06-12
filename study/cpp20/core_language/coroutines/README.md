# C++20 › Coroutines

## Overview
Coroutines are functions that can **suspend and resume** execution at designated
points, enabling cooperative multitasking, generators, and asynchronous I/O
without threads.

## Three Magic Keywords

| Keyword | Effect |
|---------|--------|
| `co_await expr` | Suspend until `expr` is ready |
| `co_yield value` | Suspend and produce a value to the caller |
| `co_return [value]` | Complete the coroutine |

A function is a coroutine if its body contains any of these keywords.

## The Coroutine Protocol
C++20 defines the protocol but provides **no ready-to-use coroutine types**.
You must implement (or use a library for) the promise type and awaitable types.

```
┌─────────────────────────────────────────────────────┐
│  Coroutine Frame (heap-allocated by the compiler)   │
│  ┌──────────────┐   ┌────────────────────────────┐  │
│  │ Promise type │   │  Coroutine state machine   │  │
│  └──────────────┘   └────────────────────────────┘  │
└─────────────────────────────────────────────────────┘
```

## Minimal Generator Pattern
```cpp
// Requires implementing a Generator<T> type with promise_type
// See cppreference for the full boilerplate, or use cppcoro / libcoro.
Generator<int> range(int from, int to) {
    for (int i = from; i < to; ++i)
        co_yield i;
}
for (int x : range(0, 5))
    std::cout << x;
```

## Ecosystem
The standard provides the machinery; use these libraries for production coroutines:
- **cppcoro** (Lewis Baker) — `task<T>`, `generator<T>`, schedulers
- **libunifex** / **P2300 `std::execution`** — structured concurrency (C++26 direction)
- **Asio** — coroutine-based async I/O

## Study Checklist
- [ ] Implement a minimal `Generator<T>` with `promise_type`
- [ ] Write a lazy Fibonacci generator using `co_yield`
- [ ] Implement a simple `Task<T>` that wraps a coroutine
- [ ] Use `co_await std::suspend_always{}` to understand suspend/resume points
- [ ] Compare coroutine-based async vs `std::async` + `std::future`

## References
- [cppreference — Coroutines](https://en.cppreference.com/w/cpp/language/coroutines)
- [cppcoro library](https://github.com/lewissbaker/cppcoro)
