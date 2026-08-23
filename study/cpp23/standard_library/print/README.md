# C++23 › `std::print` and `std::println`

## Overview
C++23 (**P2093**) adds `std::print` and `std::println` in `<print>` — direct,
formatted, allocation-free output built on top of `std::format`. It is the
long-awaited replacement for both `printf` (type-unsafe) and `std::cout`
(verbose, allocating, and slow).

## Synopsis
```cpp
#include <print>

int main() {
    std::print("Hello, {}!\n", "world");            // to stdout
    std::println("x = {:>6}, y = {:.3f}", 42, 3.14159);   // adds '\n'

    std::print(stderr, "warning: {}\n", "disk full");
    std::print(some_ofstream, "id={}\n", id);
}
```

## Key Points
- **Same format language as `std::format`** — full mini-language, custom
  `std::formatter<T>`, positional args `{0} {1}`, all the numeric / alignment
  specifiers.
- **No `iostream` dependency** — much smaller code footprint than `<iostream>`.
- **No intermediate allocation** — writes directly to the stream's buffer.
- **Unicode-aware** on `stdout`/`stderr` when the terminal reports UTF-8, on
  all major platforms (POSIX + Windows console).
- Overloads for `FILE*` and `std::ostream&` (via `std::print(stream, fmt, args…)`).

## `println` — Trailing Newline
`std::println("…", …)` == `std::print("…\n", …)`. Also has zero-arg form
`std::println()` for a bare newline.

## Format String
The format string is a compile-time-checked `std::format_string` — bad
`{`/`}` or type mismatches error at compile time (like `-Wformat` for
`printf`, but portable and mandatory).

## Runtime-Constructed Format Strings
Compile-time checking prevents `std::print(user_input, x);`. Use
`std::vprint_unicode(fmt_str, std::make_format_args(x))` for runtime format
strings — read the docs carefully; the argument-pack machinery is subtle.

## Performance
Faster than `iostream` (no locale, no sync-with-stdio), faster than the
allocating `std::cout << std::format(...)` idiom, and typically comparable to
or faster than `printf` for equivalent output.

## Availability
- GCC 14+ (libstdc++)
- Clang 18+ (libc++), Clang 19+ (libstdc++)
- MSVC 19.40+ (VS 2022 17.10)

## Study Checklist
- [ ] Replace `std::cout << std::format(…)` with `std::print(…)` and compare speed
- [ ] Provoke a compile-time format-string error
- [ ] Redirect to `stderr` and to a file stream
- [ ] Implement a custom `std::formatter` for a user type; use it in `std::print`
- [ ] Investigate `std::vprint_unicode` / `std::vprint_nonunicode` for runtime format strings

## References
- [P2093 — Formatted output](https://wg21.link/P2093)
- [cppreference — `std::print`](https://en.cppreference.com/w/cpp/io/print)
