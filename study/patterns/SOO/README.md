````markdown
# Design Pattern › SOO — Small Object Optimization

## Overview
Small Object Optimization (also called Small Buffer Optimization, SBO) is a
technique where an object stores small payloads **inline**, inside its own
memory footprint, instead of allocating on the heap. Only when the payload
exceeds an inline capacity threshold does the object fall back to a heap
allocation. This avoids the cost of `new`/`delete` (allocator calls, cache
misses, fragmentation) for the common case of small values.

It is the reason types like `std::string` (SSO), `std::function`, and
`std::any` are fast for small payloads but still support arbitrary sizes.

## Why It Matters
- Heap allocation is relatively expensive: allocator bookkeeping, potential
  syscalls, cache-unfriendly pointer chasing.
- Many real-world values are small (short strings, small closures, small
  numeric types wrapped in `std::any`/`std::variant`-like erasure).
- Keeping data inline improves cache locality and reduces overhead
  dramatically for the hot path.

## Small String Optimization (SSO) — The Classic Example
Most standard library implementations avoid heap allocation for short
strings by storing characters directly inside the `std::string` object:

```cpp
std::string s = "hello";       // fits in inline buffer -> no heap allocation
std::string big(100, 'x');     // exceeds inline capacity -> heap allocation

std::cout << s.capacity();     // often 15 on libstdc++/libc++ (implementation-defined)
```

Layout idea (simplified, libc++/libstdc++ style union):
```cpp
struct sso_string {
    union {
        struct { char* data; size_t cap; } heap;
        char inline_buf[16];   // small strings live here directly
    };
    size_t size;
    bool is_heap;
};
```

## Implementing a Minimal SOO Container
A generic small-buffer-optimized value holder:

```cpp
#include <cstddef>
#include <new>
#include <utility>
#include <type_traits>

template <std::size_t BufferSize = 16>
class small_buffer {
public:
    template <typename T>
    small_buffer(T value)
    {
        if constexpr (sizeof(T) <= BufferSize && alignof(T) <= alignof(std::max_align_t))
        {
            ::new (static_cast<void*>(&storage_)) T(std::move(value));
            on_heap_ = false;
        }
        else
        {
            ptr_ = new T(std::move(value));
            on_heap_ = true;
        }
    }

    // Access requires knowing T (a real implementation would type-erase
    // via a vtable/function-pointer, similar to std::function/std::any).
private:
    union {
        std::aligned_storage_t<BufferSize> storage_;
        void* ptr_;
    };
    bool on_heap_ = false;
};
```

In production code (e.g. `std::function`, `std::any`), the small buffer is
paired with **type erasure**: a small vtable of function pointers
(construct/destroy/copy/move/invoke) that knows how to manipulate whatever
type is stored — inline or on the heap — through a uniform interface.

## Key Design Considerations
| Aspect | Consideration |
|--------|--------------|
| Buffer size | Trade-off: bigger buffer = more inline hits, but larger object footprint always paid |
| Alignment | Inline storage must satisfy the stored type's `alignof` requirement |
| Move/Copy | Must correctly move/copy either the inline object or the heap pointer |
| Exception safety | Placement-new construction must handle exceptions from `T`'s constructor |
| Type erasure | Needed if the container must hold heterogeneous types (see `type_erasure/`) |

## Real-World Examples
- `std::string` — Small String Optimization (SSO)
- `std::function` — small callables (captures) stored inline
- `std::any` — small trivially-copyable values stored inline
- `boost::small_vector` / `llvm::SmallVector` — inline element storage

## Study Checklist
- [ ] Measure `sizeof(std::string)` and find your standard library's SSO threshold
- [ ] Write a benchmark comparing heap-allocated vs inline-stored small objects
- [ ] Extend `small_buffer` above with type erasure (construct/destroy/move vtable)
- [ ] Explain why alignment matters when reusing the inline buffer for arbitrary `T`
- [ ] Compare SOO in `std::function` vs a naive `std::function` that always heap-allocates

## References
- [cppreference — std::string](https://en.cppreference.com/w/cpp/string/basic_string)
- [cppreference — std::any](https://en.cppreference.com/w/cpp/utility/any)
- [LLVM — SmallVector](https://llvm.org/doxygen/classllvm_1_1SmallVector.html)
- Sean Parent — *Better Code: Runtime Polymorphism* (talks about SOO + type erasure together)

````