#pragma once

#include "castle/callbacks/inplace_callback_registry.h"

#include <cstddef>

using castle::callbacks::inplace_callback_registry;

namespace castle
{
namespace events
{

// -----------------------------------------------------------------------------
// inplace_event_config
// -----------------------------------------------------------------------------
// Compile-time description of one event slot inside inplace_event_dispatcher.
//
// Template parameters:
//   EventTag                   - the TYPE used as a compile-time key to look
//                                up this slot. Carries no payload; only its
//                                identity matters. Can be an empty struct,
//                                enum type, or any regular type.
//   MaxCallback                - maximum number of concurrent subscribers for
//                                this event.
//   CallbackStorageSize        - inplace_function internal buffer size for
//                                each callback stored under this tag.
//   CallbackStorageAlignment   - inplace_function internal buffer alignment.
//   Args...                    - callback signature is void(Args...). This is
//                                the payload the producer passes to
//                                dispatch_event<Tag>(...). May be empty
//                                (event without payload).
//
// Note on parameter order:
//   Storage size/alignment come BEFORE Args... because Args... is a variadic
//   pack that must appear last. This is the price of allowing per-event
//   customisation of the inplace_function buffer while still using a
//   variadic argument list.
//
// Examples:
//   // Timer event: two loose args, default 64-byte callback buffer
//   inplace_event_config<TimerExpired, 8, 64, alignof(std::max_align_t),
//                        std::uint32_t, std::uint32_t>
//
//   // Shutdown: no args, small callback buffer
//   inplace_event_config<Shutdown, 2, 32, alignof(std::max_align_t)>
//
//   // Payload-carrying event: single struct-ref arg
//   inplace_event_config<KeyEvent, 8, 64, alignof(std::max_align_t),
//                        const KeyEvent&>
// -----------------------------------------------------------------------------
template <
    typename EventTag,
    std::size_t MaxCallback,
    std::size_t CallbackStorageSize = 64,
    std::size_t CallbackStorageAlignment = alignof(std::max_align_t),
    typename... Args>
struct inplace_event_config
{
    using event_tag = EventTag;

    static constexpr std::size_t max_callbacks = MaxCallback;
    static constexpr std::size_t callback_storage_size = CallbackStorageSize;
    static constexpr std::size_t callback_storage_alignment = CallbackStorageAlignment;

    using registry_type = inplace_callback_registry<MaxCallback, void(Args...), CallbackStorageSize, CallbackStorageAlignment>;

    // The registry is stored by value so the whole inplace_event_dispatcher
    // can hold a tuple<inplace_event_config...> with no heap and no
    // placement new.
    registry_type registry;

    inplace_event_config() = default;

    inplace_event_config(const inplace_event_config&) = delete;
    inplace_event_config& operator=(const inplace_event_config&) = delete;

    inplace_event_config(inplace_event_config&&) = delete;
    inplace_event_config& operator=(inplace_event_config&&) = delete;
};

} // namespace events
} // namespace castle
