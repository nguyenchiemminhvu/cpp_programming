/**
 * @file ILocationDatabase.h
 * @brief Abstract interfaces for the Location Manager Database.
 *
 * The interface is split following the Interface Segregation Principle (ISP):
 *
 *   i_location_database_writer  - used exclusively by the GNSS Parser (write side)
 *   i_location_database_reader  - used exclusively by Location Features (read side)
 *   i_location_database_control - used by the service lifecycle manager
 *   i_location_database         - composes all three; implemented by the concrete DB
 *
 * Subscribers implement i_field_update_listener and register themselves through
 * i_location_database_reader::subscribe().
 *
 * SOLID notes:
 *   SRP - each interface has exactly one reason to change.
 *   OCP - new fields are added to field_id / data structs; interface unchanged.
 *   LSP - all concrete implementations must honour these contracts fully.
 *   ISP - callers depend only on the narrowest interface they need.
 *   DIP - writers, readers, and the lifecycle owner depend on abstractions.
 */

#pragma once

#include "locmgr/data/FieldDefs.h"
#include "locmgr/data/LocationData.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace locmgr {
namespace db {

// ---------------------------------------------------------------------------
// i_field_update_listener - implemented by any component that subscribes to DB
// ---------------------------------------------------------------------------

/**
 * @brief Callback interface for field-update notifications.
 *
 * A subscriber registers a field_mask listing the fields it cares about.
 * The database fires on_fields_updated() exactly once per write epoch in which
 * ALL of the subscriber's requested fields were updated.
 *
 * The updated_mask argument carries which fields were actually freshly written
 * this epoch (may be a superset of the subscriber's own mask).
 */
class i_field_update_listener
{
public:
    virtual ~i_field_update_listener() = default;

    /**
     * @brief Called when all fields in the subscription mask are fresh.
     * @param updated_mask  Bitmask of every field updated this epoch.
     *
     * @note  This callback is invoked on the database's internal dispatch
     *        thread.  Implementations must be non-blocking; heavy work
     *        should be posted to their own thread.
     */
    virtual void on_fields_updated(const field_mask& updated_mask) = 0;
};

// ---------------------------------------------------------------------------
// i_location_database_writer - write-only surface (for the GNSS Parser)
// ---------------------------------------------------------------------------
class i_location_database_writer
{
public:
    virtual ~i_location_database_writer() = default;

    /**
     * @brief Write a complete PVT solution.
     *
     * Marks all PVT_* field_ids as updated.  If a subscriber's mask is
     * satisfied after this write, it will be notified at epoch-commit time.
     */
    virtual void write_pvt(const pvt_data& pvt) = 0;

    /**
     * @brief Write aggregated satellite data.
     *
     * Marks all SAT_* field_ids as updated.
     */
    virtual void write_satellite_data(const satellite_data& sat) = 0;

    /**
     * @brief Write IMU data from the GNSS chip's internal sensor.
     *
     * Marks all IMU_* field_ids as updated.
     */
    virtual void write_imu_data(const imu_data& imu) = 0;

    /**
     * @brief Write Dead-Reckoning output.
     *
     * Marks all DR_* field_ids as updated.
     */
    virtual void write_dr_data(const dr_data& dr) = 0;

    /**
     * @brief Write a single arbitrary field by ID.
     *
     * Provided for fine-grained updates when a full struct write is
     * unnecessary.  The value is encoded as a double; callers are
     * responsible for correct casting of integer/boolean fields.
     */
    virtual void write_field(field_id id, double value) = 0;

    /**
     * @brief Commit the current write epoch.
     *
     * After all writes for one GNSS message cycle are done the parser calls
     * commit_epoch().  The database then evaluates all subscribers and fires
     * on_fields_updated() for those whose masks are fully satisfied, then
     * resets the epoch-dirty mask.
     */
    virtual void commit_epoch() = 0;
};

// ---------------------------------------------------------------------------
// i_location_database_reader - read-only surface (for Location Features)
// ---------------------------------------------------------------------------
class i_location_database_reader
{
public:
    virtual ~i_location_database_reader() = default;

    // ---- Snapshot reads ----------------------------------------------------

    /**
     * @brief Read the latest full PVT snapshot.
     * @return A copy of the current pvt_data (thread-safe).
     */
    virtual pvt_data read_pvt() const = 0;

    /**
     * @brief Read the latest aggregated satellite data snapshot.
     */
    virtual satellite_data read_satellite_data() const = 0;

    /**
     * @brief Read the latest IMU data snapshot.
     */
    virtual imu_data read_imu_data() const = 0;

    /**
     * @brief Read the latest Dead-Reckoning data snapshot.
     */
    virtual dr_data read_dr_data() const = 0;

    /**
     * @brief Read a single field value by ID.
     *
     * @param id      The field to read.
     * @param value   Output parameter filled with the current value.
     * @return        true if the field has been written at least once,
     *                false if it is still in its default-constructed state.
     */
    virtual bool read_field(field_id id, double& value) const = 0;

    // ---- Subscription ------------------------------------------------------

    /**
     * @brief Register a listener that will be notified when all fields in
     *        @p mask have been updated within a single write epoch.
     *
     * @param listener  Non-owning pointer; the caller must keep it alive for
     *                  as long as the subscription is active.
     * @param mask      Set of field_ids the listener cares about.
     * @return          A subscription token (opaque uint64_t) that must be
     *                  passed to unsubscribe() to cancel.
     */
    virtual uint64_t subscribe(i_field_update_listener* listener,
                               const field_mask& mask) = 0;

    /**
     * @brief Cancel a previously registered subscription.
     * @param token  The value returned by subscribe().
     */
    virtual void unsubscribe(uint64_t token) = 0;

    // ---- Diagnostics -------------------------------------------------------

    /**
     * @brief Mask of fields that have been written at least once since the
     *        database was started.
     */
    virtual field_mask get_available_mask() const = 0;

    /**
     * @brief Mask of fields updated in the most recently committed epoch.
     */
    virtual field_mask get_last_epoch_mask() const = 0;
};

// ---------------------------------------------------------------------------
// i_location_database_control - lifecycle surface (for the service owner)
// ---------------------------------------------------------------------------
class i_location_database_control
{
public:
    virtual ~i_location_database_control() = default;

    /// Transition from uninitialized -> running
    virtual bool start() = 0;

    /// Transition from running -> stopped (idempotent)
    virtual void stop() = 0;

    /// Hard reset: clear all data and return to uninitialized
    virtual void reset() = 0;

    /// Human-readable state name for diagnostics
    virtual std::string get_state_name() const = 0;
};

// ---------------------------------------------------------------------------
// i_location_database - the full composed interface
// ---------------------------------------------------------------------------
class i_location_database
    : public i_location_database_writer
    , public i_location_database_reader
    , public i_location_database_control
{
public:
    virtual ~i_location_database() = default;
};

} // namespace db
} // namespace locmgr
