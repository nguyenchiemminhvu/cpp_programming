#pragma once

#include "callback_registry.h"
#include "event_config.h"
#include "function.h"

#include <bitset>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

namespace castle
{
namespace events
{

// -----------------------------------------------------------------------------
// Error codes returned by event_dispatcher operations.
// -----------------------------------------------------------------------------
enum class event_dispatcher_error : std::uint8_t
{
    ok = 0,
    full,
    invalid_callback,
    invalid_subscription,
    event_disabled
};

// -----------------------------------------------------------------------------
// event_dispatcher - compile-time, tag-keyed event dispatcher.
//
// Model:
//   - The event set is fixed at compile time via a pack of event_config<...>.
//   - Each event is identified by a TAG TYPE (event_config::event_tag). The
//     tag acts as a compile-time key — analogous to an integer key in a hash
//     table, but resolved at compile time to a tuple index (O(1), no hashing,
//     no virtual dispatch, no placement new).
//   - Each event carries its own callback signature void(Args...), declared
//     inside event_config. Payload and key are decoupled.
//
// Storage:
//   - A std::tuple<EventConfigs...> holds each event_config by value; each
//     event_config embeds its own callback_registry sized independently.
//   - A std::bitset<sizeof...(EventConfigs)> tracks per-event enable state.
//
// Subscriptions:
//   - register_callback<Tag>(&cb) returns a callback_subscription that
//     carries a back-pointer to the correct registry, so callers can
//     self-unsubscribe: sub.unsubscribe();
//
// Usage:
//   struct TimerExpired  {};   // pure tag, no payload
//   struct VehicleStarted {};  // pure tag, no payload
//
//   using dispatcher_t = event_dispatcher<
//       event_config<TimerExpired,   8, std::uint32_t, std::uint32_t>,
//       event_config<VehicleStarted, 4, std::uint8_t>
//   >;
//
//   dispatcher_t dispatcher;
//
//   void on_timer(std::uint32_t id, std::uint32_t ts) { ... }
//   castle::events::function<std::uint32_t, std::uint32_t> cb(&on_timer);
//
//   auto sub = dispatcher.register_callback<TimerExpired>(&cb);
//   dispatcher.dispatch_event<TimerExpired>(42u, 1000u);
//   sub.unsubscribe();
// -----------------------------------------------------------------------------
template <typename... EventConfigs>
class event_dispatcher
{
    static_assert(sizeof...(EventConfigs) > 0, "event_dispatcher requires at least one event_config");

public:
    using error = event_dispatcher_error;
    using subscription = callback_registry_subscription;

    event_dispatcher()
    {
        // All events start enabled. bitset default-initialises to 0.
        enabled_.set();
    }

    ~event_dispatcher() = default;

    // Non-copyable, non-movable. Dispatcher identity is tied to the addresses
    // of the registries embedded in its tuple — outstanding subscriptions
    // reference those addresses via i_unsubscribable*.
    event_dispatcher(const event_dispatcher&) = delete;
    event_dispatcher& operator=(const event_dispatcher&) = delete;

    event_dispatcher(event_dispatcher&&) = delete;
    event_dispatcher& operator=(event_dispatcher&&) = delete;

    // -------------------------------------------------------------------------
    // Compile-time capacity queries.
    // -------------------------------------------------------------------------
    static constexpr std::size_t event_capacity() noexcept
    {
        return sizeof...(EventConfigs);
    }

    template <typename Tag>
    static constexpr std::size_t callback_capacity() noexcept
    {
        return config_for<Tag>::capacity;
    }

    // -------------------------------------------------------------------------
    // Register a non-owning callback for the event identified by Tag.
    //
    // The callback signature MUST match the Args... declared in
    // event_config<Tag, N, Args...>. This is enforced by the type of the
    // callback pointer: registry_for<Tag>::callback_type ==
    // i_function<Args...>.
    //
    // The caller owns the lifetime of the i_function object and must keep it
    // alive until the returned subscription is unsubscribed (or the
    // dispatcher is destroyed).
    // -------------------------------------------------------------------------
    template <typename Tag, typename CallbackPtr>
    subscription register_callback(CallbackPtr callback, error* out_error = nullptr) noexcept
    {
        // Delegate signature checking to the registry: CallbackPtr must be
        // convertible to the exact i_function<Args...>* expected by Tag.
        callback_registry_error inner_error = callback_registry_error::ok;

        subscription sub = registry<Tag>().subscribe(callback, &inner_error);

        if (out_error != nullptr)
        {
            *out_error = convert_error(inner_error);
        }

        return sub;
    }

