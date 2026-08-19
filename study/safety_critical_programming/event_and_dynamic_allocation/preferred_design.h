#pragma once
// =============================================================================
// PREFERRED DESIGN: Static Fixed-Size Invoker (No-Heap Observer)
//
// Goals:
//   - ZERO dynamic allocation after initialization.
//   - Deterministic O(1) registration, O(N) dispatch (N fixed at compile time).
//   - Callable size validated at COMPILE TIME via static_assert.
//   - Fully noexcept on the hot dispatch path.
//   - Correct RAII: the destructor of the stored callable is always invoked.
// =============================================================================

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <new> // placement new
#include <type_traits>
#include <utility>

// =============================================================================
// Part 1 - inplace_function<Signature, StorageSize>
//
// A zero-allocation replacement for std::function. The callable is stored
// directly inside the object (a fixed-size buffer), so no heap interaction ever
// occurs.
//
// Senior insight - three hidden function pointers:
//   invoker_    : dispatches operator() without virtual dispatch / a vtable.
//   destructor_ : calls ~F() so RAII resources are released correctly.
//   copier_     : enables copy construction / copy assignment.
//
// Senior insight - move semantics:
//   std::function move steals an internal heap pointer in O(1).
//   inplace_function move must copy the raw bytes and then destroy the source,
//   because the callable lives in stack storage - there is no pointer to steal.
//   This is the fundamental trade-off of a zero-allocation design.
// =============================================================================
template <typename Signature, std::size_t StorageSize = 32>
class inplace_function; // primary template - declaration only

template <typename ReturnType, typename... Args, std::size_t StorageSize>
class inplace_function<ReturnType(Args...), StorageSize>
{
public:
    // -- Default constructor: empty (invoker_ == nullptr) ---------------------
    inplace_function() = default;

    // -- Construct from any callable F ---------------------------------------
    template <
        typename F,
        typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, inplace_function>>>
    inplace_function(F&& callable)
    {
        using decayed_f = std::decay_t<F>;

        // Compile-time gate: the build fails immediately with a readable message
        // if the lambda is too large. There is no silent runtime heap fallback.
        static_assert(
            sizeof(decayed_f) <= StorageSize,
            "Callable too large for inplace_function storage. "
            "Increase StorageSize or reduce the lambda capture size.");
        static_assert(
            alignof(decayed_f) <= alignof(std::max_align_t),
            "Callable alignment exceeds storage alignment.");

        // Placement new: construct F in place. ZERO heap allocation.
        new (storage_) decayed_f(std::forward<F>(callable));

        // Stateless lambdas decay to plain function pointers (no closure object),
        // so the casts below are well-defined and cheap.
        invoker_ = [](void* storage, Args&&... args) -> ReturnType
        {
            return (*static_cast<decayed_f*>(storage))(std::forward<Args>(args)...);
        };
        destructor_ = [](void* storage)
        {
            static_cast<decayed_f*>(storage)->~decayed_f();
        };
        copier_ = [](void* dst, const void* src)
        {
            new (dst) decayed_f(*static_cast<const decayed_f*>(src));
        };
    }

    // -- Copy constructor -----------------------------------------------------
    inplace_function(const inplace_function& other)
        : invoker_(other.invoker_)
        , destructor_(other.destructor_)
        , copier_(other.copier_)
    {
        if (copier_)
        {
            copier_(storage_, other.storage_);
        }
    }

    // -- Move constructor -----------------------------------------------------
    // This is a copy-then-keep, not a true O(1) steal. See the header comment.
    inplace_function(inplace_function&& other) noexcept
        : invoker_(other.invoker_)
        , destructor_(other.destructor_)
        , copier_(other.copier_)
    {
        if (copier_)
        {
            copier_(storage_, other.storage_);
        }
        // other is left valid so its destructor can run without a double free.
    }

    // -- Copy assignment ------------------------------------------------------
    inplace_function& operator=(const inplace_function& other)
    {
        if (this != &other)
        {
            reset();
            invoker_    = other.invoker_;
            destructor_ = other.destructor_;
            copier_     = other.copier_;
            if (copier_)
            {
                copier_(storage_, other.storage_);
            }
        }
        return *this;
    }

    // -- Move assignment ------------------------------------------------------
    inplace_function& operator=(inplace_function&& other) noexcept
    {
        if (this != &other)
        {
            reset();
            invoker_    = other.invoker_;
            destructor_ = other.destructor_;
            copier_     = other.copier_;
            if (copier_)
            {
                copier_(storage_, other.storage_);
            }
        }
        return *this;
    }

    // -- Destructor: must call ~F() to release any RAII resource --------------
    ~inplace_function()
    {
        reset();
    }

