# C++23 › `std::out_ptr` and `std::inout_ptr`

## Overview
C++23 (**P1132**) adds `std::out_ptr` and `std::inout_ptr` in `<memory>` —
adapters that let modern smart pointers interoperate cleanly with **C-style
out-parameter APIs** that write a raw pointer through a `T**` argument.

## The Problem
C libraries frequently expose:
```c
int create_widget(widget** out);   // writes a new pointer into *out
```
Wrapping the result in `std::unique_ptr<widget, widget_deleter>` used to
require a temporary raw pointer plus manual `.reset(...)`:
```cpp
widget* raw = nullptr;
if (create_widget(&raw) != 0) return error;
std::unique_ptr<widget, widget_deleter> owned(raw);   // easy to leak on early return
```

## The Solution
```cpp
std::unique_ptr<widget, widget_deleter> owned;
if (create_widget(std::out_ptr(owned)) != 0) return error;
```
`std::out_ptr(owned)` returns a temporary adapter whose destructor calls
`owned.reset(new_raw_ptr)` after the C function fills in the pointer.

## `std::inout_ptr`
Some C APIs modify an existing pointer in place (`realloc`-like):
```c
int resize_widget(widget** inout, std::size_t new_capacity);
```
Then use `inout_ptr` — it releases the current smart pointer's ownership
before the call and reacquires after:
```cpp
resize_widget(std::inout_ptr(owned), new_cap);
```

## Signatures
```cpp
namespace std {
    template <class Pointer, class Smart, class... Args>
    auto out_ptr(Smart& s, Args&&... args) noexcept;

    template <class Pointer, class Smart, class... Args>
    auto inout_ptr(Smart& s, Args&&... args) noexcept;
}
```
`Args` are forwarded to `Smart::reset(new_raw_ptr, args…)` — perfect for
smart pointers that need extra parameters at reset time (e.g., a stateful
deleter).

## Works With Any Smart-Pointer-Like Type
- `std::unique_ptr<T, Deleter>`
- `std::shared_ptr<T>`  (via `inout_ptr` requires care; see the paper)
- User-defined RAII wrappers exposing `.reset(pointer)` / `.release()`

## Common C API Bridges

### OpenSSL, SQLite, libcurl…
```cpp
std::unique_ptr<sqlite3, sqlite3_deleter> db;
if (sqlite3_open("file.db", std::out_ptr(db)) != SQLITE_OK) …
```

### Windows COM
```cpp
Microsoft::WRL::ComPtr<IUnknown> obj;
if (CoCreateInstance(clsid, nullptr, ctx, iid,
                     std::out_ptr<void*>(obj)) != S_OK) …
```
(Explicit `Pointer = void*` disambiguates the pointer type when it differs
from the smart pointer's target type.)

## Exception Safety
The adapter's destructor performs the reset. If the C function throws (which
should be rare, but can happen through callbacks), the adapter still commits
whatever pointer was written up to that point — usually the desired behaviour.
If nothing was written, the smart pointer is left null (`out_ptr`) or
unchanged (`inout_ptr`).

## Study Checklist
- [ ] Wrap an OpenSSL / SQLite `create_*` API with `std::out_ptr`
- [ ] Use `std::inout_ptr` on a `realloc`-style function
- [ ] Store a custom deleter in `std::unique_ptr<T, D>` and pass it to `out_ptr`
- [ ] Compare with the manual `raw + reset` pattern; verify no leaks under exceptions
- [ ] Adapt a custom RAII wrapper class to interoperate with `out_ptr`

## References
- [P1132 — `out_ptr`, a scalable output pointer abstraction](https://wg21.link/P1132)
- [cppreference — `std::out_ptr`](https://en.cppreference.com/w/cpp/memory/out_ptr_t/out_ptr)
- [cppreference — `std::inout_ptr`](https://en.cppreference.com/w/cpp/memory/inout_ptr_t/inout_ptr)
