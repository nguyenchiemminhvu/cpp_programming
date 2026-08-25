# CASTLE

**Common Automotive Safety-critical Template Library for Embedded**

CASTLE is a lightweight, header-only C++17 library built for embedded systems with limited resources. It is inspired by the [ETL (Embedded Template Library)](https://www.etlcpp.com/docs/) and designed to replace heavy enterprise frameworks in automotive products.

The core idea is simple: give embedded developers useful, safe building blocks — without dynamic memory allocation, without exceptions, and without hidden costs.

---

## Why CASTLE?

Most C++ standard library features (`std::function`, `std::string`, logging frameworks, etc.) rely on heap allocation. That is often not acceptable in safety-critical embedded environments where memory is tight and behavior must be deterministic.

CASTLE solves this by providing **stack-allocated, fixed-capacity alternatives** that are predictable, fast, and safe.

---

## Features

- **Header-only** — no build system needed, just add to your include path
- **No heap allocation on the hot path** — all core components use stack or static storage
- **Deterministic behavior** — no exceptions, no RTTI, explicit overflow handling
- **`constexpr` and `noexcept`** — compile-time evaluation where possible, zero-overhead at runtime

## Requirements

- C++17 or later
- A compiler that supports `constexpr`, `if constexpr`, `std::string_view`, and fold expressions (GCC 7+, Clang 5+, MSVC 19.14+)

---

## Roadmap

CASTLE is actively growing. More modules are planned to cover common needs in automotive embedded systems — containers, state machines, CRC, checksums, and more.

---

## License

MIT License © 2026 nguyenchiemminhvu
