# C++23 › `std::mdspan` — Non-Owning Multidimensional View

## Overview
C++23 (**P0009**) introduces `std::mdspan<T, Extents, LayoutPolicy, AccessorPolicy>`
in `<mdspan>` — a non-owning, zero-overhead multidimensional view over a
contiguous or strided memory region. It is to matrices and tensors what
`std::span` is to 1-D arrays.

## Anatomy
```cpp
template <
    class ElementType,
    class Extents,                  // std::extents<IndexType, E0, E1, …>
    class LayoutPolicy   = std::layout_right,
    class AccessorPolicy = std::default_accessor<ElementType>
>
class mdspan;
```
- **Extents** — compile-time and/or runtime dimensions (`std::dynamic_extent`).
- **LayoutPolicy** — index → offset mapping. Built-in: `layout_right` (row-major),
  `layout_left` (column-major), `layout_stride` (custom strides).
- **AccessorPolicy** — customises `operator[]` — e.g. atomic access, `restrict`
  pointers, or type-punning bridges.

## Basic Usage
```cpp
#include <mdspan>

std::vector<double> buf(3 * 4);
std::mdspan<double, std::extents<std::size_t, 3, 4>> m(buf.data());

m[1, 2] = 7.0;                              // C++23 multi-arg subscript
for (std::size_t r = 0; r < m.extent(0); ++r)
    for (std::size_t c = 0; c < m.extent(1); ++c)
        std::print("{:6.2f} ", m[r, c]);
```

## Dynamic Extents
```cpp
using mat = std::mdspan<double,
    std::extents<std::size_t, std::dynamic_extent, std::dynamic_extent>>;

mat m(buf.data(), 3, 4);                    // rows, cols known at runtime
```
Or the convenient alias `std::dextents<std::size_t, 2>`.

## Submdspans (P2630, C++26)
`std::submdspan` provides slicing — currently a C++26 addition, but many
implementations already ship an experimental preview. It supports full slices,
strided slices, and index reductions:
```cpp
// C++26 preview
auto row = std::submdspan(m, 1, std::full_extent);
```

## Layout Customisation
```cpp
// Column-major view over the same buffer
using col_mat = std::mdspan<double,
    std::dextents<std::size_t, 2>,
    std::layout_left>;
col_mat cm(buf.data(), 3, 4);
```

## Accessor Customisation
Common patterns:
- `std::default_accessor<T>` — plain pointer access.
- User-supplied atomic accessor for lock-free tensors.
- Restrict / aligned accessors for HPC codegen.

## Interaction With Existing Types
- Construct from `std::span<T>`.
- `mdspan` itself is a **view** — cheap to copy, does not own storage.
- Pairs perfectly with `std::mdarray` (planned C++26) as the owning counterpart.

## Availability
- GCC 13+
- Clang 18+ (libc++)
- MSVC 19.39+ (VS 2022 17.9)

## Study Checklist
- [ ] Construct an `mdspan` over `std::vector<double>` with static extents
- [ ] Repeat with `dextents` and runtime extents
- [ ] Compare row-major vs column-major traversal performance
- [ ] Write an `mdspan`-based matrix-multiply kernel
- [ ] Implement a custom accessor that clamps out-of-range writes (educational only)

## References
- [P0009 — `mdspan`: A non-owning multidimensional array reference](https://wg21.link/P0009)
- [cppreference — `std::mdspan`](https://en.cppreference.com/w/cpp/container/mdspan)
- [kokkos/mdspan](https://github.com/kokkos/mdspan) — reference implementation
