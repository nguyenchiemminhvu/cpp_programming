# C++17 › Polymorphic Memory Resources (`std::pmr`)

## Overview
`std::pmr` (Polymorphic Memory Resources, `<memory_resource>`) decouples
containers from allocators using **runtime polymorphism** rather than template
parameters, enabling monotonic/pool allocators to be swapped transparently.

## Core Types
| Type | Role |
|------|------|
| `std::pmr::memory_resource` | Abstract base for all allocators |
| `std::pmr::monotonic_buffer_resource` | Bump allocator — fast, no individual frees |
| `std::pmr::unsynchronized_pool_resource` | Pool for small objects, single-threaded |
| `std::pmr::synchronized_pool_resource` | Thread-safe pool |
| `std::pmr::polymorphic_allocator<T>` | STL-compatible allocator wrapping a `memory_resource*` |

## Usage Pattern
```cpp
#include <memory_resource>
#include <vector>

// Stack buffer — avoids heap allocation entirely
char buffer[4096];
std::pmr::monotonic_buffer_resource pool{buffer, sizeof(buffer)};

std::pmr::vector<int> v{&pool};   // vector backed by stack buffer
for (int i = 0; i < 100; ++i) v.push_back(i);
// All allocations come from buffer — no heap traffic
```

## Allocator Propagation
`pmr` containers propagate their allocator through nested containers via
`polymorphic_allocator`, avoiding the template explosion of the traditional
allocator model.

## Study Checklist
- [ ] Use `monotonic_buffer_resource` on a stack buffer for a hot loop
- [ ] Measure heap allocation count: standard `vector` vs `pmr::vector`
- [ ] Chain resources: pool → `null_memory_resource` (throws on overflow)
- [ ] Implement a scoped allocator that resets a monotonic pool each frame

## References
- [cppreference — pmr](https://en.cppreference.com/w/cpp/memory/memory_resource)
