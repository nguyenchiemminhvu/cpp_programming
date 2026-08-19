# InplaceFunction — Type Erasure Without Heap Allocation

## What Problem Are We Solving?

In C++, we often want to store a **callable** (a lambda, a functor, or a free function) inside a container or class, without knowing its exact type at compile time. The standard library gives us `std::function` for this purpose, and it works great — until it doesn't.

### The Hidden Cost of `std::function`

`std::function` uses a technique called **type erasure** internally. The problem is that its implementation almost always includes a **small buffer optimization (SBO)**: it has a small internal buffer (typically 16–32 bytes depending on the platform). If your callable fits in that buffer, no heap allocation happens. But if your callable is larger — for example, a lambda that captures many variables — `std::function` will **silently allocate memory on the heap**.

```cpp
#include <functional>
#include <string>

struct LargeCapture {
    std::string name;        // ~32 bytes
    double table[4];         // 32 bytes
    int id;                  // 4 bytes
    // Total: ~68 bytes — definitely exceeds std::function's internal buffer
};

int main() {
    LargeCapture ctx{"MyECU", {1.0, 2.0, 3.0, 4.0}, 42};

    // This LOOKS safe but silently does a heap allocation!
    std::function<void(int)> fn = [ctx](int x) {
        // uses ctx...
    };
}
```

The heap allocation itself is not always a problem — but in performance-critical, real-time, or embedded environments, **you cannot afford non-deterministic timing** from a memory allocator. You also cannot always guarantee that `std::bad_alloc` is handled properly (many embedded toolchains disable exceptions entirely).

### The Solution: Store the Callable Inline, Always

`InplaceFunction` is a replacement for `std::function` that stores the callable **directly inside itself**, in a fixed-size internal buffer. There is no heap allocation — ever. If the callable is too large to fit in the buffer, you get a **compile-time error**, not a runtime crash.

---

## Understanding Type Erasure First

Before diving into `InplaceFunction`, you need to understand **type erasure**, because `InplaceFunction` is built entirely on this concept.

### What is Type Erasure?

Type erasure is a technique where you **hide the concrete type** of an object behind a uniform interface. The classic example is `void*` in C:

```c
void call_it(void* callable, int arg);
```

This `void*` could point to anything. The caller doesn't know the type — but as long as there is a mechanism to invoke it correctly, we don't need to know.

In C++, we can do this more safely using **function pointers** combined with **placement new**.

### A Minimal Type Erasure Example

Here is the simplest possible type erasure — wrapping any callable into a fixed interface using function pointers:

```cpp
#include <cstddef>
#include <new>

// A fixed-size buffer to store any callable
std::byte buffer[64];

// A function pointer that knows how to call the thing stored in the buffer
void (*invoker)(void* buf, int arg) = nullptr;

// Store a lambda into the buffer
auto myLambda = [](int x) { return x * 2; };
new (buffer) decltype(myLambda)(myLambda);  // placement new — no heap!

// Create an invoker that knows the real type
invoker = [](void* buf, int arg) {
    auto* fn = static_cast<decltype(myLambda)*>(buf);
    (*fn)(arg);
};

// Later, call it without knowing the type:
invoker(buffer, 5);  // calls myLambda(5)
```

This is the **core idea** of `InplaceFunction`. Let's now build it properly, step by step.

---

## Step 1: The Storage Buffer

Every `InplaceFunction` needs a fixed-size buffer to hold the callable:

```cpp
alignas(std::max_align_t) std::byte storage_[StorageSize];
```

Two things to note:

**`std::byte`** — This is not a `char` array or an `int` array. `std::byte` is the correct type to use when you want raw memory for placement new, because it avoids aliasing issues.

**`alignas(std::max_align_t)`** — This is **not optional**. When you place an object into raw memory, that memory must be aligned correctly for that object's type. `std::max_align_t` is the strictest alignment required by any fundamental type on the platform (typically 8 or 16 bytes). Without proper alignment, accessing certain types (like `double` or SIMD types) in that buffer is **undefined behavior** — meaning your program may crash on some CPUs, or silently produce wrong results on others.

