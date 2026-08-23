# C++23 › Monadic `std::optional`

## Overview
C++23 (**P0798**) adds monadic member functions to `std::optional`:
`and_then`, `transform`, and `or_else`. These make it easy to chain fallible
operations without nested `if (opt)` blocks — the same style already familiar
from Rust `Option`, Haskell `Maybe`, and Swift `Optional`.

## The New Members

| Method | Signature (essence) | Purpose |
|---|---|---|
| `and_then(f)` | `f: T → optional<U>` → `optional<U>` | Chain when present; short-circuit on `nullopt`. |
| `transform(f)` | `f: T → U` → `optional<U>` | Map the contained value. |
| `or_else(f)` | `f: () → optional<T>` → `optional<T>` | Recover when absent. |

Each method returns `nullopt` propagated when the input is empty (or, for
`or_else`, when both the input *and* the recovery return `nullopt`).

## Example Pipeline
```cpp
std::optional<std::string> get_user_name(int id);
std::optional<int>         parse_score(const std::string&);

auto score = get_user_name(42)
                .and_then(parse_score)                            // optional<int>
                .transform([](int s){ return s + 10; })           // optional<int>
                .or_else([]{ return std::optional{0}; });          // fallback
```
Compare to the C++17 idiom:
```cpp
std::optional<int> score;
if (auto name = get_user_name(42))
    if (auto s = parse_score(*name))
        score = *s + 10;
if (!score) score = 0;
```

## Interaction With `std::expected`
`std::expected<T, E>` in C++23 offers the analogous monadic surface plus
`transform_error`. When you need only "present or absent," use `optional`;
when you need a *reason* for absence, use `expected`.

## Details

### `and_then`
```cpp
auto opt = std::optional<int>{3};
auto out = opt.and_then([](int x) -> std::optional<double> {
    return x > 0 ? std::optional{std::sqrt(x)} : std::nullopt;
});
```

### `transform`
```cpp
auto opt = std::optional<int>{3};
auto out = opt.transform([](int x){ return x * x; });   // optional<int>{9}
```

### `or_else`
```cpp
std::optional<int> opt = std::nullopt;
auto out = opt.or_else([]{ return std::optional<int>{-1}; });   // optional<int>{-1}
```

## Value Category Overloads
All three come in `&`, `const&`, `&&`, `const&&` overloads — so `std::move(opt).transform(...)`
moves the contained value into the callable without an extra copy.

## Pitfalls
- The callable **must** return an `optional` for `and_then` and `or_else`;
  otherwise the pipeline is a compile error.
- Do not `throw` from these callables in performance-critical paths — use
  `expected` and `transform_error` instead.
- Chained `transform`s of expensive-to-move types can be surprising — consider
  moving explicitly.

## Study Checklist
- [ ] Rewrite a nested `if (opt)` chain using `and_then` + `transform` + `or_else`
- [ ] Provoke each of the three short-circuit cases and verify the result
- [ ] Move an `optional<std::string>` through `.transform(...)` — verify no copy
- [ ] Compare readability and codegen with an `std::expected`-based equivalent
- [ ] Extend `std::optional`-like monadic thinking to `std::variant` (spoiler: it's harder)

## References
- [P0798 — Monadic operations for `std::optional`](https://wg21.link/P0798)
- [cppreference — `std::optional`](https://en.cppreference.com/w/cpp/utility/optional)
