# C++20 › `std::latch`, `std::barrier` & `std::counting_semaphore`

## Overview
C++20 adds three new synchronisation primitives for common concurrent
coordination patterns, available in `<latch>`, `<barrier>`, and `<semaphore>`.

## `std::latch` — Single-Use Countdown
A latch counts down from N to 0; threads wait until it reaches 0.
**Non-reusable** — once it reaches 0 it stays there.
```cpp
std::latch done{3};   // count = 3

void worker() { do_work(); done.count_down(); }

done.wait();          // blocks until count == 0
```

## `std::barrier<F>` — Reusable Phase Barrier
Synchronises N threads at a rendezvous point, then resets for the next phase.
Optionally calls a completion function between phases.
```cpp
std::barrier sync{N, []() noexcept { advance_phase(); }};

void worker() {
    while (!done) {
        do_phase_work();
        sync.arrive_and_wait();   // wait for all N threads
    }
}
```

## `std::counting_semaphore<N>` — Resource Counting
Limits concurrent access to a resource pool.
```cpp
std::counting_semaphore<4> pool{4};   // max 4 concurrent holders

void use_resource() {
    pool.acquire();      // decrements; blocks if count == 0
    critical_section();
    pool.release();      // increments; wakes waiting thread
}
// std::binary_semaphore = counting_semaphore<1>
```

## Study Checklist
- [ ] Use `latch` to wait for N async tasks to complete
- [ ] Implement parallel matrix multiply using `barrier` for phase synchronisation
- [ ] Build a connection pool with `counting_semaphore`
- [ ] Show why `latch` is non-reusable; demonstrate `barrier` phase reset

## References
- [cppreference — latch](https://en.cppreference.com/w/cpp/thread/latch)
- [cppreference — barrier](https://en.cppreference.com/w/cpp/thread/barrier)
- [cppreference — counting_semaphore](https://en.cppreference.com/w/cpp/thread/counting_semaphore)
