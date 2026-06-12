# C++17 › `[[nodiscard]]` & `[[maybe_unused]]`

## Overview
C++17 standardises several common attributes. The two most impactful for daily
code quality are `[[nodiscard]]` (warn on ignored return values) and
`[[maybe_unused]]` (silence unused-entity warnings).

## `[[nodiscard]]`
```cpp
[[nodiscard]] bool save(const std::string& path);

save("data.bin");           // WARNING: return value discarded
if (!save("data.bin")) { }  // OK
```
Apply to:
- Error-code returning functions
- Allocation functions
- RAII guard types: `struct [[nodiscard]] Transaction { … };`

C++20 extends it with a message: `[[nodiscard("Check the error code")]]`.

## `[[maybe_unused]]`
```cpp
[[maybe_unused]] int debug_value = expensive_compute();

void f([[maybe_unused]] int x) { }   // x unused in release build — no warning
```

## Other C++17 Attributes
| Attribute | Purpose |
|-----------|---------|
| `[[fallthrough]]` | Suppress warning in intentional switch fall-through |
| `[[deprecated("msg")]]` | C++14 — mark API for removal |

## Study Checklist
- [ ] Mark a custom `Result<T>` return type `[[nodiscard]]`; verify the warning fires
- [ ] Apply `[[nodiscard]]` to an allocation function and ignore the result
- [ ] Use `[[maybe_unused]]` for a debug-only variable gated by `#ifdef NDEBUG`
- [ ] Add `[[fallthrough]]` to an intentional switch fall-through and remove the warning

## References
- [cppreference — Attributes](https://en.cppreference.com/w/cpp/language/attributes)
