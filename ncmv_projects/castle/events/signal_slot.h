#pragma once

#include "../callbacks/callback_registry.h"

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace castle
{
namespace events
{

// =============================================================================
// signal_error
// =============================================================================
//
// Error domain of signal/slot.
//
// Do not expose callback_registry_error here. callback_registry is an
// implementation primitive; signal is a higher-level event abstraction.
//
// This keeps the public API independent from the internal callback registry.
// =============================================================================

enum class signal_error : std::uint8_t
{
    ok = 0,
    full,
    invalid_slot,
    invalid_connection
};


// =============================================================================
// basic_connection
// =============================================================================
//
// A lightweight, non-owning connection handle.
//
// Important lifetime rule:
//
// A connection MUST NOT outlive the signal from which it was created.
//
// The connection contains a pointer to the underlying registry. Therefore,
// destroying the signal while keeping a live connection and subsequently
// calling disconnect() on that connection is invalid.
//
// This is intentional.
//
// For an embedded library, we prefer this explicit lifetime contract over
// introducing shared ownership / reference counting merely to make an
// accidentally misused connection safe.
//
// Generation is used by callback_registry to prevent:
//
//     old connection -> slot A
//     unsubscribe
//     new connection -> slot B using the same slot index
//
// from allowing the old connection to remove the new subscription.
//
// =============================================================================

template <typename Registry>
class basic_connection
{
public:
    constexpr basic_connection() noexcept = default;

    constexpr bool valid() const noexcept
    {
        return owner_ != nullptr && valid_;
    }

    // Disconnect this connection.
    //
    // The operation is idempotent from the caller's point of view:
    //
    //     connection.disconnect();  // first call -> ok
    //     connection.disconnect();  // second call -> invalid_connection
    //
    // The handle is reset after the first attempt, regardless of the result.
    //
    signal_error disconnect() noexcept
    {
        if (!valid())
        {
            return signal_error::invalid_connection;
        }

        const auto result = owner_->unsubscribe_slot(index_, generation_);

        reset();

        return translate_error(result);
    }

    // Explicitly forget the handle without disconnecting the slot.
    //
    // This is rarely needed. It is provided mainly for low-level integration
    // with code that deliberately transfers connection responsibility.
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
    using registry_error = castle::callbacks::callback_registry_error;
    using registry_base = castle::callbacks::i_unsubscribable;

    friend Registry;

    constexpr basic_connection(registry_base* owner, std::size_t index, std::uint32_t generation) noexcept
        : owner_(owner)
        , index_(index)
        , generation_(generation)
        , valid_(true)
    {
    }

    static constexpr signal_error translate_error(registry_error error) noexcept
    {
        switch (error)
        {
            case registry_error::ok:
            {
                return signal_error::ok;
            }
            case registry_error::full:
            {
                return signal_error::full;
            }
            case registry_error::invalid_callback:
            {
                return signal_error::invalid_slot;
            }
            case registry_error::invalid_subscription:
            {
                return signal_error::invalid_connection;
            }
        }

        return signal_error::invalid_connection;
    }

private:
    registry_base* owner_ = nullptr;
    std::size_t index_ = 0U;
    std::uint32_t generation_ = 0U;
    bool valid_ = false;
};


// =============================================================================
// signal
// =============================================================================
//
// Qt-inspired signal/slot facade.
//
// This is deliberately NOT a Qt clone.
//
// Semantic model:
//
//     signal<MaxConnection, void(Args...)>
//
// owns a fixed-capacity callback registry:
//
//     signal
//       |
//       +-- callback_registry<MaxConnection, void(Args...)>
//
// The signal does NOT own slot objects.
//
// Example:
//
//     struct Controller
//     {
//         void on_value(int value)
//         {
//             // ...
//         }
//     };
//
//     Controller controller;
//
//     castle::callbacks::function_ct_m<&Controller::on_value>
//         slot(controller);
//
//     castle::events::signal<4, void(int)> value_changed;
//
//     auto connection = value_changed.connect(&slot);
//
//     value_changed.emit(42);
//
//     connection.disconnect();
//
//
//
// Lifetime:
//
//     Controller
//         |
//         +-- slot
//
//     signal
//         |
//         +-- registry
//
//     connection
//         |
//         +-- non-owning reference to registry
//
// The caller must ensure that both the signal and slot outlive their
// corresponding connection usage.
//
// =============================================================================

template <std::size_t MaxConnection, typename Signature>
class signal;


// =============================================================================
// signal<MaxConnection, void(Args...)>
// =============================================================================

template <std::size_t MaxConnection, typename... Args>
class signal<MaxConnection, void(Args...)> final
{
private:
    using registry_type =
        castle::callbacks::callback_registry<
            MaxConnection,
            void(Args...)>;

public:
    using slot_type = castle::callbacks::i_function<Args...>;

    using connection = basic_connection<registry_type>;

    using error = signal_error;

    signal() = default;

    ~signal() = default;

    // A signal has identity.
    //
    // A connection contains a reference to this signal's registry, so moving
    // the registry would invalidate all outstanding connections.
    signal(const signal&) = delete;
    signal& operator=(const signal&) = delete;

    signal(signal&&) = delete;
    signal& operator=(signal&&) = delete;


    // =========================================================================
    // connect
    // =========================================================================
    //
    // Register a non-owning slot.
    //
    // No allocation is performed.
    //
    // The callback object is owned by the caller.
    //
    // Example:
    //
    //     function_ct_m<&Handler::on_event> slot(handler);
    //
    //     auto c = sig.connect(&slot);
    //
    // =========================================================================

    connection connect(slot_type* slot, error* out_error = nullptr) noexcept
    {
        if (slot == nullptr)
        {
            if (out_error != nullptr)
            {
                *out_error = error::invalid_argument;
            }
            return connection{};
        }

        using registry_error = castle::callbacks::callback_registry_error;
        registry_error registry_result = registry_error::ok;
        const auto registry_connection = registry_.subscribe(slot, &registry_result);

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
    // emit
    // =========================================================================
    //
    // Invoke all currently connected slots in registration order.
    //
    // Example:
    //
    //     sig.emit(42);
    //
    // The underlying callback_registry performs the actual dispatch.
    //
    // Reentrancy/mutation rule:
    //
    //     Calling emit() recursively is allowed.
    //
    //     connect(), disconnect() or disconnect_all() while the registry is
    //     being iterated should NOT be used.
    //
    // This restriction is intentional: it avoids maintaining a snapshot,
    // deferred-operation queue, or additional bookkeeping in the hot path.
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
    // Convenience syntax:
    //
    //     sig.emit(42);
    //
    // is equivalent to:
    //
    //     sig(42);
    //
    // Keeping operator() makes signal usable as a generic callable.
    // =========================================================================

    void operator()(Args... args)
    {
        emit(std::forward<Args>(args)...);
    }


    // =========================================================================
    // disconnect_all
    // =========================================================================
    //
    // Remove all currently connected slots.
    //
    // Existing connection handles become stale because callback_registry
    // increments the generation of every active slot.
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


private:
    static constexpr error translate_error(castle::callbacks::callback_registry_error error_code) noexcept
    {
        switch (error_code)
        {
            case castle::callbacks::callback_registry_error::ok:
            {
                return error::ok;
            }
            case castle::callbacks::callback_registry_error::full:
            {
                return error::full;
            }
            case castle::callbacks::callback_registry_error::invalid_callback:
            {
                return error::invalid_slot;
            }
            case castle::callbacks::callback_registry_error::invalid_subscription:
            {
                return error::invalid_connection;
            }
        }

        return error::invalid_connection;
    }

private:
    // The registry is embedded directly.
    //
    // Therefore:
    //
    //     signal memory =
    //         callback_registry memory
    //
    // No heap allocation exists anywhere in the signal.
    registry_type registry_{};
};


// =============================================================================
// scoped_connection
// =============================================================================
//
// RAII connection.
//
// The important design decision here is:
//
//     scoped_connection takes ownership of the CONNECTION HANDLE,
//     not ownership of the SLOT.
//
// Example:
//
//     {
//         auto connection = signal.connect(&slot);
//
//         scoped_connection scoped(
//             std::move(connection));
//
//         signal.emit(42);
//
//     } // scoped destructor disconnects automatically
//
//
//
// A scoped_connection is:
//     - non-copyable
//     - movable
//     - fixed-size
//     - heap-free
//
// =============================================================================

template <typename Registry>
class basic_scoped_connection
{
public:
    using connection_type = basic_connection<Registry>;

    basic_scoped_connection() noexcept = default;

    explicit basic_scoped_connection(connection_type&& connection) noexcept
        : connection_(std::move(connection))
    {
    }

    ~basic_scoped_connection() noexcept
    {
        disconnect();
    }

    basic_scoped_connection(const basic_scoped_connection&) = delete;
    basic_scoped_connection& operator=(const basic_scoped_connection&) = delete;

    basic_scoped_connection(basic_scoped_connection&& other) noexcept
        : connection_(std::move(other.connection_))
    {
        other.connection_.reset();
    }

    basic_scoped_connection& operator=(basic_scoped_connection&& other) noexcept
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

    // Explicitly disconnect before destruction.
    void disconnect() noexcept
    {
        if (connection_.valid())
        {
            (void)connection_.disconnect();
        }
    }

    // Give the raw connection handle back to the caller without disconnecting.
    //
    // After this call, scoped_connection no longer owns the connection.
    connection_type release() noexcept
    {
        connection_type result = std::move(connection_);
        connection_.reset();
        return result;
    }

private:
    connection_type connection_{};
};


// =============================================================================
// Convenience aliases
// =============================================================================

template <std::size_t MaxConnection, typename... Args>
using scoped_signal_connection = basic_scoped_connection<typename signal<MaxConnection, void(Args...)>::connection>;


// =============================================================================
// connect helper
// =============================================================================
//
// Optional convenience API:
//
//     auto c = castle::events::connect(sig, &slot);
//
// This is intentionally just syntax sugar.
//
// The canonical API remains:
//
//     sig.connect(&slot);
//
// =============================================================================

template <std::size_t MaxConnection, typename... Args>
auto connect(
    signal<MaxConnection, void(Args...)>& sig,
    castle::callbacks::i_function<Args...>* slot,
    signal_error* out_error = nullptr) noexcept
    -> typename signal<MaxConnection, void(Args...)>::connection
{
    return sig.connect(slot, out_error);
}

} // namespace events
} // namespace castle
