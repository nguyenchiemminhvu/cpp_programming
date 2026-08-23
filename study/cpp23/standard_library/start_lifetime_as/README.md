# C++23 › `std::start_lifetime_as`

## Overview
C++23 (**P2590**) adds `std::start_lifetime_as<T>(void*)` in `<memory>` — a
standard, well-defined way to **begin the lifetime** of an implicit-lifetime
object over raw storage without invoking a constructor. It is the answer to
one of the most common questions in low-level C++: "How do I legally
reinterpret this buffer as a `T`?"

## The Problem It Solves
Reading a struct out of a byte buffer (from disk, network, mmap, DMA) has
historically been UB in strict C++:
```cpp
std::vector<std::byte> buf = /* read from wire */;
// ❌ pre-C++23: no object of type packet_header exists at buf.data()
auto* hdr = reinterpret_cast<packet_header*>(buf.data());
auto  id  = hdr->id;
```
Even for **trivially copyable** types, the abstract machine sees no object at
that address — so a compiler is free to assume anything. In practice most
compilers do the right thing, but the standard was silent.

## The C++23 Rule
For any **implicit-lifetime type** `T` (trivially copyable, aggregates,
arrays of implicit-lifetime types), `std::start_lifetime_as<T>(p)` starts a
new `T` object at `p`, reusing the storage's byte representation, and returns
a `T*` pointing to it.

```cpp
auto* hdr = std::start_lifetime_as<packet_header>(buf.data());
auto  id  = hdr->id;              // ✅ well-defined
```

## Signatures
```cpp
namespace std {
    template <class T>
    T*       start_lifetime_as(void* p) noexcept;
    template <class T>
    const T* start_lifetime_as(const void* p) noexcept;

    template <class T>
    T*       start_lifetime_as_array(void* p, std::size_t n) noexcept;
    template <class T>
    const T* start_lifetime_as_array(const void* p, std::size_t n) noexcept;
}
```

## Constraints
- `T` must be an **implicit-lifetime type** (`std::is_implicit_lifetime_v<T>`
  is a C++23 trait).
- Storage must be sufficient in size and alignment.
- The bytes already at `p` become the object representation of the new `T`.
  Reading a field then interprets *those bytes* — the same values that were
  in the buffer.
- Existing objects at `p` are *ended* (their lifetime terminates).

## Arrays
```cpp
auto* pixels = std::start_lifetime_as_array<pixel>(buf.data(), width * height);
```

## When to Use It

| Scenario | Preferred tool |
|---|---|
| Zero-copy parsing of a wire-format struct | `std::start_lifetime_as` |
| Interpret a `mmap`'d file as an array of records | `std::start_lifetime_as_array` |
| Reinterpret one struct as another (same layout) | Prefer `std::bit_cast` (copies but always safe) |
| Access as an array of bytes | Just use `std::byte*` — no lifetime call needed |

## Contrast With Related APIs

| API | Copies bytes? | Starts new object? | Use case |
|---|---|---|---|
| `reinterpret_cast<T*>(p)` | No | No — still UB without lifetime | Legacy code |
| `std::bit_cast<T>(x)` | Yes | Yes, as a return value | Safe copy-reinterpretation |
| `std::start_lifetime_as<T>(p)` | No | Yes | Zero-copy re-typing of raw storage |
| `new (p) T(...)` placement new | No, but invokes constructor | Yes | Constructing a non-trivial object |

## Availability
- GCC 15+
- Clang: not yet in mainstream libc++
- MSVC 19.40+ (VS 2022 17.10)

## Study Checklist
- [ ] Parse a fixed-format packet header from a byte buffer with `start_lifetime_as`
- [ ] Interpret a `mmap` region as an array of records with `start_lifetime_as_array`
- [ ] Confirm the trait `std::is_implicit_lifetime_v<T>` for aggregates and integers
- [ ] Contrast with `std::bit_cast` for a small trivially-copyable struct
- [ ] Verify that changing the buffer bytes changes the observed field values

## References
- [P2590 — Explicit lifetime management](https://wg21.link/P2590)
- [cppreference — `std::start_lifetime_as`](https://en.cppreference.com/w/cpp/memory/start_lifetime_as)