---

## Step 2: The Function Pointers (The Heart of Type Erasure)

Instead of virtual functions (which require heap-allocated vtables and polymorphic types), `InplaceFunction` uses three plain function pointers:

```cpp
ReturnType (*invoker_)(void*, Args&&...)  = nullptr;
void       (*destructor_)(void*)          = nullptr;
void       (*copier_)(void* dst, const void* src) = nullptr;
```

Each of these is set during construction to a small lambda (which becomes a stateless function pointer):

- **`invoker_`** — knows how to call the stored callable.
- **`destructor_`** — knows how to destroy the stored callable (important for RAII!).
- **`copier_`** — knows how to copy the callable from one buffer to another.

Why three pointers? Because after type erasure happens, the stored object is accessed only through `void*`. The system no longer knows its type. These three function pointers **preserve the type information** in an encoded, callable form — this is the essence of type erasure.

---

## Step 3: The Constructor — Where Type Erasure Happens

```cpp
template<typename F>
InplaceFunction(F&& callable) {
    using DecayedF = std::decay_t<F>;

    static_assert(sizeof(DecayedF) <= StorageSize,
        "Callable too large for InplaceFunction storage.");
    static_assert(alignof(DecayedF) <= alignof(std::max_align_t),
        "Callable alignment exceeds storage alignment.");

    new (storage_) DecayedF(std::forward<F>(callable));

    invoker_ = [](void* storage, Args&&... args) -> ReturnType {
        return (*static_cast<DecayedF*>(storage))(std::forward<Args>(args)...);
    };

    destructor_ = [](void* storage) {
        static_cast<DecayedF*>(storage)->~DecayedF();
    };

    copier_ = [](void* dst, const void* src) {
        new (dst) DecayedF(*static_cast<const DecayedF*>(src));
    };
}
```

Let's go line by line:

### `std::decay_t<F>`

`F` is a template parameter — it could be an lvalue reference (`LambdaType&`), an rvalue reference (`LambdaType&&`), or a const reference. `std::decay_t` strips away references and const/volatile qualifiers, giving you the plain underlying type. We need the decayed type because that is what we actually store.

### `static_assert` — Compile-Time Safety Net

```cpp
static_assert(sizeof(DecayedF) <= StorageSize, "...");
```

This is the key safety guarantee of `InplaceFunction`. If you try to store a callable that is larger than `StorageSize`, the compiler refuses to compile the code and shows a clear error message. You see the problem immediately, at compile time, not as a mysterious crash at 3am in production.

### `new (storage_) DecayedF(...)` — Placement New

This is the magic. `new (storage_) DecayedF(...)` constructs a `DecayedF` object **at the memory address `storage_`**. No memory is allocated — the storage already exists. The callable lives inside the `InplaceFunction` object itself.

### The Three Lambdas as Function Pointers

```cpp
invoker_ = [](void* storage, Args&&... args) -> ReturnType {
    return (*static_cast<DecayedF*>(storage))(std::forward<Args>(args)...);
};
```

This lambda has **no captures**, which means it implicitly converts to a function pointer. Inside the lambda, it casts the `void* storage` back to `DecayedF*` and calls it. The type `DecayedF` is captured at the time of construction as a template argument — this is how the type information survives erasure.

The same principle applies to `destructor_` and `copier_`.

---

## Step 4: Copy and Move Semantics

### Copy Constructor

```cpp
InplaceFunction(const InplaceFunction& other)
    : invoker_(other.invoker_)
    , destructor_(other.destructor_)
    , copier_(other.copier_)
{
    if (copier_) {
        copier_(storage_, other.storage_);
    }
}
```

Copying an `InplaceFunction` copies the three function pointers (which are just small values), and then calls `copier_` to copy-construct the stored callable into the new buffer.

### Move Constructor

```cpp
InplaceFunction(InplaceFunction&& other) noexcept
    : invoker_(other.invoker_)
    , destructor_(other.destructor_)
    , copier_(other.copier_)
{
    if (copier_) {
        copier_(storage_, other.storage_);
    }
    // other will clean up its own storage in its destructor
}
```