    // -------------------------------------------------------------------------
    // Dispatch an event identified by Tag with the payload declared by
    // Args... in event_config<Tag, N, Args...>.
    //
    // Perfect-forwarded to callback_registry::invoke(Args...).
    // Returns error::event_disabled if the event tag is currently disabled.
    // (An unknown Tag fails to compile — no runtime "not found".)
    // -------------------------------------------------------------------------
    template <typename Tag, typename... CallArgs>
    error dispatch_event(CallArgs&&... args)
    {
        constexpr std::size_t idx = index_of<Tag>();

        if (!enabled_.test(idx))
        {
            return error::event_disabled;
        }

        registry<Tag>().invoke(std::forward<CallArgs>(args)...);

        return error::ok;
    }

    // -------------------------------------------------------------------------
    // Enable / disable / query an event tag. Disabled events accept
    // subscribes but dispatch_event() returns event_disabled without
    // invoking any callbacks.
    // -------------------------------------------------------------------------
    template <typename Tag>
    void enable_event() noexcept
    {
        enabled_.set(index_of<Tag>());
    }

    template <typename Tag>
    void disable_event() noexcept
    {
        enabled_.reset(index_of<Tag>());
    }

    template <typename Tag>
    bool is_event_enabled() const noexcept
    {
        return enabled_.test(index_of<Tag>());
    }

    // -------------------------------------------------------------------------
    // Clear all subscriptions for a specific event tag. Outstanding
    // subscription handles for that tag become stale.
    // -------------------------------------------------------------------------
    template <typename Tag>
    void clear_event() noexcept
    {
        registry<Tag>().clear();
    }

    // -------------------------------------------------------------------------
    // Clear all subscriptions across every event tag.
    // -------------------------------------------------------------------------
    void clear() noexcept
    {
        clear_all_impl(std::index_sequence_for<EventConfigs...>{});
    }

    template <typename Tag>
    std::size_t subscriber_count() const noexcept
    {
        return registry<Tag>().size();
    }

private:
    // -------------------------------------------------------------------------
    // Compile-time lookup: find the event_config whose event_tag == Tag.
    // -------------------------------------------------------------------------
    template <typename Tag, std::size_t I, typename... Rest>
    struct index_of_impl;

    template <typename Tag, std::size_t I, typename First, typename... Rest>
    struct index_of_impl<Tag, I, First, Rest...>
    {
        static constexpr std::size_t value = std::is_same<typename First::event_tag, Tag>::value
                                             ? I
                                             : index_of_impl<Tag, I + 1, Rest...>::value;
    };

    template <typename Tag, std::size_t I>
    struct index_of_impl<Tag, I>
    {
        static_assert(sizeof(Tag) > 0, "event_dispatcher: Tag is not present in the EventConfigs pack");
        static constexpr std::size_t value = I;
    };

    template <typename Tag>
    static constexpr std::size_t index_of() noexcept
    {
        return index_of_impl<Tag, 0, EventConfigs...>::value;
    }

    // event_config bound to a specific tag.
    template <typename Tag>
    using config_for = std::tuple_element_t<index_of<Tag>(), std::tuple<EventConfigs...>>;

    // Direct access to the callback_registry for a specific tag.
    template <typename Tag>
    typename config_for<Tag>::registry_type& registry() noexcept
    {
        return std::get<index_of<Tag>()>(configs_).registry;
    }

    template <typename Tag>
    const typename config_for<Tag>::registry_type& registry() const noexcept
    {
        return std::get<index_of<Tag>()>(configs_).registry;
    }

    // Sequence a clear() call across every tuple element without fold
    // expressions (C++14 friendly).
    template <std::size_t... Is>
    void clear_all_impl(std::index_sequence<Is...>) noexcept
    {
        int dummy[] = {
            (std::get<Is>(configs_).registry.clear(), 0)...
        };
        (void)dummy;
    }

    // Map callback_registry_error to event_dispatcher_error.
    static constexpr error convert_error(callback_registry_error error_code) noexcept
    {
        switch (error_code)
        {
        case callback_registry_error::ok:
            return error::ok;

        case callback_registry_error::full:
            return error::full;

        case callback_registry_error::invalid_callback:
            return error::invalid_callback;

        case callback_registry_error::invalid_subscription:
            return error::invalid_subscription;
        }

        return error::invalid_subscription;
    }

private:
    // Tuple of event_config instances — each embeds its own callback_registry
    // sized independently. This is the whole storage for the dispatcher.
    std::tuple<EventConfigs...> configs_{};

    // Per-event enable/disable flags, indexed by index_of<Tag>().
    std::bitset<sizeof...(EventConfigs)> enabled_{};
};

} // namespace events
} // namespace castle
