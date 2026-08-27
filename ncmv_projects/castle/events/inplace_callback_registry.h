#pragma once

#include "inplace_function.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace castle
{
namespace events
{

// -----------------------------------------------------------------------------
// Error codes returned by inplace_callback_registry operations.
// -----------------------------------------------------------------------------
enum class inplace_callback_registry_error : std::uint8_t
{
    ok = 0,
    full,
    invalid_callback,
    invalid_subscription
};

// -----------------------------------------------------------------------------
// Type-erased unsubscribe interface.
// Every inplace_callback_registry instantiation implements this so that an
// inplace_callback_subscription can call back into its owning registry
// without knowing the concrete signature / storage / max_callback template
// parameters.
// -----------------------------------------------------------------------------
class i_inplace_unsubscribable
{
public:
    virtual ~i_inplace_unsubscribable() = default;

    // Remove the slot identified by (index, generation). Implementations must
    // treat mismatched generation / inactive slot as invalid_subscription.
    virtual inplace_callback_registry_error unsubscribe_slot(
        std::size_t index,
        std::uint32_t generation) noexcept = 0;
};

// -----------------------------------------------------------------------------
// Subscription handle returned by inplace_callback_registry::subscribe().
// Holds slot index + generation to prevent stale unsubscribe from removing a
// reused slot. Also holds a non-owning back-pointer to its owning registry so
// that callers can self-unsubscribe:
//
//   auto sub = registry.subscribe([](int v){ ... });
//   sub.unsubscribe();
//
// Lightweight value type — safe to copy/store. Copies share the same identity;
// the first successful unsubscribe wins, subsequent ones return
// invalid_subscription.
// -----------------------------------------------------------------------------
class inplace_callback_subscription
{
public:
    constexpr inplace_callback_subscription() noexcept = default;

    constexpr inplace_callback_subscription(
        i_inplace_unsubscribable* owner,
        std::size_t index,
        std::uint32_t generation) noexcept
        : owner_(owner),
          index_(index),
          generation_(generation),
          valid_(true)
    {
    }

    constexpr bool valid() const noexcept
    {
        return valid_ && owner_ != nullptr;
    }

    constexpr std::size_t index() const noexcept
    {
        return index_;
    }

    constexpr std::uint32_t generation() const noexcept
    {
        return generation_;
    }

    // Non-const because a successful unsubscribe invalidates *this in place.
    inplace_callback_registry_error unsubscribe() noexcept
    {
        if (!valid())
        {
            return inplace_callback_registry_error::invalid_subscription;
        }

        const inplace_callback_registry_error result =
            owner_->unsubscribe_slot(index_, generation_);

        // Regardless of success/failure, this handle no longer refers to a
        // live slot. Reset so subsequent calls are no-ops.
        reset();

        return result;
    }

    void reset() noexcept
    {
        owner_ = nullptr;
        index_ = 0;
        generation_ = 0;
        valid_ = false;
    }

private:
    i_inplace_unsubscribable* owner_ = nullptr;
    std::size_t index_ = 0;
    std::uint32_t generation_ = 0;
    bool valid_ = false;
};

// -----------------------------------------------------------------------------
// Fixed-capacity callback registry.
// Stores up to max_callback callbacks by value using inplace_function
// (no heap). Invocation order = registration order (first-registered fires
// first).
//
// Unlike the non-owning callback_registry, this registry OWNS each callback
// via inplace_function's internal buffer — so lambdas with captures and other
// stateful callables are fully supported without external lifetime management.
//
// Template parameters:
//   max_callback               - maximum number of concurrent subscriptions
//   signature                  - callback signature, e.g. void(int, float)
//   callback_storage_size      - inplace_function internal buffer size
//   callback_storage_alignment - inplace_function internal buffer alignment
//
// Usage:
//   inplace_callback_registry<4, void(int)> registry;
//   auto sub = registry.subscribe([](int v) { /* ... */ });
//   registry.invoke(42);
//   sub.unsubscribe();
// -----------------------------------------------------------------------------
template <
    std::size_t max_callback,
    typename signature,
    std::size_t callback_storage_size = 64,
    std::size_t callback_storage_alignment = alignof(std::max_align_t)>
class inplace_callback_registry;

template <
    std::size_t max_callback,
    typename return_type,
    typename... Args,
    std::size_t callback_storage_size,
    std::size_t callback_storage_alignment>
class inplace_callback_registry<
    max_callback,
    return_type(Args...),
    callback_storage_size,
    callback_storage_alignment> final
    : public i_inplace_unsubscribable
{
    static_assert(
        std::is_void<return_type>::value,
        "inplace_callback_registry requires void callback return type");

public:
    using callback_type =
        inplace_function<
            return_type(Args...),
            callback_storage_size,
            callback_storage_alignment>;

    using subscription = inplace_callback_subscription;
    using error = inplace_callback_registry_error;

    inplace_callback_registry() = default;
    ~inplace_callback_registry() override = default;

    // Non-copyable, non-movable. Registry identity is tied to slot storage
    // AND to the back-pointer embedded in outstanding subscriptions.
    inplace_callback_registry(const inplace_callback_registry&) = delete;
    inplace_callback_registry& operator=(const inplace_callback_registry&) = delete;

    inplace_callback_registry(inplace_callback_registry&&) = delete;
    inplace_callback_registry& operator=(inplace_callback_registry&&) = delete;

    // -------------------------------------------------------------------------
    // Subscribe a ready-made inplace_function callback.
    // Returns a subscription handle. On failure the returned handle is
    // !valid() and out_error (if provided) is set.
    // -------------------------------------------------------------------------
    subscription subscribe(
        callback_type&& callback,
        error* out_error = nullptr)
    {
        if (!callback)
        {
            if (out_error != nullptr)
            {
                *out_error = error::invalid_callback;
            }
            return subscription{};
        }

        for (std::size_t i = 0; i < max_callback; ++i)
        {
            slot& current_slot = slots_[i];

            if (!current_slot.active)
            {
                current_slot.callback = std::move(callback);
                current_slot.active = true;
                ++active_count_;

                if (out_error != nullptr)
                {
                    *out_error = error::ok;
                }

                return subscription{
                    this,
                    i,
                    current_slot.generation};
            }
        }

        if (out_error != nullptr)
        {
            *out_error = error::full;
        }
        return subscription{};
    }

    // -------------------------------------------------------------------------
    // Subscribe any callable that is convertible to inplace_function.
    // Wraps the callable into callback_type and delegates to the primary
    // subscribe(callback_type&&) above.
    // -------------------------------------------------------------------------
    template <
        typename callback_t,
        typename = std::enable_if_t<
            !std::is_same<
                std::decay_t<callback_t>,
                callback_type>::value &&
            !std::is_same<
                std::decay_t<callback_t>,
                subscription>::value>>
    subscription subscribe(
        callback_t&& callback,
        error* out_error = nullptr)
    {
        callback_type callback_wrapper{
            std::forward<callback_t>(callback)};

        return subscribe(
            std::move(callback_wrapper),
            out_error);
    }

    // -------------------------------------------------------------------------
    // Type-erased unsubscribe entry point used by inplace_callback_subscription.
    // Not intended for direct client use — prefer subscription::unsubscribe().
    // -------------------------------------------------------------------------
    error unsubscribe_slot(
        std::size_t index,
        std::uint32_t generation) noexcept override
    {
        if (index >= max_callback)
        {
            return error::invalid_subscription;
        }

        slot& current_slot = slots_[index];

        if (!current_slot.active)
        {
            return error::invalid_subscription;
        }

        if (current_slot.generation != generation)
        {
            return error::invalid_subscription;
        }

        current_slot.callback = callback_type{};
        current_slot.active = false;

        ++current_slot.generation;
        --active_count_;

        return error::ok;
    }

    // -------------------------------------------------------------------------
    // Invoke all active callbacks in registration order (slot 0 .. N-1).
    // -------------------------------------------------------------------------
    void invoke(Args... args)
    {
        for (std::size_t i = 0; i < max_callback; ++i)
        {
            slot& current_slot = slots_[i];

            if (current_slot.active && current_slot.callback)
            {
                current_slot.callback(
                    std::forward<Args>(args)...);
            }
        }
    }

    // -------------------------------------------------------------------------
    // Remove all active callbacks. Bumps generation on each cleared slot so
    // outstanding subscription handles become stale.
    // -------------------------------------------------------------------------
    void clear() noexcept
    {
        for (std::size_t i = 0; i < max_callback; ++i)
        {
            slot& current_slot = slots_[i];

            if (current_slot.active)
            {
                current_slot.callback = callback_type{};
                current_slot.active = false;
                ++current_slot.generation;
            }
        }

        active_count_ = 0;
    }

    constexpr std::size_t size() const noexcept
    {
        return active_count_;
    }

    constexpr bool empty() const noexcept
    {
        return active_count_ == 0;
    }

    static constexpr std::size_t capacity() noexcept
    {
        return max_callback;
    }

private:
    // Each slot holds one callback, a generation counter for identity, and an
    // active flag. Generation increments on each deactivation so that stale
    // subscription handles are detected.
    struct slot
    {
        callback_type callback;
        std::uint32_t generation = 0;
        bool active = false;
    };

    std::array<slot, max_callback> slots_{};
    std::size_t active_count_ = 0;
};

} // namespace events
} // namespace castle