Here is an important point for new programmers: **moving an `InplaceFunction` is not truly a "move" in the usual sense**. Normally, moving an object steals its resources (e.g., moving a `std::vector` just transfers the internal pointer). But because the callable lives **inside** the `InplaceFunction`'s own buffer (on the stack, not the heap), there is no pointer to steal. Instead, we must **copy the bytes** from one buffer to another, then let the original clean itself up.

This is a conscious trade-off of the zero-allocation design: moves are slightly more expensive than for `std::function`, but you gain complete control over memory.

### Destructor

```cpp
~InplaceFunction() {
    if (destructor_) {
        destructor_(storage_);
    }
}
```

This calls the destructor of the stored callable. This is **critical** if your callable holds RAII resources — for example, a lambda that captures a `std::shared_ptr` or a lock guard. Without calling the destructor explicitly, those resources would leak. Because the callable lives in raw `std::byte` storage, the compiler does not know to call its destructor automatically — you have to do it manually through the `destructor_` pointer.

---

## Step 5: The Call Operator

```cpp
ReturnType operator()(Args... args) const {
    assert(invoker_ != nullptr && "Calling empty InplaceFunction!");
    return invoker_(
        const_cast<void*>(static_cast<const void*>(storage_)),
        std::forward<Args>(args)...
    );
}
```

When you call an `InplaceFunction`, it delegates to `invoker_`, passing the storage buffer and the arguments. The `invoker_` then re-interprets the buffer as the real callable type and calls it.

The `assert` guard catches misuse in debug builds — calling an empty (default-constructed) `InplaceFunction` is a programming error.

---

## The Complete Implementation

```cpp
// File: inplace_function.hpp
#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>
#include <new>
#include <cassert>

template<typename Signature, size_t StorageSize = 32>
class InplaceFunction;

template<typename ReturnType, typename... Args, size_t StorageSize>
class InplaceFunction<ReturnType(Args...), StorageSize> {
private:
    alignas(std::max_align_t) std::byte storage_[StorageSize];

    ReturnType (*invoker_)(void*, Args&&...) = nullptr;
    void       (*destructor_)(void*)         = nullptr;
    void       (*copier_)(void*, const void*) = nullptr;

public:
    // Default constructor: empty state
    InplaceFunction() = default;

    // Constructor from any callable
    template<
        typename F,
        typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, InplaceFunction>>
    >
    InplaceFunction(F&& callable) {
        using DecayedF = std::decay_t<F>;

        static_assert(
            sizeof(DecayedF) <= StorageSize,
            "Callable too large for InplaceFunction storage. "
            "Increase StorageSize or reduce lambda capture size."
        );
        static_assert(
            alignof(DecayedF) <= alignof(std::max_align_t),
            "Callable alignment exceeds storage alignment."
        );

        new (storage_) DecayedF(std::forward<F>(callable));

        invoker_ = [](void* storage, Args&&... args) -> ReturnType {
            return (*static_cast<DecayedF*>(storage))(std::forward<Args>(args)...);
        };

        destructor_ = [](void* storage) {
            static_cast<DecayedF*>(storage)->~DecayedF();
        };

        copier_ = [](void* dst, const void* src) {
            new (dst) DecayedF(*static_cast<const DecayedF*>(src));
        };
    }

    // Copy constructor
    InplaceFunction(const InplaceFunction& other)
        : invoker_(other.invoker_)
        , destructor_(other.destructor_)
        , copier_(other.copier_)
    {
        if (copier_) copier_(storage_, other.storage_);
    }

    // Move constructor (copy-then-invalidate, not true steal)
    InplaceFunction(InplaceFunction&& other) noexcept
        : invoker_(other.invoker_)
        , destructor_(other.destructor_)
        , copier_(other.copier_)
    {
        if (copier_) copier_(storage_, other.storage_);
    }

    // Destructor: must explicitly call callable's destructor
    ~InplaceFunction() {
        if (destructor_) destructor_(storage_);
    }

    // Call operator
    ReturnType operator()(Args... args) const {
        assert(invoker_ != nullptr && "Calling empty InplaceFunction!");
        return invoker_(
            const_cast<void*>(static_cast<const void*>(storage_)),
            std::forward<Args>(args)...
        );
    }

    // Boolean check: is this InplaceFunction holding a callable?
    explicit operator bool() const noexcept {
        return invoker_ != nullptr;
    }
};
```

