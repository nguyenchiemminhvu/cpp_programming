# Advanced › Memory Ordering (`std::memory_order`)

## Overview
Memory ordering controls how atomic operations on one thread become visible to
other threads. Weaker orderings allow more compiler/CPU reordering for better
performance; stronger orderings ensure visibility at a higher cost.

## Memory Orders
| Order | Semantics |
|-------|-----------|
| `memory_order_relaxed` | No ordering constraints — only atomicity |
| `memory_order_consume` | Carries dependency (effectively `acquire` in practice) |
| `memory_order_acquire` | No reads/writes can move before this operation |
| `memory_order_release` | No reads/writes can move after this operation |
| `memory_order_acq_rel` | Both acquire and release |
| `memory_order_seq_cst` | Total sequential consistency (default) |

## Producer-Consumer Pattern
```cpp
std::atomic<int>  data{0};
std::atomic<bool> ready{false};

// Producer
data.store(42, std::memory_order_relaxed);
ready.store(true, std::memory_order_release);   // "publishes" data

// Consumer
while (!ready.load(std::memory_order_acquire))  // "subscribes" to data
    ;   // spin
assert(data.load(std::memory_order_relaxed) == 42);  // guaranteed visible
```

## Mental Model
- `release` + `acquire` pair: a "publication" fence.
- `seq_cst`: most intuitive (total order), but most expensive.
- `relaxed`: valid for counters where exact interleaving doesn't matter.

## Study Checklist
- [ ] Implement a publish-subscribe flag using release/acquire pair
- [ ] Show that `relaxed` alone allows out-of-order visibility
- [ ] Write a sequentially-consistent message passing example
- [ ] Use `ThreadSanitizer` to detect a data race; fix with proper ordering

## References
- [cppreference — std::memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order)
- *C++ Concurrency in Action*, Anthony Williams — Chapter 5
