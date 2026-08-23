# C++23 — Study Overview
> ISO/IEC 14882:2023 · A focused refinement release building on the C++20 foundations

## Why C++23 Matters
C++23 is a smaller revision than C++20, but it fills critical usability gaps left
open by the previous cycle: **`std::print` / `std::println`** finally makes
`printf`-free I/O practical, **`std::expected<T,E>`** delivers a standard error
value type, **`std::mdspan`** enables zero-overhead multidimensional views for
numeric code, **`std::generator`** standardises coroutine generators, and the
ranges library grows a large family of new views (`zip`, `enumerate`,
`cartesian_product`, `chunk`, `slide`, …) plus the long-awaited `std::ranges::to`.
At the language level, **deducing `this`**, `if consteval`, the
multidimensional subscript operator, and `[[assume]]` polish rough edges
that had accumulated for over a decade.

---

## Study Map

### `core_language/` — Language Feature Changes
| Folder | Feature | Key Idea |
|--------|---------|---------|
| `deducing_this/` | Explicit object parameter (P0847) | `void f(this Self&& self, …)` — deduce the object type like any other parameter |
| `if_consteval/` | `if consteval` (P1938) | Detect immediate-function context without `std::is_constant_evaluated()` pitfalls |
| `multidimensional_subscript/` | `operator[](i, j, …)` (P2128) | Native multi-argument subscript for matrices, tensors, `mdspan` |
| `static_call_operator/` | Static `operator()` / `operator[]` (P1169, P2589) | Stateless callables without a `this` pointer |
| `lambda_attributes/` | Attributes on lambdas (P2173) | `[]<typename T>[[nodiscard]](T){}` — attribute the operator, not the closure |
| `labels_at_end/` | Labels at end of compound statement (P2324) | Simpler C-idiom `for` loops with trailing `end:` labels |
| `size_t_literals/` | `uz` / `z` literal suffixes (P0330) | Portable `size_t` / `ptrdiff_t` integer literals |
| `auto_decay_copy/` | `auto(x)` / `auto{x}` (P0849) | Explicit decay-copy expression — no more `[](auto x){ return x; }(v)` trick |
| `constexpr_relaxations/` | Relaxed `constexpr` (P2242, P2647) | Non-literal variables, `static` / `thread_local`, `goto`, labels in `constexpr` |
| `simplified_implicit_move/` | Implicit move on `return` (P2266) | Fewer surprises when returning parameters and by-value locals |
| `extended_floating_point/` | `std::float16_t` … `bfloat16_t` (P1467) | Named fixed-width floating-point types |
| `named_universal_char_escapes/` | `\N{LATIN SMALL LETTER A}` (P2071) | Reference Unicode characters by their canonical name |
| `delimited_escape_sequences/` | `\x{…}`, `\o{…}`, `\u{…}` (P2290) | Unambiguous, delimited numeric escapes |
| `preprocessor_directives/` | `#warning`, `#elifdef`, `#elifndef` (P2437, P2334) | Standardised preprocessor tools already common as extensions |
| `assume_attribute/` | `[[assume(expr)]]` (P1774) | Portable optimiser hint — assume an expression holds without evaluating it |
| `utf8_source_encoding/` | UTF-8 as portable source encoding (P2295) | Guaranteed cross-compiler UTF-8 source support |
| `whitespace_in_raw_strings/` | Trimming raw-string whitespace (P2178) | Consistent handling of trailing whitespace and line continuations |

