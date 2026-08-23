# C++23 › `std::flat_map` and `std::flat_set`

## Overview
C++23 (**P0429** for `flat_map`, **P1222** for `flat_set`) adds *flat* variants
of the associative containers in `<flat_map>` and `<flat_set>`. They store
elements in a **sorted contiguous container** (`std::vector` by default) rather
than a red-black tree.

## Container Family

| Container | Elements | Storage |
|---|---|---|
| `std::flat_map<K, V>` | Sorted `pair<K, V>` | Two sorted `vector`s (SoA) |
| `std::flat_multimap<K, V>` | Sorted, duplicates allowed | Same as above |
| `std::flat_set<K>` | Sorted unique keys | Single sorted `vector` |
| `std::flat_multiset<K>` | Sorted, duplicates allowed | Same as above |

## Why "Flat"?
Trade **mutation cost** for **read cost** and **cache friendliness**:

| Operation | `std::map` (RB tree) | `std::flat_map` |
|---|---|---|
| `find` / lookup | `O(log n)`, pointer-chasing | `O(log n)`, contiguous binary search — much faster in practice |
| `insert` / `erase` | `O(log n)`, no shifting | `O(n)`, shifts on average |
| Iteration | Follows tree pointers | Sequential — cache-optimal |
| Memory per element | ~3 pointers + colour bit | Zero overhead beyond the pair itself |
| Iterator stability | Stable across insert/erase | Invalidated on modification |

Use case: **read-heavy, write-once** or **infrequent update** lookup tables.

## Synopsis
```cpp
#include <flat_map>
std::flat_map<std::string, int> counts;

counts["apple"] = 3;
counts["banana"] = 5;
counts.insert({"cherry", 7});

for (auto& [k, v] : counts)
    std::println("{} → {}", k, v);
```

## SoA Storage — Separate Key / Value Vectors
`std::flat_map` internally holds:
```cpp
KeyContainer   keys_;     // sorted keys
MappedContainer values_;  // parallel values
```
so a range over keys or values is a *pure contiguous scan* — great for SIMD.
The underlying containers can be customised (any random-access container):
```cpp
std::flat_map<int, double, std::less<int>, std::vector<int>, std::deque<double>>
```

## Bulk Insertion — Amortised O(n log n)
Insert many keys, then a single sort:
```cpp
std::flat_map<int, int> m;
m.insert(std::sorted_unique, first, last);       // caller guarantees sorted
```
Massively faster than `n` individual insertions.

## Iterator Invalidation
Any modification invalidates all iterators — do not hold iterators across
`insert` / `erase`.

## When to Prefer `std::flat_map`
- Small-to-medium size (< a few thousand entries).
- Read-heavy: lookups vastly outnumber modifications.
- Build once, query many times.
- Memory-sensitive: no per-node allocations.
- Cache-sensitive: linear scans in tight loops.

## When to Stick With `std::map` / `std::unordered_map`
- Many independent inserts / erases.
- Iterator stability required.
- Very large containers where `O(n)` insertion is prohibitive.

## Availability
- GCC 15+
- Clang: not yet as of late 2025 — check libc++.
- MSVC 19.40+ (VS 2022 17.10)

## Study Checklist
- [ ] Benchmark lookup: `flat_map` vs `map` vs `unordered_map` at N = 10, 1k, 100k
- [ ] Benchmark bulk insertion using `std::sorted_unique` overload
- [ ] Confirm iterator invalidation after `erase`
- [ ] Explore SoA layout via `keys()` / `values()` (if provided)
- [ ] Repeat the exercises for `flat_set`

## References
- [P0429 — `flat_map`](https://wg21.link/P0429)
- [P1222 — `flat_set`](https://wg21.link/P1222)
- [cppreference — `<flat_map>`](https://en.cppreference.com/w/cpp/container/flat_map)
- [cppreference — `<flat_set>`](https://en.cppreference.com/w/cpp/container/flat_set)
