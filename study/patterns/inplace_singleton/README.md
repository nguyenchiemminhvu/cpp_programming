# Design Pattern › InplaceSingleton — Singleton Without Heap Allocation

## What Problem Are We Solving?

The Singleton pattern ensures that exactly **one instance** of a class exists during the program's lifetime. In standard C++ there are two common ways to implement it, and both have serious problems in safety-critical or embedded environments.

### The Classic Heap-Based Singleton

```cpp
class Logger {
public:
    static Logger& instance() {
        if (!ptr_) ptr_ = new Logger();  // heap allocation!
        return *ptr_;
    }
private:
    static Logger* ptr_;
};
```

**Problems:**
- `new` calls the global allocator — this is **non-deterministic** (it may block, call OS system calls, or fail).
- In bare-metal or RTOS environments, the heap may not exist or may be severely limited.
- `new` can throw `std::bad_alloc`; embedded toolchains often disable exceptions.
- The object is **never destroyed** (unless you add explicit `delete` logic), causing resource leaks.

### The Meyers Singleton (Static Local Variable)

```cpp
class Logger {
public:
    static Logger& instance() {
        static Logger obj;  // constructed on first call
        return obj;
    }
};
```

**Problems:**
- **Thread safety** is compiler-defined (C++11 mandates it, but some embedded compilers lag behind or require runtime support).
- Lifetime is tied to program termination — **order of destruction** across translation units is undefined (the "static initialization order fiasco").
- You have **no explicit control** over when construction and destruction happen — critical in systems with strict sequencing requirements (e.g., hardware drivers that must be initialized in a specific order).

### The Solution: Explicit Lifecycle, Zero Heap

`InplaceSingleton` solves all of the above by storing the managed object in a **statically allocated, properly aligned byte buffer** inside the template class. Lifecycle is controlled through explicit `create()` and `destroy()` calls — making it fully deterministic and safe-critical compliant.

---

## Understanding the Core Mechanism

Before examining the full implementation, you need to understand three low-level C++ tools that make this pattern possible.

### `std::aligned_storage` — A Type-Safe Raw Buffer

```cpp
using storage_type =
    typename std::aligned_storage<sizeof(T), alignof(T)>::type;
```

`std::aligned_storage<Size, Align>` produces a **POD type** whose size and alignment match those of `T`. It is essentially a raw byte buffer that the compiler guarantees is correctly aligned for placement of any object of type `T`.

Why not use `char buffer[sizeof(T)]`? Because `char` arrays have alignment of 1 — placing a `double` or a SIMD type into one is **undefined behaviour** on platforms that enforce strict alignment (ARM Cortex-M, RISC-V). `std::aligned_storage` eliminates this hazard.

> **Note (C++23):** `std::aligned_storage` is deprecated in C++23. The modern replacement is a plain `alignas(T) std::byte buffer[sizeof(T)]`. The principle is identical.

### Placement New — Construction Without Allocation

```cpp
new (&storage_) value_type(std::forward<Args>(args)...);
```

Placement new constructs an object **at a given memory address**. No memory is allocated — the storage already exists as a static class member. This is how `InplaceSingleton` builds the managed object entirely within the pre-allocated buffer.

### Explicit Destructor Call — Destruction Without Deallocation

```cpp
reinterpret_cast<value_type*>(&storage_)->~value_type();
```

Because the object was not created with `new`, it cannot be deleted with `delete`. Instead, the destructor is called manually. This is correct and well-defined C++. It releases all resources held by the object (file handles, mutexes, RAII members) without touching the allocator.

---

## Step-by-Step Implementation Walkthrough

### Step 1: The Static Storage Buffer

```cpp
using storage_type =
    typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type;

static storage_type storage_;
static bool         is_initialized_;
```

Both members are `static` — they live in the **BSS or data segment** at program start, with no runtime allocation. Their lifetime spans the entire program, just like a global variable, but access is controlled through the class interface.

`is_initialized_` is the guard flag. It prevents double-construction and use-before-init — two of the most dangerous defects in safety-critical software.

### Step 2: The `create()` Method

```cpp
template <typename... Args>
static void create(Args&&... args)
{
    SINGLETON_ASSERT(!is_initialized_, "Singleton instance is already initialized.");
    new (&storage_) value_type(std::forward<Args>(args)...);
    is_initialized_ = true;
}
```

`create()` accepts a variadic argument pack and **perfect-forwards** it to the constructor of `value_type`. This means you can initialize any object — simple or complex — without needing a default constructor.

The assert fires if `create()` is called a second time. In debug builds this is a hard abort. In production builds you can replace `SINGLETON_ASSERT` with a no-op or a custom fault handler (see the `#define` hook below).

**Setting `is_initialized_ = true` after construction** (not before) ensures that if the constructor throws, the guard remains `false` and the object is not falsely considered valid.

### Step 3: The `instance()` Method

```cpp
static T& instance()
{
    SINGLETON_ASSERT(is_initialized_, "Singleton instance is not initialized.");
    return *reinterpret_cast<value_type*>(&storage_);
}
```

