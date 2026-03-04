/**
 * @file LocationDatabase.h
 * @brief Concrete implementation of i_location_database.
 *
 * Architecture overview:
 * +----------------------------------------------------------+
 * |                   location_database                      |
 * |                                                          |
 * |  m_pvt          : pvt_data       ───────────────────┐   |
 * |  m_sat          : satellite_data                    |   |
 * |  m_imu          : imu_data       <- data store      |   |
 * |  m_dr           : dr_data                           |   |
 * |                                                     |   |
 * |  m_epoch_mask   : field_mask     <- dirty bits/epoch|   |
 * |  m_avail_mask   : field_mask     <- ever-written    |   |
 * |                                                     |   |
 * |  m_subsc_mgr    : subscription_manager  ────────────┘   |
 * |                                                          |
 * |  m_state        : database_state (state machine)        |
 * |  m_data_mutex   : shared_mutex (reader-writer lock)     |
 * +----------------------------------------------------------+
 *
 * Lifecycle state machine:
 *
 *   [uninitialized] --start()--> [running] --stop()--> [stopped]
 *        ^                                                  |
 *        +-------------------- reset() -------------------+
 *
 * Thread-safety model:
 *   - All write operations (write_pvt, etc.) acquire a unique_lock on
 *     m_data_mutex and record bits in m_epoch_mask.
 *   - commit_epoch() takes unique_lock, snapshots the epoch mask, clears it,
 *     then releases the lock before calling dispatch() so reads can proceed
 *     inside subscriber callbacks.
 *   - All read operations acquire a shared_lock on m_data_mutex.
 *   - subscribe() / unsubscribe() are delegated to subscription_manager which
 *     has its own internal shared_mutex.
 *
 * SOLID notes:
 *   SRP - storage + dispatch logic; lifecycle delegated to state machine.
 *   OCP - extend field set by adding to field_id/structs; no code change here.
 *   LSP - fully satisfies all three base interfaces.
 *   ISP - callers receive i_location_database_writer or i_location_database_reader
 *         pointers so they never see the wrong surface.
 *   DIP - subscription_manager is used through its concrete type but is owned
 *         privately; external callers depend only on i_location_database.
 */

#pragma once

#include "locmgr/db/ILocationDatabase.h"
#include "locmgr/db/SubscriptionManager.h"

#include <mutex>
#include <shared_mutex>
#include <string>
#include <stdexcept>

namespace locmgr {
namespace db {

// ---------------------------------------------------------------------------
// database_state - explicit lifecycle state machine
// ---------------------------------------------------------------------------
enum class database_state
{
    uninitialized,
    running,
    stopped
};

inline std::string database_state_name(database_state s) noexcept
{
    switch (s)
    {
        case database_state::uninitialized: return "uninitialized";
        case database_state::running:       return "running";
        case database_state::stopped:       return "stopped";
    }
    return "unknown";
}

// ---------------------------------------------------------------------------
// location_database
// ---------------------------------------------------------------------------
class location_database final : public i_location_database
{
public:
    location_database();
    ~location_database() override;

    // Non-copyable, non-movable
    location_database(const location_database&)            = delete;
    location_database& operator=(const location_database&) = delete;
    location_database(location_database&&)                 = delete;
    location_database& operator=(location_database&&)      = delete;

    // -----------------------------------------------------------------------
    // i_location_database_control
    // -----------------------------------------------------------------------
    bool        start()              override;
    void        stop()               override;
    void        reset()              override;
    std::string get_state_name()     const override;

    // -----------------------------------------------------------------------
    // i_location_database_writer
    // -----------------------------------------------------------------------
    void write_pvt(const pvt_data& pvt)               override;
    void write_satellite_data(const satellite_data& s) override;
    void write_imu_data(const imu_data& imu)           override;
    void write_dr_data(const dr_data& dr)              override;
    void write_field(field_id id, double value)        override;
    void commit_epoch()                                override;

    // -----------------------------------------------------------------------
    // i_location_database_reader
    // -----------------------------------------------------------------------
    pvt_data        read_pvt()              const override;
    satellite_data  read_satellite_data()   const override;
    imu_data        read_imu_data()         const override;
    dr_data         read_dr_data()          const override;
    bool            read_field(field_id id, double& value) const override;

    uint64_t        subscribe(i_field_update_listener* listener,
                              const field_mask& mask)      override;
    void            unsubscribe(uint64_t token)             override;

    field_mask      get_available_mask()    const override;
    field_mask      get_last_epoch_mask()   const override;

private:
    // -----------------------------------------------------------------------
    // Internal helpers
    // -----------------------------------------------------------------------
    void assert_running(const char* caller) const;
    void mark_fields(const field_mask& bits) noexcept;

    // -----------------------------------------------------------------------
    // State
    // -----------------------------------------------------------------------
    mutable std::shared_mutex   m_data_mutex;    ///< reader-writer lock for data
    mutable std::mutex          m_state_mutex;   ///< serialises lifecycle transitions

    database_state              m_state { database_state::uninitialized };

    // -----------------------------------------------------------------------
    // Data store
    // -----------------------------------------------------------------------
    pvt_data        m_pvt;
    satellite_data  m_sat;
    imu_data        m_imu;
    dr_data         m_dr;

    // Per-field scalar cache for the write_field / read_field fine-grained API.
    // Indexed by static_cast<std::size_t>(field_id).
    double  m_field_cache[FIELD_COUNT] {};
    bool    m_field_valid[FIELD_COUNT] {};

    // -----------------------------------------------------------------------
    // Masks
    // -----------------------------------------------------------------------
    field_mask   m_epoch_mask;   ///< fields written this epoch (reset on commit)
    field_mask   m_avail_mask;   ///< fields written at least once since start()
    field_mask   m_last_epoch;   ///< snapshot of epoch_mask at last commit_epoch()

    // -----------------------------------------------------------------------
    // Subscription
    // -----------------------------------------------------------------------
    subscription_manager m_subsc_mgr;
};

} // namespace db
} // namespace locmgr
