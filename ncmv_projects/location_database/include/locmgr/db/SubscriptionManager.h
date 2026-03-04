/**
 * @file SubscriptionManager.h
 * @brief Manages field-update subscriptions on behalf of the database.
 *
 * Responsibility (SRP):
 *   This class owns the subscriber registry and the logic for evaluating
 *   which subscribers to notify.  It is deliberately separate from the data
 *   storage so each class has one reason to change.
 *
 * Usage pattern:
 *   1. GNSS Parser writes fields -> database accumulates epoch dirty mask.
 *   2. Parser calls commit_epoch() -> database calls
 *      subscription_manager::dispatch(epoch_mask).
 *   3. dispatch() iterates registrations; if (reg.mask & epoch_mask) == reg.mask
 *      all requested fields are fresh -> listener->on_fields_updated() is called.
 *
 * Thread-safety:
 *   subscribe() and unsubscribe() acquire a write lock.
 *   dispatch() acquires a read lock so concurrent dispatches from multiple
 *   threads are safe (though the design uses a single parser thread).
 */

#pragma once

#include "locmgr/db/ILocationDatabase.h"

#include <atomic>
#include <cstdint>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace locmgr {
namespace db {

class subscription_manager final
{
public:
    subscription_manager() = default;
    ~subscription_manager() = default;

    // Non-copyable, non-movable (contains a mutex)
    subscription_manager(const subscription_manager&)            = delete;
    subscription_manager& operator=(const subscription_manager&) = delete;
    subscription_manager(subscription_manager&&)                 = delete;
    subscription_manager& operator=(subscription_manager&&)      = delete;

    // -----------------------------------------------------------------------
    // Registration
    // -----------------------------------------------------------------------

    /**
     * @brief Add a subscription.
     * @param listener  Non-owning pointer; caller guarantees lifetime.
     * @param mask      Fields the listener cares about.
     * @return          Opaque token; pass to unsubscribe() to cancel.
     */
    uint64_t subscribe(i_field_update_listener* listener, const field_mask& mask)
    {
        if (listener == nullptr)
            return k_invalid_token;

        const uint64_t token = m_next_token.fetch_add(1u, std::memory_order_relaxed);

        std::unique_lock lock(m_mutex);
        m_registry.emplace(token, registration{ listener, mask });
        return token;
    }

    /**
     * @brief Remove a subscription by token.
     *        No-op if the token is not found.
     */
    void unsubscribe(uint64_t token)
    {
        std::unique_lock lock(m_mutex);
        m_registry.erase(token);
    }

    // -----------------------------------------------------------------------
    // Dispatch
    // -----------------------------------------------------------------------

    /**
     * @brief Fire notifications for all subscribers whose masks are satisfied
     *        by @p epoch_mask.
     *
     * Called once per epoch after all writes are complete.
     * Listeners are invoked while the read lock is held; they MUST NOT call
     * subscribe() or unsubscribe() from within on_fields_updated() (doing so
     * would deadlock).  Post deferred work to a separate thread instead.
     *
     * @param epoch_mask  Fields written during this epoch.
     */
    void dispatch(const field_mask& epoch_mask)
    {
        std::shared_lock lock(m_mutex);
        for (auto& entry : m_registry)
        {
            // Subscriber's mask is satisfied iff every bit it requests is set
            auto& reg = entry.second;
            if ((reg.mask & epoch_mask) == reg.mask)
            {
                reg.listener->on_fields_updated(epoch_mask);
            }
        }
    }

    // -----------------------------------------------------------------------
    // Diagnostics
    // -----------------------------------------------------------------------

    std::size_t subscriber_count() const
    {
        std::shared_lock lock(m_mutex);
        return m_registry.size();
    }

    static constexpr uint64_t k_invalid_token = 0u;

private:
    struct registration
    {
        i_field_update_listener* listener { nullptr };
        field_mask               mask;
    };

    mutable std::shared_mutex                        m_mutex;
    std::unordered_map<uint64_t, registration>       m_registry;
    std::atomic<uint64_t>                            m_next_token { 1u };
};

} // namespace db
} // namespace locmgr
