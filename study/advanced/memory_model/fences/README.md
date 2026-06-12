# Advanced › Memory Fences (`std::atomic_thread_fence`)

## Overview
Fences are standalone memory barriers that impose ordering on **surrounding**
atomic and non-atomic operations, without being tied to a specific atomic
variable — giving finer-grained control over memory ordering.

## Types
```cpp
std::atomic_thread_fence(std::memory_order_acquire);   // acquire fence
std::atomic_thread_fence(std::memory_order_release);   // release fence
std::atomic_thread_fence(std::memory_order_seq_cst);   // full fence
```

## Fence vs Per-Operation Ordering
```cpp
// Per-operation (common pattern):
flag.store(true, std::memory_order_release);

// Equivalent with fence:
std::atomic_thread_fence(std::memory_order_release);
flag.store(true, std::memory_order_relaxed);
```

## Use Cases
- Protecting **non-atomic** writes before publishing a flag.
- Grouping multiple stores under a single fence rather than individual release stores.
- Low-level algorithm design where granular barrier placement is needed.

## `std::atomic_signal_fence`
Like `atomic_thread_fence` but only prevents reordering between **signal handlers**
and the calling thread — no hardware fence instruction emitted.

## Study Checklist
- [ ] Replace a release store with a fence + relaxed store; verify equivalence
- [ ] Use an acquire fence to protect reading non-atomic data after a relaxed check
- [ ] Show the cost difference: seq_cst fence vs acquire/release fence
- [ ] Implement a sequence lock using fences

## References
- [cppreference — atomic_thread_fence](https://en.cppreference.com/w/cpp/atomic/atomic_thread_fence)
