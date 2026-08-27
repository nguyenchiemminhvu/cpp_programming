#pragma once

#include "callback_registry.h"

#include <cstddef>

namespace castle
{
namespace events
{

// -----------------------------------------------------------------------------
// event_config
// -----------------------------------------------------------------------------
// Compile-time description of one event slot inside event_dispatcher.
//
// Template parameters:
//   EventTag      - the TYPE used as a compile-time key to look up this slot.
//                   It carries no payload; it can be an empty struct, an enum
//                   type, or any regular type. Only its identity matters.
//   MaxCallback   - maximum number of concurrent subscribers for this event.
//   Args...       - the callback signature is void(Args...). This is the
//                   payload the producer will pass to dispatch_event<Tag>(...).
//                   May be empty (event without payload).
//
// Rationale:
//   Separating the tag (identity) from the arguments (payload) mirrors the
//   classic signal/slot model. It allows:
//     - events with zero arguments:
//         event_config<Shutdown, 4>                     -> void()
//     - events with multiple loose arguments (no wrapper struct required):
//         event_config<TimerExpired, 8, uint32_t, uint32_t>
//                                                       -> void(uint32_t, uint32_t)
//     - events with a single struct payload if preferred:
//         event_config<KeyEvent, 8, const KeyEvent&>    -> void(const KeyEvent&)
//
// event_dispatcher takes a variadic pack of event_config<...> and stores them
// in a std::tuple. Lookup by event tag is a compile-time tuple index.
// -----------------------------------------------------------------------------
template <typename EventTag, std::size_t MaxCallback, typename... Args>
struct event_config
{
    using event_tag = EventTag;

    static constexpr std::size_t capacity = MaxCallback;

    using registry_type = callback_registry<MaxCallback, void(Args...)>;

    // The registry is stored by value so event_dispatcher can hold a
    // tuple<event_config...> with no heap allocation and no placement new.
    registry_type registry;

    event_config() = default;

    event_config(const event_config&) = delete;
    event_config& operator=(const event_config&) = delete;

    event_config(event_config&&) = delete;
    event_config& operator=(event_config&&) = delete;
};

} // namespace events
} // namespace castle
