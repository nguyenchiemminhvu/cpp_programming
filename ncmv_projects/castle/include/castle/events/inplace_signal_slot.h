#pragma once

#include "castle/callbacks/inplace_callback_registry.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace castle
{
namespace events
{

// =============================================================================
// inplace_signal_error
// =============================================================================
//
// Error domain exposed by inplace_signal.
//
// The signal API deliberately does not expose
// inplace_callback_registry_error directly.
//
// This keeps the signal abstraction independent from the implementation
// details of inplace_callback_registry.
//
// =============================================================================

enum class inplace_signal_error : std::uint8_t
{
    ok = 0,
    full,
    invalid_callback,
    invalid_connection
};


// =============================================================================
// basic_inplace_connection
// =============================================================================
//
// Lightweight, non-owning connection handle.
//
// IMPORTANT:
//
// This object does NOT own the signal.
// This object does NOT own the callback.
//
// The callback is owned by inplace_callback_registry.
//
// The connection only contains:
//
//     registry pointer
//     slot index
//     generation
//
// Therefore:
//
//     connection lifetime MUST NOT exceed signal lifetime.
//
// Example:
//
//     inplace_signal<4, void(int)> signal;
//
//     auto connection = signal.connect(
//         [](int value)
//         {
//             // ...
//         });
//
//     signal.emit(42);
//
//     connection.disconnect();
//
//
//
// Why don't we use std::shared_ptr / weak_ptr here?
//
// Because this library targets embedded systems where:
//
//     - heap allocation is undesirable
//     - reference counting is unnecessary overhead
//     - ownership should remain explicit
//     - object lifetime should be deterministic
//
// Generation prevents a stale connection from disconnecting a newly
// registered callback that happens to reuse the same slot.
//
// Example:
//
//     connection A -> slot #1, generation 5
//
//     A.disconnect()
//
//     new callback -> slot #1, generation 6
//
//     stale A cannot disconnect generation 6.
//
// =============================================================================

template <typename Registry>
class basic_inplace_connection
{
public:
    constexpr basic_inplace_connection() noexcept = default;

    constexpr bool valid() const noexcept
    {
        return owner_ != nullptr && valid_;
    }

    // -------------------------------------------------------------------------
    // disconnect
    // -------------------------------------------------------------------------
    //
    // Disconnect the callback associated with this connection.
    //
    // After this function returns, this connection becomes invalid regardless
    // of whether the registry accepted the request.
    //
    // This makes the handle single-use from the caller's perspective.
    //
    // -------------------------------------------------------------------------

    inplace_signal_error disconnect() noexcept
    {
        if (!valid())
        {
            return inplace_signal_error::invalid_connection;
        }

        const auto result = owner_->unsubscribe_slot(index_, generation_);

        // The handle is no longer usable after disconnect().
        reset();

        return translate_error(result);
    }

    // -------------------------------------------------------------------------
    // reset
    // -------------------------------------------------------------------------
    //
    // Forget this connection without modifying the registry.
    //
    // This function should normally NOT be used to disconnect a callback.
    //
    // Use disconnect() when the intention is to remove the callback.
    //
    // reset() is useful when transferring/abandoning a connection handle.
    //
    // -------------------------------------------------------------------------

    void reset() noexcept
    {
        owner_ = nullptr;
        index_ = 0U;
        generation_ = 0U;
        valid_ = false;
    }

    constexpr std::size_t index() const noexcept
    {
        return index_;
    }

    constexpr std::uint32_t generation() const noexcept
    {
        return generation_;
    }

private:
    using registry_error = castle::callbacks::inplace_callback_registry_error;
    using registry_base = castle::callbacks::i_inplace_unsubscribable;

    friend Registry;

    constexpr basic_inplace_connection(registry_base* owner, std::size_t index, std::uint32_t generation) noexcept
        : owner_(owner)
        , index_(index)
        , generation_(generation)
        , valid_(true)
    {
    }

    static constexpr inplace_signal_error translate_error(
        registry_error error) noexcept
    {
        switch (error)
        {
            case registry_error::ok:
            {
                return inplace_signal_error::ok;
            }
            case registry_error::full:
            {
                return inplace_signal_error::full;
            }
            case registry_error::invalid_callback:
            {
                return inplace_signal_error::invalid_callback;
            }
            case registry_error::invalid_subscription:
            {
                return inplace_signal_error::invalid_connection;
            }
        }

        return inplace_signal_error::invalid_connection;
    }

private:
    // Non-owning pointer to the registry inside signal.
    registry_base* owner_ = nullptr;

    // Registry slot identity.
    std::size_t index_ = 0U;

    // Slot generation protects against stale connections.
    std::uint32_t generation_ = 0U;

    bool valid_ = false;
};


// =============================================================================
// inplace_signal
// =============================================================================
//
// Qt-inspired signal/slot abstraction for embedded systems.
//
// The important difference from signal_slot.h is:
//
//     signal_slot.h
//         signal
//           └── callback_registry
//                 └── pointer to externally owned callback
//
//     inplace_signal_slot.h
//         inplace_signal
//           └── inplace_callback_registry
//                 └── inplace_function
//                       └── owns callable
//
// Therefore:
//
//     inplace_signal owns the lifetime of every connected callable.
//
//
//
// Example:
//
//     castle::events::inplace_signal<4, void(int)> signal;
//
//     auto connection = signal.connect(
//         [](int value)
//         {
//             // callback owns its captured state
//         });
//
//     signal.emit(42);
//
//     connection.disconnect();
//
//
//
// No heap allocation is required.
//
// The callback is stored directly inside the inplace_callback_registry.
//
// =============================================================================

template <
    std::size_t MaxConnection,
    typename Signature,
    std::size_t CallbackStorageSize = 64,
    std::size_t CallbackStorageAlignment = alignof(std::max_align_t)>
class inplace_signal;


// =============================================================================
// inplace_signal<MaxConnection, void(Args...)>
// =============================================================================
//
// Signature specialization.
//
// Callback storage parameters are propagated directly to
// inplace_callback_registry.
//
// Example:
//
//     inplace_signal<8, void(uint32_t), 32>
//
// means:
//
//     maximum callbacks = 8
//     callback signature = void(uint32_t)
//     each callback has 32 bytes of inline storage
//
// =============================================================================

template <
    std::size_t MaxConnection,
    typename... Args,
    std::size_t CallbackStorageSize,
    std::size_t CallbackStorageAlignment>
class inplace_signal<
    MaxConnection,
    void(Args...),
    CallbackStorageSize,
    CallbackStorageAlignment> final
{
private:
    using registry_type =
        castle::callbacks::inplace_callback_registry<
            MaxConnection,
            void(Args...),
            CallbackStorageSize,
            CallbackStorageAlignment>;

public:
    using callback_type = typename registry_type::callback_type;
    using connection = basic_inplace_connection<registry_type>;

    using error = inplace_signal_error;

    // -------------------------------------------------------------------------
    // Construction
    // -------------------------------------------------------------------------

    inplace_signal() = default;
    ~inplace_signal() = default;

    // The registry identity is embedded inside the signal.
    //
    // Existing connections contain a pointer to this registry.
    //
    // Therefore moving the signal would invalidate those pointers.
    //
    // A signal also represents a unique event source, so copying it would
    // create ambiguous ownership/connection semantics.
    //
    inplace_signal(const inplace_signal&) = delete;
    inplace_signal& operator=(const inplace_signal&) = delete;
    inplace_signal(inplace_signal&&) = delete;
    inplace_signal& operator=(inplace_signal&&) = delete;


    // =========================================================================
    // connect(callback_type&&)
    // =========================================================================
    //
    // Register an already constructed inplace_function.
    //
    // The registry takes ownership of the callable stored inside callback.
    //
    // Example:
    //
    //     inplace_signal<4, void(int)> signal;
    //
    //     inplace_signal<4, void(int)>::callback_type callback =
    //         [](int value)
    //         {
    //             // ...
    //         };
    //
    //     auto connection =
    //         signal.connect(std::move(callback));
    //
    // After a successful connection, the callback lifetime is controlled by
    // inplace_callback_registry.
    //
    // =========================================================================

    connection connect(callback_type&& callback, error* out_error = nullptr)
    {
        using registry_error = castle::callbacks::inplace_callback_registry_error;
        registry_error registry_result = registry_error::ok;

        const auto registry_connection = registry_.subscribe(std::move(callback), &registry_result);

        if (out_error != nullptr)
        {
            *out_error = translate_error(registry_result);
        }

        if (!registry_connection.valid())
        {
            return connection{};
        }

        return connection{
            &registry_,
            registry_connection.index(),
            registry_connection.generation()
        };
    }


    // =========================================================================
    // connect(Callable&&)
    // =========================================================================
    //
    // Primary user-facing API.
    //
    // Any callable that can be stored by inplace_function can be connected
    // directly.
    //
    // Examples:
    //
    //     signal.connect([](int value)
    //     {
    //         // ...
    //     });
    //
    //
    // Stateful lambda:
    //
    //     int counter = 0;
    //
    //     signal.connect([&counter](int value)
    //     {
    //         counter += value;
    //     });
    //
    //
    // Functor:
    //
    //     struct Handler
    //     {
    //         void operator()(int value)
    //         {
    //             // ...
    //         }
    //     };
    //
    //     signal.connect(Handler{});
    //
    //
    // IMPORTANT:
    //
    // The callable is COPIED or MOVED into the signal's registry.
    //
    // Therefore the caller does NOT need to keep the callable alive after
    // connect() returns.
    //
    // =========================================================================

    template <typename Callable,
              typename = std::enable_if_t<!std::is_same<std::decay_t<Callable>, callback_type>::value>>
    connection connect(Callable&& callable, error* out_error = nullptr)
    {
        callback_type callback{std::forward<Callable>(callable)};
        return connect(std::move(callback), out_error);
    }


    // =========================================================================
    // emit
    // =========================================================================
    //
    // Invoke all currently active callbacks.
    //
    // Invocation order is registration/slot order as defined by
    // inplace_callback_registry.
    //
    // Example:
    //
    //     signal.emit(42);
    //
    // Because inplace_callback_registry owns the callbacks, the callable
    // objects remain alive until:
    //
    //     1. their connection is disconnected, or
    //     2. the signal is destroyed.
    //
    // =========================================================================

    void emit(Args... args)
    {
        registry_.invoke(std::forward<Args>(args)...);
    }


    // =========================================================================
    // operator()
    // =========================================================================
    //
    // Convenience API.
    //
    //     signal.emit(42);
    //
    // and:
    //
    //     signal(42);
    //
    // have identical semantics.
    //
    // =========================================================================

    void operator()(Args... args)
    {
        emit(std::forward<Args>(args)...);
    }


    // =========================================================================
    // disconnect_all
    // =========================================================================
    //
    // Remove all callbacks from the signal.
    //
    // Every active callback object is destroyed by inplace_callback_registry
    // when its slot is cleared.
    //
    // Existing connection handles become stale because slot generations are
    // incremented.
    //
    // =========================================================================

    void disconnect_all() noexcept
    {
        registry_.clear();
    }


    // =========================================================================
    // Introspection
    // =========================================================================

    std::size_t connection_count() const noexcept
    {
        return registry_.size();
    }

    bool empty() const noexcept
    {
        return registry_.empty();
    }

    static constexpr std::size_t capacity() noexcept
    {
        return MaxConnection;
    }

    static constexpr std::size_t callback_storage_size() noexcept
    {
        return CallbackStorageSize;
    }

    static constexpr std::size_t callback_storage_alignment() noexcept
    {
        return CallbackStorageAlignment;
    }


private:
    static constexpr error translate_error(
        castle::callbacks::inplace_callback_registry_error error_code) noexcept
    {
        switch (error_code)
        {
            case castle::callbacks::inplace_callback_registry_error::ok:
            {
                return error::ok;
            }
            case castle::callbacks::inplace_callback_registry_error::full:
            {
                return error::full;
            }
            case castle::callbacks::inplace_callback_registry_error::invalid_callback:
            {
                return error::invalid_callback;
            }
            case castle::callbacks::inplace_callback_registry_error::invalid_subscription:
            {
                return error::invalid_connection;
            }
        }

        return error::invalid_connection;
    }

private:
    // =========================================================================
    // Registry is embedded directly inside the signal.
    //
    // No dynamic allocation occurs here.
    //
    // The memory layout is conceptually:
    //
    //     inplace_signal
    //     |
    //     +-- registry
    //          |
    //          +-- slot[0]
    //          |     +-- inplace_function
    //          |
    //          +-- slot[1]
    //          |     +-- inplace_function
    //          |
    //          +-- ...
    //
    // Therefore the maximum memory consumption is known at compile time.
    // =========================================================================

    registry_type registry_{};
};


// =============================================================================
// basic_inplace_scoped_connection
// =============================================================================
//
// RAII wrapper around basic_inplace_connection.
//
// This class owns the CONNECTION HANDLE, not the callback.
//
// The callback itself remains owned by inplace_callback_registry.
//
// Example:
//
//     {
//         auto connection = signal.connect(
//             [](int value)
//             {
//                 // ...
//             });
//
//         castle::events::basic_inplace_scoped_connection<
//             decltype(signal)::connection>
//             scoped(std::move(connection));
//
//         signal.emit(42);
//
//     } // callback automatically disconnected
//
//
//
// For embedded systems this gives a convenient deterministic lifetime:
//
//     scope
//       |
//       +-- signal
//       |
//       +-- scoped_connection
//
// The scoped connection should be declared AFTER the signal if both have the
// same scope and the connection is intended to be valid until scope exit.
//
// =============================================================================

template <typename Registry>
class basic_inplace_scoped_connection
{
public:
    using connection_type = basic_inplace_connection<Registry>;

    basic_inplace_scoped_connection() noexcept = default;

    explicit basic_inplace_scoped_connection(connection_type&& connection) noexcept
        : connection_(std::move(connection))
    {
    }

    ~basic_inplace_scoped_connection() noexcept
    {
        disconnect();
    }

    basic_inplace_scoped_connection(const basic_inplace_scoped_connection&) = delete;
    basic_inplace_scoped_connection& operator=(const basic_inplace_scoped_connection&) = delete;

    basic_inplace_scoped_connection(basic_inplace_scoped_connection&& other) noexcept
        : connection_(std::move(other.connection_))
    {
        other.connection_.reset();
    }

    basic_inplace_scoped_connection& operator=(basic_inplace_scoped_connection&& other) noexcept
    {
        if (this != &other)
        {
            disconnect();

            connection_ = std::move(other.connection_);

            other.connection_.reset();
        }

        return *this;
    }

    bool valid() const noexcept
    {
        return connection_.valid();
    }

    void disconnect() noexcept
    {
        if (connection_.valid())
        {
            (void)connection_.disconnect();
        }
    }

    connection_type release() noexcept
    {
        connection_type result =std::move(connection_);

        connection_.reset();

        return result;
    }

private:
    connection_type connection_{};
};


// =============================================================================
// inplace_scoped_signal_connection
// =============================================================================
// Sample:
//
//     inplace_scoped_signal_connection<
//         8,
//         void(int),
//         32>
//
// The latter would require a parameter pack followed by ordinary template
// parameters, which is not valid for this alias template.
//
// Keeping Signature also makes this alias consistent with inplace_signal.
//
// =============================================================================

template <
    std::size_t MaxConnection,
    typename Signature,
    std::size_t CallbackStorageSize = 64,
    std::size_t CallbackStorageAlignment = alignof(std::max_align_t)>
using inplace_scoped_signal_connection =
    basic_inplace_scoped_connection<
        typename inplace_signal<
            MaxConnection,
            Signature,
            CallbackStorageSize,
            CallbackStorageAlignment>::connection>;


// =============================================================================
// Free connect helper
// =============================================================================
//
// Same principle as the scoped connection alias:
//
// DO NOT put Args... before CallbackStorageSize.
//
// Instead, use Signature as a single template parameter.
//
// Example:
//
//     inplace_signal<8, void(int), 32> signal;
//
//     auto connection =
//         castle::events::connect(
//             signal,
//             [](int value)
//             {
//                 // ...
//             });
//
// The canonical API remains:
//
//     signal.connect(...);
//
// This helper is only syntactic sugar.
//
// =============================================================================

template <
    std::size_t MaxConnection,
    typename Signature,
    std::size_t CallbackStorageSize,
    std::size_t CallbackStorageAlignment,
    typename Callable>
auto connect(
    inplace_signal<MaxConnection, Signature, CallbackStorageSize, CallbackStorageAlignment>& signal,
    Callable&& callable,
    inplace_signal_error* out_error = nullptr)
    -> typename inplace_signal<MaxConnection, Signature, CallbackStorageSize, CallbackStorageAlignment>::connection
{
    return signal.connect(std::forward<Callable>(callable), out_error);
}

} // namespace events
} // namespace castle