---

## Usage Examples

### Example 1: Free Function

```cpp
#include "inplace_function.hpp"
#include <cstdio>

void greet(const char* name) {
    std::printf("Hello, %s!\n", name);
}

int main() {
    InplaceFunction<void(const char*), 16> fn = greet;
    fn("World");  // Prints: Hello, World!
}
```

### Example 2: Lambda with Small Capture

```cpp
int multiplier = 3;
InplaceFunction<int(int), 16> fn = [multiplier](int x) {
    return x * multiplier;
};

int result = fn(5);  // result = 15
```

The lambda captures one `int` (4 bytes), which is well within the 16-byte storage.

### Example 3: Stateful Functor

```cpp
struct Counter {
    int count = 0;
    void operator()(int increment) noexcept {
        count += increment;
    }
};

Counter c;
InplaceFunction<void(int), 16> fn = c;
fn(5);
fn(3);
// The Counter stored inside fn now has count = 8
```

### Example 4: Compile-Time Error on Oversized Callable

```cpp
struct TooBig {
    char data[100];  // 100 bytes
    void operator()(int) {}
};

// This line will NOT compile:
InplaceFunction<void(int), 32> fn = TooBig{};
// ERROR: "Callable too large for InplaceFunction storage."
```

You see the problem **immediately at compile time**, not as a runtime crash or silent memory corruption.

---

## Choosing the Right `StorageSize`

Since `StorageSize` is a template parameter, you must decide it at compile time. Here are some practical guidelines:

**Measure what you store.** Use `sizeof(YourLambda)` or `sizeof(YourFunctor)` to know the size of your callable. Add a small margin for safety.

```cpp
auto myCallback = [capturedVar1, capturedVar2](int x) { ... };
static_assert(sizeof(myCallback) <= 48, "callback too large");
```

**Use a sensible default.** A `StorageSize` of 32 or 48 bytes covers most practical cases (a few captured scalars or a small struct pointer).

**If your callable is too large, reduce its captures.** Instead of capturing a large struct by value, capture a pointer or reference to it (be careful about lifetime). Or break your callable into smaller pieces.

---

## Key Concepts Summary

| Concept | What it does |
|---|---|
| `alignas(std::max_align_t)` | Ensures the buffer is correctly aligned for any type placed in it |
| `std::byte storage_[N]` | Fixed-size raw memory buffer — no heap, lives inside the object |
| Placement new | Constructs an object at a specific memory address without allocating memory |
| `invoker_` function pointer | Encodes how to call the stored callable after type is erased |
| `destructor_` function pointer | Encodes how to destroy the stored callable properly |
| `copier_` function pointer | Encodes how to copy the stored callable to another buffer |
| `static_assert` | Compile-time guard — catches size violations before the program runs |
| `std::decay_t<F>` | Strips references and cv-qualifiers to get the plain stored type |

---

## Common Mistakes to Avoid

**Forgetting the destructor pointer.** If your callable holds a `std::shared_ptr`, a file handle, or a mutex lock, and you skip the `destructor_` mechanism, those resources will never be released. Always implement the destructor pointer.

**Using misaligned storage.** Skipping `alignas(std::max_align_t)` on the buffer is a common mistake. It may appear to work on x86 (which tolerates misaligned access in some cases) but will crash on ARM or other architectures where alignment is strictly enforced.

**Ignoring `static_assert` messages.** When you see the message `"Callable too large for InplaceFunction storage"`, the fix is either to increase `StorageSize` or to reduce the number of captured variables in your lambda.

**Calling an empty `InplaceFunction`.** A default-constructed `InplaceFunction` has `invoker_ == nullptr`. Calling it is undefined behavior (the `assert` in the call operator catches this in debug builds). Always check `if (fn)` before calling if the empty state is possible.
