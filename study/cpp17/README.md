# C++17 — Study Overview
> ISO/IEC 14882:2017 · Substantial language and library additions

## Why C++17 Matters
C++17 is the first standard that delivers genuinely new programming paradigms
beyond C++11 refinements: structured bindings enable destructuring assignment,
`if constexpr` eliminates SFINAE boilerplate, and the vocabulary types
(`optional`, `variant`, `any`) standardise patterns previously spread across
many independent libraries (Boost, Abseil, etc.).

---

## Study Map

### `core_language/` — Language Feature Changes
| Folder | Feature | Key Idea |
|--------|---------|---------|
| `structured_bindings/` | `auto [a,b] = expr` | Destructuring assignment for pairs, tuples, structs |
| `if_constexpr/` | `if constexpr (cond)` | Compile-time branch selection in templates |
| `inline_variables/` | `inline` variables | Header-only variable definitions without ODR violations |

### `standard_library/` — Library Additions
| Folder | Feature | Key Idea |
|--------|---------|---------|
| `optional/` | `std::optional<T>` | Type-safe nullable value |
| `variant/` | `std::variant<Ts…>` | Type-safe tagged union |
| `any/` | `std::any` | Type-erased value container |
| `filesystem/` | `std::filesystem` | Portable file system operations |
| `string_view/` | `std::string_view` | Non-owning read-only string reference |

---

## Additional C++17 Topics (self-study)
- **Fold expressions** — `(args + …)` over parameter packs
- **Class template argument deduction (CTAD)** — `std::pair p{1, 2.0};`
- **`if` / `switch` with initializer** — `if (auto it = m.find(k); it != m.end())`
- **`[[nodiscard]]`, `[[maybe_unused]]`, `[[fallthrough]]`** attributes
- **`std::invoke`** — uniform callable invocation
- **`std::apply`** — call a function with a tuple of arguments
- **Parallel algorithms** — `std::sort(std::execution::par, …)`
- **`std::byte`** — explicit byte type, not an integer
- **`std::clamp`** — clamp a value to a range
- **Guaranteed copy elision (RVO)** — mandatory in C++17

## Recommended Resources
- [cppreference C++17](https://en.cppreference.com/w/cpp/17)
- *C++17 in Detail* — Bartłomiej Filipek
