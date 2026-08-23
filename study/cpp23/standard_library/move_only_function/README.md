# C++23 › `std::move_only_function`

## Overview
C++23 (**P0288**) adds `std::move_only_function` in `<functional>` — a
type-erased callable wrapper like `std::function`, but **without the
copyability requirement**. It can hold move-only callables such as
lambdas that capture `std::unique_ptr`, `std::promise`, or other
non-copyable types.

## Signature (essence)
```cpp
namespace std {
    template <class Signature>
    class move_only_function;   // partial specialisations follow
}
```
The signature can carry cvref qualifiers, `noexcept`, and a return type:
```cpp
std::move_only_function<int(std::string_view)>              cb;
std::move_only_function<void(int) const>                     const_cb;
std::move_only_function<void(int) noexcept>                  nx_cb;
std::move_only_function<int(std::string_view) const noexcept> combo;
```

## Basic Usage
```cpp
auto owned = std::make_unique<expensive_state>();
std::move_only_function<int(int)> f =
    [state = std::move(owned)](int x){ return state->process(x); };
    // ✅ move-only capture — impossible with std::function

int r = f(42);
auto g = std::move(f);   // moving is fine
// auto h = f;           // ❌ deleted copy constructor
```

## `move_only_function` vs `std::function`

| Aspect | `std::function` | `std::move_only_function` (C++23) |
|---|---|---|
| Copy | Required from the callable | Not required |
| Small-buffer optimisation | Yes, but constrained by copyability | Yes, more permissive |
| Overhead | ~2 pointers + small buffer | Similar |
| Signature qualifiers | Only `T(Args…)` | `T(Args…) const`, `noexcept`, `&`, `&&` |
| Use case | General-purpose callback | Coroutines, one-shot handlers, sinks |

## Qualifier Support

### `const` signature — restricts to `const`-callable targets
```cpp
std::move_only_function<int() const> f = []{ return 42; };
// f() must not observe non-const behaviour on the target
```

### `noexcept` signature — target must be `noexcept`
```cpp
std::move_only_function<int() noexcept> g = []() noexcept { return 1; };
```

### Ref-qualified signatures — `&`, `&&`
Restrict how the wrapper may be invoked (l-value vs r-value context).

## Idioms

### Coroutine callback
```cpp
async_task task = std::async_read(
    filename,
    [buf = std::vector<char>(4096)](std::span<const char> data) mutable {
        buf.assign(data.begin(), data.end());
    });
```
Storing that closure inside a callback member: `std::move_only_function<void(std::span<const char>)>`.

### One-shot handler queue
```cpp
std::queue<std::move_only_function<void()>> pending;
```

## Availability
- GCC 12+, Clang 15+, MSVC 19.32+

## Migration Note
Prefer `std::move_only_function` for **any new code** where the callback is
invoked once, or where the caller owns the only reference. Use
`std::function` only when copies are *actually* needed.

## Study Checklist
- [ ] Write a lambda capturing a `std::unique_ptr` and store it in `move_only_function`
- [ ] Try to copy the wrapper — confirm the compiler error
- [ ] Use a `const`-qualified signature to enforce read-only calls
- [ ] Use `noexcept` signature and verify SFINAE rejects a throwing callable
- [ ] Build a mini task queue based on `std::move_only_function<void()>`

## References
- [P0288 — `move_only_function`](https://wg21.link/P0288)
- [cppreference — `std::move_only_function`](https://en.cppreference.com/w/cpp/utility/functional/move_only_function)
