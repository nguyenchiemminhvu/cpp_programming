# C++23 › `std::expected<T, E>`

## Overview
C++23 (**P0323**) adds `std::expected<T, E>` in `<expected>` — a value type that
holds either a successful value of type `T` **or** an error of type `E`. It is
the standard C++ analogue of Rust's `Result<T, E>`, Haskell's `Either`, and
Swift's `Result`, and offers the same benefits: **no exceptions on hot paths,
no out-parameters, no sentinel values, full monadic composition.**

## Synopsis
```cpp
#include <expected>

std::expected<int, std::string> parse(std::string_view s) {
    if (s.empty()) return std::unexpected("empty input");
    int v; auto [p, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
    if (ec != std::errc{}) return std::unexpected("not a number");
    return v;
}

if (auto r = parse("42")) {
    std::print("got {}\n", *r);
} else {
    std::print("error: {}\n", r.error());
}
```

## Core API

| Member | Meaning |
|---|---|
| `bool has_value() const` / `operator bool()` | Success flag |
| `T&    value()` / `operator*()` / `operator->()` | Access value; UB if error |
| `E&    error()` | Access error; UB if success |
| `T     value_or(U&&)` | Success or fallback |
| `E     error_or(U&&)` | Error or fallback (C++26) |

## Monadic Operations (P2505)
`std::expected` gets the same monadic surface as C++23 `std::optional`:

| Method | Purpose |
|---|---|
| `and_then(f)` | Chain if value: `f: T → expected<U, E>` |
| `transform(f)` | Map value: `f: T → U` |
| `or_else(f)` | Recover on error: `f: E → expected<T, F>` |
| `transform_error(f)` | Map error: `f: E → F` |

Example pipeline:
```cpp
auto result = read_file(path)
                  .and_then(parse_json)
                  .and_then(validate)
                  .transform(to_dto)
                  .or_else(log_error);
```

## Constructing an Error
```cpp
return std::unexpected(err);          // deduces E from err
return std::unexpected<std::string>("nope");
return std::expected<T, E>{std::unexpect, ...};
```

## Void `T`
`std::expected<void, E>` is supported — represents an operation that may fail
but yields no value:
```cpp
std::expected<void, std::error_code> commit();
```

## Design Notes
- **Value-first**: on the happy path, cost is identical to returning a `T`.
- **No dynamic allocation** unless `T` or `E` allocates.
- **No exceptions** raised by `std::expected` itself except for `bad_expected_access`
  from `value()` on an error state.
- **Trivially destructible** if both `T` and `E` are.

## Comparison

| API | Errors | Composition |
|---|---|---|
| `throw` | Exceptions | Try / catch |
| `std::optional<T>` | Only "absent" | Monadic (C++23) |
| `std::expected<T, E>` | Rich error `E` | Monadic |
| Error out-param | Ad hoc | Manual |

## Study Checklist
- [ ] Rewrite a small exception-throwing pipeline as `std::expected`
- [ ] Chain three fallible steps with `and_then`
- [ ] Compare codegen for `expected` vs exception-based error handling on the happy path
- [ ] Implement a `expected<void, error_code>` API
- [ ] Convert `std::error_code` return values into `std::expected` at API boundaries

## References
- [P0323 — `std::expected`](https://wg21.link/P0323)
- [P2505 — Monadic operations for `std::expected`](https://wg21.link/P2505)
- [cppreference — `std::expected`](https://en.cppreference.com/w/cpp/utility/expected)
