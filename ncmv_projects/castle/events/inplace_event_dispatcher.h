#pragma once

#include "../callbacks/inplace_function.h"
#include "../callbacks/inplace_callback_registry.h"
#include "inplace_event_config.h"

#include <bitset>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

using castle::callbacks::inplace_function;
using castle::callbacks::inplace_callback_registry;
using castle::callbacks::inplace_callback_registry_error;
using castle::callbacks::inplace_callback_subscription;

namespace castle
{
namespace events
{

// -----------------------------------------------------------------------------
// Error codes returned by inplace_event_dispatcher operations.
// -----------------------------------------------------------------------------
enum class inplace_event_dispatcher_error : std::uint8_t
{
    ok = 0,
    full,
    invalid_callback,
    invalid_subscription,
    event_disabled
};

// -----------------------------------------------------------------------------
// inplace_event_dispatcher - compile-time, tag-keyed event dispatcher with
// value-owned callbacks.
//
// Model:
//   - The event set is fixed at compile time via a pack of
//     inplace_event_config<...>.
//   - Each event is identified by a TAG TYPE (event_tag). The tag acts as a
//     compile-time key — analogous to an integer key in a hash table, but
//     resolved at compile time to a tuple index (O(1), no hashing, no
//     virtual dispatch, no placement new for the event slots themselves).
//   - Each event carries its own callback signature void(Args...) AND its
//     own callback storage size/alignment, declared inside
//     inplace_event_config.
//
// Storage:
//   - A std::tuple<EventConfigs...> holds each inplace_event_config by value;
//     each config embeds its own inplace_callback_registry.
//   - A std::bitset<sizeof...(EventConfigs)> tracks per-event enable state.
//
// Subscriptions:
//   - register_callback<Tag>(callable) returns an
//     inplace_callback_subscription carrying a back-pointer to the correct
//     registry, so callers can self-unsubscribe:
//         sub.unsubscribe();
//
// Usage:
//   struct TimerExpired  {};   // pure tag, no payload
//   struct VehicleStarted {};  // pure tag, no payload
//   struct Shutdown       {};  // pure tag, no payload
//
//   using dispatcher_t = inplace_event_dispatcher<
//       inplace_event_config<TimerExpired,   8, 64, alignof(std::max_align_t),
//                            std::uint32_t, std::uint32_t>,
//       inplace_event_config<VehicleStarted, 4, 64, alignof(std::max_align_t),
//                            std::uint8_t>,
//       inplace_event_config<Shutdown,       2, 32, alignof(std::max_align_t)>
//   >;
//
//   dispatcher_t dispatcher;
//   auto sub = dispatcher.register_callback<TimerExpired>(
//       [](std::uint32_t id, std::uint32_t ts) { /* ... */ });
//   dispatcher.dispatch_event<TimerExpired>(42u, 1000u);
//   sub.unsubscribe();
// -----------------------------------------------------------------------------
template <typename... EventConfigs>
class inplace_event_dispatcher
{
    static_assert(sizeof...(EventConfigs) > 0,
                  "inplace_event_dispatcher requires at least one inplace_event_config");

    // -------------------------------------------------------------------------
    // Compile-time uniqueness check for the EventConfigs... pack.
    //
    // Duplicates would silently break the design:
    //   - index_of<Tag>() always returns the FIRST match, so enable / disable /
    //     clear operations on the duplicate slot would be unreachable from the
    //     dispatch_event() side,
    //   - subscriber_count() and callback_capacity() would report values that
    //     no longer reflect the real per-event budget.
    // -------------------------------------------------------------------------
    template <typename... Configs>
    struct configs_are_unique;

    template <>
    struct configs_are_unique<> : std::true_type {};

    template <typename Head, typename... Tail>
    struct configs_are_unique<Head, Tail...>
    {
        static constexpr bool value = ((Head::event_tag != Tail::event_tag) && ...)
                                     && configs_are_unique<Tail...>::value;
    };

    static_assert(configs_are_unique<EventConfigs...>::value,
                  "event_dispatcher: the EventConfigs... pack must not contain duplicate event_tag types");

public:
    using error = inplace_event_dispatcher_error;
    using subscription = inplace_callback_subscription;

    inplace_event_dispatcher()
    {
        // All events start enabled. bitset default-initialises to 0.
        enabled_.set();
    }

