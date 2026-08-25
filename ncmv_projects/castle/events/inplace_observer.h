#pragma once

#include "inplace_function.h"
#include <cstddef>
#include <cstdint>
#include <array>
#include <utility>

namespace inplace_observer_detail
{
    enum class inplace_observer_error : uint8_t
    {
        ok = 0,
        listener_full,
        invalid_callback,
        invalid_subscription
    };

    class subscription
    {
    public:
        subscription() = default;

        subscription(std::size_t index, std::uint32_t generation) noexcept
            : index_(index), generation_(generation), valid_(true)
        {
        }

        bool valid() const noexcept
        {
            return valid_;
        }

        std::size_t index() const noexcept
        {
            return index_;
        }

        std::uint32_t generation() const noexcept
        {
            return generation_;
        }

    private:
        std::size_t index_ = 0;
        std::uint32_t generation_ = 0;
        bool valid_ = false;
    };
} // namespace inplace_observer_detail

// Primary template is only declared. The signature is decomposed via
// partial specialization below so notify() can forward the exact argument list.
template <std::size_t MaxListeners,
          typename Signature,
          std::size_t CallbackStorageSize = 64,
          std::size_t CallbackStorageAlignment = alignof(std::max_align_t)>
class inplace_observer;

template <std::size_t MaxListeners,
          typename R,
          typename... Args,
          std::size_t CallbackStorageSize,
          std::size_t CallbackStorageAlignment>
class inplace_observer<MaxListeners, R(Args...), CallbackStorageSize, CallbackStorageAlignment>
{
public:
    using callback_t = inplace_function<R(Args...), CallbackStorageSize, CallbackStorageAlignment>;

    inplace_observer() = default;
    ~inplace_observer() = default;

    inplace_observer_detail::inplace_observer_error subscribe(
        callback_t&& callback,
        inplace_observer_detail::subscription* out_handle = nullptr)
    {
        if (!callback)
        {
            return inplace_observer_detail::inplace_observer_error::invalid_callback;
        }

        for (std::size_t i = 0; i < MaxListeners; ++i)
        {
            if (!slots_[i].active)
            {
                slots_[i].callback = std::move(callback);
                slots_[i].active = true;
                ++active_count_;
                if (out_handle != nullptr)
                {
                    *out_handle = inplace_observer_detail::subscription{i, slots_[i].generation};
                }
                return inplace_observer_detail::inplace_observer_error::ok;
            }
        }

        return inplace_observer_detail::inplace_observer_error::listener_full;
    }

    inplace_observer_detail::inplace_observer_error unsubscribe(const inplace_observer_detail::subscription& handle)
    {
        if (!handle.valid() || handle.index() >= MaxListeners)
        {
            return inplace_observer_detail::inplace_observer_error::invalid_subscription;
        }

        slot& target = slots_[handle.index()];
        if (!target.active || target.generation != handle.generation())
        {
            return inplace_observer_detail::inplace_observer_error::invalid_subscription;
        }

        target.callback = callback_t{}; // call inplace_function default constructor to destroy the callable

        target.active = false;
        // Bump generation so the now-stale handle can never match a future
        // subscriber that reuses this slot.
        ++target.generation;
        --active_count_;
        return inplace_observer_detail::inplace_observer_error::ok;
    }

    // Return values are discarded by design: a fan-out to many listeners has no
    // single meaningful result to return.
    void notify(Args... args) const
    {
        for (std::size_t i = 0; i < MaxListeners; ++i)
        {
            if (slots_[i].active && slots_[i].callback)
            {
                slots_[i].callback(args...);
            }
        }
    }

    std::size_t size() const noexcept
    {
        return active_count_;
    }

    void clear() noexcept
    {
        for (std::size_t i = 0; i < MaxListeners; ++i)
        {
            if (slots_[i].active)
            {
                slots_[i].callback = callback_t{};
                slots_[i].active = false;
                ++slots_[i].generation;
            }
        }
        active_count_ = 0;
    }

    static constexpr std::size_t capacity() noexcept
    {
        return MaxListeners;
    }

private:
    struct slot
    {
        callback_t callback;
        std::uint32_t generation = 0;
        bool active = false;
    };

    std::array<slot, MaxListeners> slots_;
    std::size_t active_count_ = 0;
};

