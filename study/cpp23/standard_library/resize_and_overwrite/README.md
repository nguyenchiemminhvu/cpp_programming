# C++23 › `std::basic_string::resize_and_overwrite`

## Overview
C++23 (**P1072**) adds `resize_and_overwrite` to `std::basic_string` — a
zero-initialisation-free resize that hands the writable buffer to a
user-supplied callable, then commits the *actual* number of bytes written.
It is the single biggest single-call speedup for "write into a `std::string`"
patterns.

## Signature
```cpp
namespace std {
    template <class Operation>
    void resize_and_overwrite(std::size_t n, Operation op);
    //                        capacity  callable
}
```
`op` must be callable as `op(char* dst, std::size_t n) -> std::size_t`,
returning the number of bytes actually produced (`<= n`). After the call,
the string's `size()` becomes that returned value.

## Basic Usage
```cpp
std::string result;
result.resize_and_overwrite(1024, [](char* p, std::size_t cap) {
    // Fill `p` with up to `cap` bytes; return actual length.
    int n = std::snprintf(p, cap, "x=%d,y=%d", 42, 7);
    return static_cast<std::size_t>(n);
});
```

## Why It's Faster Than Alternatives

### Old idiom — double work
```cpp
std::string result;
result.resize(1024);                        // 🐌 zero-fills 1024 bytes
int n = std::snprintf(result.data(), 1024, …);
result.resize(n);                           // truncates
```
Steps 1 and 3 are wasted work; step 1 also blows out the cache for large
`n`.

### New idiom — no zero-fill, single resize
```cpp
result.resize_and_overwrite(1024, [](char* p, std::size_t cap){
    return static_cast<std::size_t>(std::snprintf(p, cap, …));
});
```
The string's storage is uninitialised (or, more precisely, "value-initialised
in a way the standard permits skipping") until the callable fills it.

## Use Cases
- **`sprintf`-into-string** — the archetypal case (see above).
- **Binary encoders** — hex, base64, JSON escaping.
- **Reading from I/O** — `read(fd, p, cap)` into a `std::string` in one shot.
- **Hash / compression sinks** — many APIs write into a buffer and return the
  used length.

## Rules
- The callable **must** initialise the range `[dst, dst + returned_length)` —
  reading before that is UB.
- The returned length must satisfy `<= n`.
- The trailing `NUL` terminator is appended automatically by the standard
  library, so you may write exactly `size` characters.
- Exceptions from the callable propagate; the string is left in a valid but
  unspecified state.

## Contrast With `reserve` + `push_back`
`reserve` grows capacity; `resize_and_overwrite` grows size *and* provides an
uninitialised region — the callable can write via random access rather than
appending.

## Not Available For Other Containers
This is a string-only addition. For `std::vector<T>` you still need
`reserve` + `emplace_back`, or `resize` + manual initialisation. Similar
proposals for vector exist but are not in C++23.

## Availability
- GCC 12+, Clang 14+ (libc++), MSVC 19.31+

## Study Checklist
- [ ] Rewrite an `snprintf`-into-`std::string` pattern using `resize_and_overwrite`
- [ ] Measure allocation / zero-fill cost avoided at N = 1 MB
- [ ] Read `N` bytes from a file into a fresh `std::string` in one call
- [ ] Confirm behaviour when the returned length is `< n` (truncation)
- [ ] Verify the callable is invoked exactly once

## References
- [P1072 — `basic_string::resize_and_overwrite`](https://wg21.link/P1072)
- [cppreference — `std::basic_string::resize_and_overwrite`](https://en.cppreference.com/w/cpp/string/basic_string/resize_and_overwrite)