    // -- Invoke ---------------------------------------------------------------
    ReturnType operator()(Args... args) const
    {
        assert(invoker_ != nullptr && "Calling an empty inplace_function!");
        return invoker_(
            const_cast<void*>(static_cast<const void*>(storage_)),
            std::forward<Args>(args)...);
    }

    // -- Empty check (mirrors std::function::operator bool) -------------------
    explicit operator bool() const noexcept
    {
        return invoker_ != nullptr;
    }

private:
    void reset() noexcept
    {
        if (destructor_)
        {
            destructor_(storage_);
        }
        invoker_    = nullptr;
        destructor_ = nullptr;
        copier_     = nullptr;
    }

    // alignas(std::max_align_t) ensures the storage is suitably aligned for any
    // fundamental type. Without it, placement-new of a strictly aligned type
    // (double, a SIMD vector, ...) is undefined behaviour on some CPUs.
    alignas(std::max_align_t) std::byte storage_[StorageSize];

    // Plain function pointers - no vtable, no virtual dispatch overhead.
    ReturnType (*invoker_)(void*, Args&&...)       = nullptr;
    void (*destructor_)(void*)                     = nullptr;
    void (*copier_)(void* dst, const void* src)    = nullptr;
};

// =============================================================================
// Part 2 - can_signal
//
// A richer struct than a bare (int, double) pair reflects real automotive
// practice: every signal carries a bus identity and a hardware timestamp so
// latency can be measured end-to-end.
// =============================================================================
struct can_signal
{
    std::uint32_t signal_id;
    double        value;
    std::uint8_t  bus_id;       // CAN0, CAN1, ...
    std::uint32_t timestamp_ms; // from the hardware timer (deadline monitoring)
};

// =============================================================================
// Part 3 - dispatcher_error (no exceptions)
//
// AUTOSAR and MISRA C++ both discourage or forbid exceptions in safety-critical
// paths. Automotive builds commonly use -fno-exceptions, so an enum return code
// is the idiomatic replacement.
// =============================================================================
enum class dispatcher_error : std::uint8_t
{
    ok = 0,
    listeners_full,   // MAX listeners already registered
    invalid_callback, // null / empty callback passed in
};

// =============================================================================
// Part 4 - non_allocating_can_dispatcher<MaxListeners, CallbackStorageSize>
//
// Key properties:
//   - All storage is stack / BSS: no heap after construction.
//   - std::array gives contiguous memory => cache-friendly sequential dispatch.
//   - A per-slot signal filter avoids invoking irrelevant callbacks without an
//     extra indirection layer.
//   - noexcept on every method: safe to call from RTOS tasks and ISRs.
//   - Thread-safety: NONE intentionally. In a real ECU each CAN channel usually
//     owns its own task; locking is the caller's responsibility.
// =============================================================================
template <std::size_t MaxListeners = 8, std::size_t CallbackStorageSize = 48>
class non_allocating_can_dispatcher
{
public:
    using callback = inplace_function<void(const can_signal&), CallbackStorageSize>;

    non_allocating_can_dispatcher() noexcept
    {
        signal_filters_.fill(k_accept_all); // default: no filter
    }

    // -- register_callback ----------------------------------------------------
    // signal_id_filter == k_accept_all means "receive every signal".
    dispatcher_error register_callback(
        callback&&    cb,
        std::uint32_t signal_id_filter = k_accept_all) noexcept
    {
        if (!cb)
        {
            return dispatcher_error::invalid_callback;
        }
        if (count_ >= MaxListeners)
        {
            return dispatcher_error::listeners_full;
        }

        signal_filters_[count_] = signal_id_filter;
        callbacks_[count_]      = std::move(cb);
        ++count_;
        return dispatcher_error::ok;
    }

    // -- dispatch -------------------------------------------------------------
    // Called from the CAN receive task; must be fast and allocation-free.
    void dispatch(const can_signal& signal) const noexcept
    {
        for (std::size_t i = 0; i < count_; ++i)
        {
            const std::uint32_t filter = signal_filters_[i];
            if (filter != k_accept_all && filter != signal.signal_id)
            {
                continue; // short-circuit: skip non-matching slots
            }
            callbacks_[i](signal);
        }
    }

    // -- Capacity helpers -----------------------------------------------------
    [[nodiscard]] std::size_t listener_count() const noexcept { return count_; }
    [[nodiscard]] bool        is_full() const noexcept { return count_ >= MaxListeners; }
    [[nodiscard]] bool        is_empty() const noexcept { return count_ == 0; }

private:
    static constexpr std::uint32_t k_accept_all = 0xFFFF'FFFFU;

    // Contiguous arrays: the CPU prefetcher can stream through all callbacks
    // without pointer-chasing, which matters at 1000+ signals/second.
    std::array<callback, MaxListeners>      callbacks_;
    std::array<std::uint32_t, MaxListeners> signal_filters_;
    std::size_t                             count_ = 0;
};
