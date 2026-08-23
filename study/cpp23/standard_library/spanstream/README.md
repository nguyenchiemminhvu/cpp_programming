# C++23 › `std::spanstream` — `iostream` Over a `std::span`

## Overview
C++23 (**P0448**) adds `std::spanstream`, `std::ispanstream`, and
`std::ospanstream` in `<spanstream>`. They provide the full `iostream`
formatting interface over a caller-supplied, **fixed-size**
`std::span<char>` buffer — no dynamic allocation, no `std::string`
churn.

## Motivation
`std::stringstream` allocates and grows its buffer; hot paths that need to
format into an existing scratch buffer (a stack array, a network buffer, a
temporary arena) previously had to use `snprintf` or roll their own stream.
`spanstream` is the standard, safe answer.

## Basic Usage
```cpp
#include <spanstream>

std::array<char, 128> buf{};
std::ospanstream out(std::span{buf});

out << "id=" << 42 << ", value=" << 3.14;
auto view = std::string_view{out.span()};   // characters actually written
```
`out.span()` returns the sub-span from the start of the buffer to the current
write position — exactly the produced text.

## Reading With `std::ispanstream`
```cpp
std::string_view s = "42 3.14";
std::ispanstream in(std::span{s.data(), s.size()});
int i; double d;
in >> i >> d;
```

## Bidirectional `std::spanstream`
Combines input and output — useful for parsing-then-appending scenarios.

## `stringbuf` Analogy

| `iostream` classic | `spanstream` variant | Storage |
|---|---|---|
| `std::stringstream` | `std::spanstream` | Caller's `std::span` |
| `std::istringstream` | `std::ispanstream` | Caller's read-only span |
| `std::ostringstream` | `std::ospanstream` | Caller's writable span |

The buffer type is `std::spanbuf` (analogous to `std::stringbuf`).

## Overflow Behaviour
If the caller's span is too small, writes fail — the stream's `failbit` is set
and additional output is discarded. Always check `stream.good()` when writing
close to capacity, or size the buffer generously.

## When to Prefer `spanstream`
- Hot-path formatting into a stack / arena buffer.
- Serialising to a fixed-size network protocol frame.
- Deterministic memory usage in embedded / real-time code.
- Testing formatting without allocating.

## When to Stick With `std::stringstream` / `std::format_to`
- Unknown output size and you can allocate freely.
- Complex formatting where `std::format` + `std::string` is simpler.
- Legacy code where `stringstream` is already ingrained.

Note that `std::format_to(std::span<char>::iterator, …)` is a strong
competitor for one-shot formatting into a span, without the `iostream`
overhead.

## Availability
- GCC 12+
- Clang 20+ (libc++ landed late)
- MSVC 19.29+ (VS 2022 17.0)

## Study Checklist
- [ ] Format an ID + value pair into a `std::array<char, 128>` using `ospanstream`
- [ ] Read space-separated integers from a `std::string_view` using `ispanstream`
- [ ] Trigger overflow by writing more than the span holds — inspect `failbit`
- [ ] Compare performance with `std::stringstream` and `std::format_to`
- [ ] Compose with `std::print` (no direct integration — but you can compare speed)

## References
- [P0448 — A `<spanstream>` header](https://wg21.link/P0448)
- [cppreference — `<spanstream>`](https://en.cppreference.com/w/cpp/header/spanstream)
