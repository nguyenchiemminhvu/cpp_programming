# C++11 › `alignas` & `alignof`

## Overview
`alignas` specifies the alignment of a type or variable. `alignof` queries
the alignment requirement of a type at compile time. They replace non-portable
`__attribute__((aligned(N)))` and `__declspec(align(N))`.

## `alignof` — Query Alignment
```cpp
alignof(int)               // typically 4
alignof(double)            // typically 8
alignof(std::max_align_t)  // maximum fundamental alignment
```

## `alignas` — Specify Alignment
```cpp
alignas(16) float simd_buffer[4];          // 16-byte aligned for SIMD

struct alignas(64) CacheLine {             // one cache-line-sized struct
    char data[64];
};

static_assert(alignof(CacheLine) == 64);
```

## With `std::aligned_storage` / Placement New
```cpp
alignas(T) unsigned char storage[sizeof(T)];
new (storage) T{args};   // placement new into aligned buffer
```

## Study Checklist
- [ ] Query `alignof` for all fundamental types and compare
- [ ] Create a cache-line-aligned struct; verify with `alignof`
- [ ] Use `alignas` + placement new for a manual aligned storage buffer
- [ ] Demonstrate a SIMD-friendly `alignas(16) float[4]` array

## References
- [cppreference — alignas](https://en.cppreference.com/w/cpp/language/alignas)
- [cppreference — alignof](https://en.cppreference.com/w/cpp/language/alignof)