    ~inplace_event_dispatcher() = default;

    // Non-copyable, non-movable. Dispatcher identity is tied to the addresses
    // of the registries embedded in its tuple — outstanding subscriptions
    // reference those addresses via i_inplace_unsubscribable*.
    inplace_event_dispatcher(const inplace_event_dispatcher&) = delete;
    inplace_event_dispatcher& operator=(const inplace_event_dispatcher&) = delete;

    inplace_event_dispatcher(inplace_event_dispatcher&&) = delete;
    inplace_event_dispatcher& operator=(inplace_event_dispatcher&&) = delete;

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
        return config_for<Tag>::max_callbacks;
    }

    template <typename Tag>
    static constexpr std::size_t callback_storage_capacity() noexcept
    {
        return config_for<Tag>::callback_storage_size;
    }

    template <typename Tag>
    static constexpr std::size_t callback_storage_alignment_value() noexcept
    {
        return config_for<Tag>::callback_storage_alignment;
    }

    // -------------------------------------------------------------------------
    // Register a callback for the event identified by Tag.
    //
    // Accepts any callable convertible to the inplace_function signature
    // declared by inplace_event_config<Tag, ..., Args...>. The callback is
    // stored by value inside the registry's inplace_function buffer, so
    // stateful lambdas / captures are supported (subject to the per-tag
    // callback_storage_size).
    //
    // Returns a subscription handle. On failure the handle is !valid() and
    // out_error (if provided) is set.
    // -------------------------------------------------------------------------
    template <typename Tag, typename Callback>
    subscription register_callback(
        Callback&& callback,
        error* out_error = nullptr)
    {
        inplace_callback_registry_error inner_error =
            inplace_callback_registry_error::ok;

        subscription sub =
            registry<Tag>().subscribe(
                std::forward<Callback>(callback),
                &inner_error);

        if (out_error != nullptr)
        {
            *out_error = convert_error(inner_error);
        }

        return sub;
    }

    // -------------------------------------------------------------------------
    // Dispatch an event identified by Tag with the payload declared by
    // Args... in inplace_event_config<Tag, ..., Args...>.
    //
    // Perfect-forwarded to inplace_callback_registry::invoke(Args...).
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
    // Compile-time lookup: find the inplace_event_config whose event_tag ==
    // Tag.
    // -------------------------------------------------------------------------
    template <typename Tag, std::size_t I, typename... Rest>
    struct index_of_impl;

    template <typename Tag, std::size_t I, typename First, typename... Rest>
    struct index_of_impl<Tag, I, First, Rest...>
    {
        static constexpr std::size_t value =
            std::is_same<typename First::event_tag, Tag>::value
                ? I
                : index_of_impl<Tag, I + 1, Rest...>::value;
    };

    template <typename Tag, std::size_t I>
    struct index_of_impl<Tag, I>
    {
        static_assert(sizeof(Tag) > 0, "inplace_event_dispatcher: Tag is not present in the EventConfigs pack");
        static constexpr std::size_t value = I;
    };

    template <typename Tag>
    static constexpr std::size_t index_of() noexcept
    {
        return index_of_impl<Tag, 0, EventConfigs...>::value;
    }

    // inplace_event_config bound to a specific tag.
    template <typename Tag>
    using config_for =
        std::tuple_element_t<index_of<Tag>(), std::tuple<EventConfigs...>>;

    // Direct access to the inplace_callback_registry for a specific tag.
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

    // Map inplace_callback_registry_error to inplace_event_dispatcher_error.
    static constexpr error convert_error(
        inplace_callback_registry_error error_code) noexcept
    {
        switch (error_code)
        {
        case inplace_callback_registry_error::ok:
            return error::ok;

        case inplace_callback_registry_error::full:
            return error::full;

        case inplace_callback_registry_error::invalid_callback:
            return error::invalid_callback;

        case inplace_callback_registry_error::invalid_subscription:
            return error::invalid_subscription;
        }

        return error::invalid_subscription;
    }

private:
    // Tuple of inplace_event_config instances — each embeds its own
    // inplace_callback_registry sized independently. This is the whole
    // storage for the dispatcher.
    std::tuple<EventConfigs...> configs_{};

    // Per-event enable/disable flags, indexed by index_of<Tag>().
    std::bitset<sizeof...(EventConfigs)> enabled_{};
};

} // namespace events
} // namespace castle
