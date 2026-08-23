# C++23 › `std::stacktrace` — Portable Stack Traces

## Overview
C++23 (**P0881**) adds `std::stacktrace` and `std::stacktrace_entry` in
`<stacktrace>` — a standard, portable API for capturing and inspecting the
current call stack. It replaces platform-specific tooling (`backtrace(3)`,
`CaptureStackBackTrace`, `boost::stacktrace`) with a first-class standard type.

## Basic Usage
```cpp
#include <stacktrace>
#include <print>

void diagnose() {
    auto trace = std::stacktrace::current();
    for (const auto& frame : trace)
        std::println("{}", frame);              // uses default formatter
}
```
`std::stacktrace` provides `.size()`, iteration, and range access — it is a
plain container of `stacktrace_entry`s.

## `stacktrace_entry` API
```cpp
class stacktrace_entry {
public:
    std::string description() const;      // e.g. function name (mangled if unavailable)
    std::string source_file() const;      // file path, if debug info present
    std::uint_least32_t source_line() const;
    // ordering and equality operators
};
```
The `description()` may include the mangled symbol; demangling is
implementation-defined.

## `std::stacktrace::current` Overloads
```cpp
static basic_stacktrace current() noexcept;
static basic_stacktrace current(std::size_t skip) noexcept;
static basic_stacktrace current(std::size_t skip, std::size_t max_depth) noexcept;
```
`skip` — drop N innermost frames (useful for logging helpers that shouldn't
appear in the trace). `max_depth` — cap the total number of frames.

## Formatting
`std::formatter<std::stacktrace>` and `std::formatter<std::stacktrace_entry>`
specialisations are provided:
```cpp
std::println("panic: {}\nstack:\n{}", msg, std::stacktrace::current());
```

## Storage and Cost
- `std::stacktrace` **owns** the captured frames; safe to copy, move, store.
- Uses `std::allocator<stacktrace_entry>` by default; a custom allocator
  variant is available (`std::basic_stacktrace<Allocator>`).
- Capturing is O(depth) and typically requires symbol table + debug info to
  produce useful strings — enable `-g` / `/Zi` in release builds if you
  need readable output.

## Common Patterns

### Attach to a custom exception
```cpp
struct traced : std::runtime_error {
    std::stacktrace trace = std::stacktrace::current();
    using std::runtime_error::runtime_error;
};

try { … }
catch (const traced& e) {
    std::println("{}\n{}", e.what(), e.trace);
}
```

### Signal handler support
Capturing from a signal handler is *not* async-signal-safe on most platforms —
use `sigsafe_stacktrace` extensions if provided, or defer capture to a normal
thread.

## Availability
- GCC 12+ (libstdc++, link with `-lstdc++_libbacktrace` on some distros)
- Clang: libc++ not yet, use `<experimental/stacktrace>` or libbacktrace
- MSVC 19.34+ (VS 2022 17.4)

## Study Checklist
- [ ] Print `std::stacktrace::current()` from a deeply nested function
- [ ] Capture with `skip = 1` from a logging helper — confirm the helper is elided
- [ ] Store a trace inside a custom exception type and print at the top level
- [ ] Compare readability of debug vs release builds — enable debug info in release
- [ ] Investigate demangling on your compiler / platform

## References
- [P0881 — A proposal to add stacktrace library](https://wg21.link/P0881)
- [cppreference — `std::stacktrace`](https://en.cppreference.com/w/cpp/utility/basic_stacktrace)