### `standard_library/` — Library Additions
| Folder | Feature | Key Idea |
|--------|---------|---------|
| `print/` | `std::print`, `std::println` (P2093) | Direct-to-stream formatted output; no allocation, no `iostream` |
| `expected/` | `std::expected<T,E>` (P0323) | Value-or-error type with monadic composition |
| `mdspan/` | `std::mdspan` (P0009) | Non-owning multidimensional array view with custom layouts and accessors |
| `flat_containers/` | `std::flat_map`, `std::flat_set` (P0429, P1222) | Sorted contiguous-storage associative containers |
| `generator/` | `std::generator<T>` (P2502) | Standardised coroutine generator type |
| `ranges_to/` | `std::ranges::to` (P1206) | Materialise any range into a container |
| `ranges_new_views/` | `zip`, `enumerate`, `adjacent`, `chunk`, `slide`, `stride`, `cartesian_product`, `join_with`, `chunk_by`, `repeat`, `as_const`, `as_rvalue` | The big C++23 views expansion |
| `ranges_fold/` | `fold_left`, `fold_right`, `fold_left_first`, `fold_right_last` (P2322) | Range-based reduction with proper short-circuit semantics |
| `monadic_optional/` | `optional::and_then` / `or_else` / `transform` (P0798) | Compose fallible computations without nested `if`s |
| `string_contains/` | `std::string::contains` (P1679) | Companion to `starts_with` / `ends_with` |
| `stacktrace/` | `std::stacktrace` (P0881) | Portable stack traces from user code and exceptions |
| `spanstream/` | `std::spanstream` (P0448) | `iostream` interface over a fixed `std::span` buffer |
| `byteswap/` | `std::byteswap` (P1272) | `constexpr` endianness reversal for integer types |
| `to_underlying/` | `std::to_underlying` (P1682) | `static_cast<std::underlying_type_t<E>>(e)` in one call |
| `unreachable/` | `std::unreachable` (P0627) | Portable optimiser hint for genuinely unreachable code |
| `invoke_r/` | `std::invoke_r` (P2136) | `std::invoke` with an explicit return type |
| `bind_back/` | `std::bind_back` (P2387) | Bind the *last* arguments of a callable (dual of `bind_front`) |
| `move_only_function/` | `std::move_only_function` (P0288) | `std::function` without the copyability requirement |
| `forward_like/` | `std::forward_like` (P2445) | Forward with the value category of another object |
| `out_ptr/` | `std::out_ptr`, `std::inout_ptr` (P1132) | Ergonomic bridges to C-style out-pointer APIs from smart pointers |
| `start_lifetime_as/` | `std::start_lifetime_as` (P2590) | Legal reinterpretation between trivially-copyable types over raw storage |
| `format_ranges/` | Formatting ranges (P2286) | `std::format("{}", vec)` — direct range formatting, plus fill/align improvements |
| `resize_and_overwrite/` | `std::basic_string::resize_and_overwrite` (P1072) | Zero-init-free buffer resize for in-place fill |

---

## Additional C++23 Notes (self-study)
- **`static operator()`** enables true zero-overhead stateless functors for algorithms
- **`std::expected` monadic operations** mirror the new `std::optional` interface
- **`std::print` uses `std::format`** under the hood — same format strings, less overhead
- **`std::mdspan` is compile-time layout-parameterised** — `layout_left`, `layout_right`, `layout_stride`, or user-defined
- **`std::generator` supports both by-value and by-reference `co_yield`** and integrates with the ranges library
- **`std::forward_like` is designed to be used with the deducing-`this` idiom** to forward members with the correct value category
- **UTF-8 source encoding** finally standardises what most projects already assumed

## Toolchain Support Notes
| Feature | GCC | Clang | MSVC |
|---------|-----|-------|------|
| Deducing `this` | 14+ | 18+ | VS 2022 17.9 |
| `if consteval` | 12+ | 14+ | VS 2022 17.2 |
| Multidimensional subscript | 12+ | 15+ | VS 2022 17.6 |
| `[[assume]]` | 13+ | 19+ | VS 2022 17.2 |
| `std::print` | 14+ | 18+ (libc++) | VS 2022 17.10 |
| `std::expected` | 12+ | 16+ (libc++) | VS 2022 17.3 |
| `std::mdspan` | 13+ | 18+ | VS 2022 17.9 |
| `std::flat_map` / `flat_set` | 15+ | — | VS 2022 17.10 |
| `std::generator` | 14+ | — | VS 2022 17.13 |
| `std::ranges::to` & new views | 14+ | 17+ | VS 2022 17.7 |
| `std::stacktrace` | 12+ | — | VS 2022 17.4 |

Compiler support for C++23 is uneven — check
[cppreference compiler support](https://en.cppreference.com/w/cpp/compiler_support/23)
before committing to a feature. When a feature is unavailable, the study folder
still documents the intended usage.

## Recommended Resources
- [cppreference — C++23](https://en.cppreference.com/w/cpp/23)
- *C++23 STL Cookbook* — Bill Weinman
- *Programming with C++23* — Slobodan Dmitrović
- [P2000 — C++23 status](https://wg21.link/p2000) — the master feature list
- [C++ Weekly with Jason Turner](https://www.youtube.com/@cppweekly) — running C++23 series