`reinterpret_cast<value_type*>(&storage_)` converts the address of the raw buffer to a typed pointer. This is safe and well-defined **because** we used `std::aligned_storage` (alignment is correct) and the object was constructed there with placement new (lifetime has begun).

The assert prevents the worst Singleton anti-pattern: accessing the instance before `create()` is called. In a bare-metal system this would otherwise silently read uninitialised memory.

### Step 4: The `destroy()` Method

```cpp
static void destroy()
{
    SINGLETON_ASSERT(is_initialized_, "Singleton instance is not initialized.");
    reinterpret_cast<value_type*>(&storage_)->~value_type();
    is_initialized_ = false;
}
```

Calling `destroy()` gives you **deterministic teardown**. You decide exactly when the managed object's destructor runs — not the linker, not the runtime, not the OS. This is essential for:

- Hardware drivers that must be shut down in a defined order before power-off.
- Network or file resources that must be flushed before the system halts.
- Unit tests that need a fresh singleton instance for each test case.

Setting `is_initialized_ = false` after destruction means `create()` can be legally called again, enabling full recreation in test harnesses.

### Step 5: The Customisable Assert Hook

```cpp
#ifndef SINGLETON_ASSERT
    #define SINGLETON_ASSERT(cond, msg) assert((cond) && (msg))
#endif
```

This is one of the most important features for embedded and safety-critical use. The default falls back to the C standard `assert`, which aborts the program. But in production systems you may need to:

- Log the fault to a persistent ring buffer before halting.
- Trigger a watchdog reset.
- Report the error to a safety supervisor process.
- Do nothing (for performance-optimised release builds).

You override the behavior by defining `SINGLETON_ASSERT` **before** including the header:

```cpp
#define SINGLETON_ASSERT(cond, msg) \
    do { if (!(cond)) safety_fault_handler(msg); } while(0)

#include "singleton.h"
```

---

## The Complete Implementation

```cpp
#pragma once

#include <cassert>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

#ifndef SINGLETON_ASSERT
    #define SINGLETON_ASSERT(cond, msg) assert((cond) && (msg))
#endif

template <typename T>
class singleton
{
public:
    using value_type = T;

    template <typename... Args>
    static void create(Args&&... args)
    {
        SINGLETON_ASSERT(!is_initialized_, "Singleton instance is already initialized.");
        new (&storage_) value_type(std::forward<Args>(args)...);
        is_initialized_ = true;
    }

    static void destroy()
    {
        SINGLETON_ASSERT(is_initialized_, "Singleton instance is not initialized.");
        reinterpret_cast<value_type*>(&storage_)->~value_type();
        is_initialized_ = false;
    }

    static T& instance()
    {
        SINGLETON_ASSERT(is_initialized_, "Singleton instance is not initialized.");
        return *reinterpret_cast<value_type*>(&storage_);
    }

    static bool is_initialized() noexcept
    {
        return is_initialized_;
    }

private:
    singleton() = delete;
    ~singleton() = delete;
    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;
    singleton(singleton&&) = delete;
    singleton& operator=(singleton&&) = delete;

    using storage_type =
        typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type;

    static storage_type storage_;
    static bool         is_initialized_;
};

template <typename T>
typename singleton<T>::storage_type singleton<T>::storage_{};

template <typename T>
bool singleton<T>::is_initialized_ = false;
```

---

## Usage Examples

### Example 1: Simple Logger

```cpp
#include "singleton.h"
#include <cstdio>

class Logger {
public:
    explicit Logger(const char* prefix) : prefix_(prefix) {}

    void log(const char* msg) const {
        std::printf("[%s] %s\n", prefix_, msg);
    }
private:
    const char* prefix_;
};

int main() {
    singleton<Logger>::create("APP");       // construct in-place, no heap

    singleton<Logger>::instance().log("System started");
    singleton<Logger>::instance().log("Running...");

    singleton<Logger>::destroy();           // explicit teardown
}
```

Output:
```
[APP] System started
[APP] Running...
```

### Example 2: Hardware Peripheral Driver (Embedded Use Case)

```cpp
class UartDriver {
public:
    UartDriver(uint32_t baud_rate, uint8_t port)
        : baud_rate_(baud_rate), port_(port)
    {
        hw_uart_init(port_, baud_rate_);    // configure hardware registers
    }

    ~UartDriver() {
        hw_uart_deinit(port_);              // release hardware on teardown
    }

    void send(const uint8_t* data, size_t len) { /* ... */ }

private:
    uint32_t baud_rate_;
    uint8_t  port_;
};

// In system initialisation (called before scheduler starts):
void system_init() {
    singleton<UartDriver>::create(115200U, 1U);
}

// In ISR or task:
void on_send_request(const uint8_t* buf, size_t len) {
    singleton<UartDriver>::instance().send(buf, len);
}

// In system shutdown:
void system_shutdown() {
    singleton<UartDriver>::destroy();       // hw_uart_deinit called here
}
```

No heap involved — the `UartDriver` object lives in the `.bss` segment.

### Example 3: Non-Default-Constructible Type

```cpp
struct Config {
    Config(int timeout_ms, bool verbose)
        : timeout_ms(timeout_ms), verbose(verbose) {}

    int  timeout_ms;
    bool verbose;
};

// Config has no default constructor — that's fine, create() forwards args:
singleton<Config>::create(500, true);

auto& cfg = singleton<Config>::instance();
// cfg.timeout_ms == 500, cfg.verbose == true
```

### Example 4: Using `is_initialized()` for Safe Conditional Access

```cpp
void maybe_log(const char* msg) {
    if (singleton<Logger>::is_initialized()) {
        singleton<Logger>::instance().log(msg);
    }
}
```

This is safe during early boot or late shutdown phases when the singleton may not yet be (or no longer be) alive.

### Example 5: Reset in Unit Tests

```cpp
TEST(LoggerTest, ResetBetweenTests) {
    singleton<Logger>::create("TEST1");
    singleton<Logger>::instance().log("first test");
    singleton<Logger>::destroy();

    singleton<Logger>::create("TEST2");  // perfectly legal after destroy()
    singleton<Logger>::instance().log("second test");
    singleton<Logger>::destroy();
}
```

---

## Safety Properties Summary

| Property | Guarantee |
|---|---|
| No heap allocation | The managed object lives entirely in static storage — no `new`, no allocator |
| Deterministic construction | `create()` is called explicitly; no implicit first-use initialisation |
| Deterministic destruction | `destroy()` is called explicitly; no reliance on static destructor ordering |
| Double-init protection | `SINGLETON_ASSERT` fires if `create()` is called on an already-live instance |
| Use-before-init protection | `SINGLETON_ASSERT` fires if `instance()` is called before `create()` |
| Non-instantiable | All constructors are `delete`d — the class cannot be accidentally instantiated |
| Customisable fault policy | `SINGLETON_ASSERT` is overridable per-project without modifying the header |
| No virtual dispatch | Zero vtable overhead — suitable for deeply embedded targets |

---

## Key Concepts Summary

| Concept | What it does |
|---|---|
| `std::aligned_storage<Size, Align>` | Provides a correctly aligned raw buffer sized for the managed type |
| Placement new | Constructs the object at the buffer's address without any allocation |
| Explicit destructor call | Destroys the object without deallocating the buffer |
| `reinterpret_cast<T*>(&storage_)` | Interprets the raw buffer as a typed pointer after construction |
| `static` members | Both buffer and flag live in the data/BSS segment — zero runtime cost |
| `SINGLETON_ASSERT` hook | Separates fault policy from mechanism for portability across platforms |
| Deleted special members | Prevents accidental instantiation or copying of the manager class |

---

## Common Mistakes to Avoid

**Calling `instance()` before `create()`.** The storage contains uninitialised bytes until `create()` is called. Reading from it is undefined behaviour. Always ensure `create()` is called in system initialisation before any component calls `instance()`.

**Calling `create()` twice without an intervening `destroy()`.** The second call would construct a second object into the buffer, bypassing the first object's destructor. This leaks all resources held by the first instance. The assert guards against this in debug builds.

**Calling `destroy()` and then accessing the instance.** After `destroy()`, the stored object's lifetime has ended. The buffer memory still exists, but dereferencing it as `T` is undefined behaviour. Use `is_initialized()` guards where the liveness of the singleton is conditional.

**Assuming thread safety.** `InplaceSingleton` does not use any synchronisation primitives. If `create()`, `instance()`, or `destroy()` may be called from multiple threads or ISRs concurrently, you must add an external mutex or ensure calls happen in a single-threaded phase (e.g., before the scheduler starts).

**Storing a type with dynamic internal allocation.** The singleton itself uses no heap, but if `T` itself allocates memory on the heap in its constructor (e.g., `std::vector`, `std::string`), heap usage is reintroduced through `T`. For true zero-heap operation, ensure `T` also uses only static or stack storage.

---

## Study Checklist

- [ ] Explain why `std::aligned_storage` is necessary instead of a raw `char[]` buffer
- [ ] Show what happens (UB) if you call `instance()` before `create()` without the assert
- [ ] Implement a custom `SINGLETON_ASSERT` that logs to a ring buffer before halting
- [ ] Add a thread-safe wrapper around `InplaceSingleton` using `std::mutex`
- [ ] Compare the generated assembly of `InplaceSingleton` vs Meyers singleton for a simple type
- [ ] Demonstrate the order-of-destruction problem with Meyers singletons across translation units, and show how `InplaceSingleton` avoids it

---

## References

- [cppreference — Placement new](https://en.cppreference.com/w/cpp/language/new#Placement_new)
- [cppreference — `std::aligned_storage`](https://en.cppreference.com/w/cpp/types/aligned_storage)
- [MISRA C++ 2023 — Rule on dynamic memory](https://misra.org.uk)
- [AUTOSAR C++14 Guidelines — A18-5-1: No use of dynamic memory allocation after init](https://www.autosar.org)
- *Embedded C++ Patterns*, Miro Samek — Chapter on singleton and static objects

