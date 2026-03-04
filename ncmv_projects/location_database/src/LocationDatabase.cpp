/**
 * @file LocationDatabase.cpp
 * @brief Implementation of location_database.
 *
 * See LocationDatabase.h for architecture and SOLID notes.
 */

#include "locmgr/db/LocationDatabase.h"

#include <stdexcept>
#include <cstring>

namespace locmgr {
namespace db {

// ---------------------------------------------------------------------------
// Field-to-field_mask constants used by write helpers
// ---------------------------------------------------------------------------
namespace {

/// Mask covering every PVT field
const field_mask k_pvt_mask = make_field_mask({
    field_id::PVT_LATITUDE,
    field_id::PVT_LONGITUDE,
    field_id::PVT_ALTITUDE_MSL,
    field_id::PVT_ALTITUDE_HAE,
    field_id::PVT_HEADING_MOTION,
    field_id::PVT_HEADING_VEHICLE,
    field_id::PVT_SPEED,
    field_id::PVT_HORIZONTAL_ACC,
    field_id::PVT_VERTICAL_ACC,
    field_id::PVT_SPEED_ACC,
    field_id::PVT_HEADING_ACC,
    field_id::PVT_TIMESTAMP_UTC,
    field_id::PVT_FIX_TYPE,
    field_id::PVT_FIX_FLAGS,
    field_id::PVT_NUM_SV,
    field_id::PVT_PDOP,
    field_id::PVT_HDOP,
    field_id::PVT_VDOP,
});

/// Mask covering every aggregated satellite field
const field_mask k_sat_mask = make_field_mask({
    field_id::SAT_COUNT,
    field_id::SAT_USED_IN_FIX,
    field_id::SAT_AVG_CN0,
    field_id::SAT_MAX_CN0,
    field_id::SAT_CONSTELLATION_MASK,
});

/// Mask covering every IMU field
const field_mask k_imu_mask = make_field_mask({
    field_id::IMU_ACCEL_X,
    field_id::IMU_ACCEL_Y,
    field_id::IMU_ACCEL_Z,
    field_id::IMU_GYRO_X,
    field_id::IMU_GYRO_Y,
    field_id::IMU_GYRO_Z,
    field_id::IMU_TEMPERATURE,
    field_id::IMU_CALIBRATION_STATUS,
});

/// Mask covering every Dead-Reckoning field
const field_mask k_dr_mask = make_field_mask({
    field_id::DR_POSITION_VALID,
    field_id::DR_SPEED,
    field_id::DR_HEADING,
});

/// Sync the scalar cache from a pvt_data struct
void pvt_to_cache(double* cache, bool* valid, const pvt_data& p) noexcept
{
    auto set = [&](field_id id, double v) {
        const auto i = static_cast<std::size_t>(id);
        cache[i] = v; valid[i] = true;
    };
    set(field_id::PVT_LATITUDE,        p.latitude);
    set(field_id::PVT_LONGITUDE,       p.longitude);
    set(field_id::PVT_ALTITUDE_MSL,    p.altitude_msl);
    set(field_id::PVT_ALTITUDE_HAE,    p.altitude_hae);
    set(field_id::PVT_HEADING_MOTION,  p.heading_motion);
    set(field_id::PVT_HEADING_VEHICLE, p.heading_vehicle);
    set(field_id::PVT_SPEED,           p.speed);
    set(field_id::PVT_HORIZONTAL_ACC,  p.horizontal_acc);
    set(field_id::PVT_VERTICAL_ACC,    p.vertical_acc);
    set(field_id::PVT_SPEED_ACC,       p.speed_acc);
    set(field_id::PVT_HEADING_ACC,     p.heading_acc);
    set(field_id::PVT_TIMESTAMP_UTC,   static_cast<double>(p.timestamp_utc_ms));
    set(field_id::PVT_FIX_TYPE,        static_cast<double>(p.fix_type));
    set(field_id::PVT_FIX_FLAGS,       static_cast<double>(p.fix_flags));
    set(field_id::PVT_NUM_SV,          static_cast<double>(p.num_sv_used));
    set(field_id::PVT_PDOP,            static_cast<double>(p.pdop));
    set(field_id::PVT_HDOP,            static_cast<double>(p.hdop));
    set(field_id::PVT_VDOP,            static_cast<double>(p.vdop));
}

void sat_to_cache(double* cache, bool* valid, const satellite_data& s) noexcept
{
    auto set = [&](field_id id, double v) {
        const auto i = static_cast<std::size_t>(id);
        cache[i] = v; valid[i] = true;
    };
    set(field_id::SAT_COUNT,              static_cast<double>(s.total_count));
    set(field_id::SAT_USED_IN_FIX,        static_cast<double>(s.used_in_fix));
    set(field_id::SAT_AVG_CN0,            static_cast<double>(s.avg_cno));
    set(field_id::SAT_MAX_CN0,            static_cast<double>(s.max_cno));
    set(field_id::SAT_CONSTELLATION_MASK, static_cast<double>(s.constellation_mask));
}

void imu_to_cache(double* cache, bool* valid, const imu_data& m) noexcept
{
    auto set = [&](field_id id, double v) {
        const auto i = static_cast<std::size_t>(id);
        cache[i] = v; valid[i] = true;
    };
    set(field_id::IMU_ACCEL_X,            m.accel_x);
    set(field_id::IMU_ACCEL_Y,            m.accel_y);
    set(field_id::IMU_ACCEL_Z,            m.accel_z);
    set(field_id::IMU_GYRO_X,             m.gyro_x);
    set(field_id::IMU_GYRO_Y,             m.gyro_y);
    set(field_id::IMU_GYRO_Z,             m.gyro_z);
    set(field_id::IMU_TEMPERATURE,        static_cast<double>(m.temperature));
    set(field_id::IMU_CALIBRATION_STATUS, static_cast<double>(m.calib_status));
}

void dr_to_cache(double* cache, bool* valid, const dr_data& d) noexcept
{
    auto set = [&](field_id id, double v) {
        const auto i = static_cast<std::size_t>(id);
        cache[i] = v; valid[i] = true;
    };
    set(field_id::DR_POSITION_VALID, static_cast<double>(d.position_valid));
    set(field_id::DR_SPEED,          d.speed);
    set(field_id::DR_HEADING,        d.heading);
}

} // anonymous namespace

// ===========================================================================
// Constructor / Destructor
// ===========================================================================

location_database::location_database()
{
    std::memset(m_field_cache, 0, sizeof(m_field_cache));
    std::memset(m_field_valid, 0, sizeof(m_field_valid));
}

location_database::~location_database()
{
    stop();
}

// ===========================================================================
// i_location_database_control
// ===========================================================================

bool location_database::start()
{
    std::lock_guard state_lock(m_state_mutex);

    if (m_state == database_state::running)
        return true; // already running - idempotent

    if (m_state == database_state::stopped)
        return false; // must reset() first

    m_state = database_state::running;
    return true;
}

void location_database::stop()
{
    std::lock_guard state_lock(m_state_mutex);

    if (m_state != database_state::running)
        return; // already stopped or uninitialised

    m_state = database_state::stopped;
}

void location_database::reset()
{
    std::lock_guard state_lock(m_state_mutex);

    {
        std::unique_lock data_lock(m_data_mutex);

        m_pvt  = pvt_data{};
        m_sat  = satellite_data{};
        m_imu  = imu_data{};
        m_dr   = dr_data{};

        std::memset(m_field_cache, 0, sizeof(m_field_cache));
        std::memset(m_field_valid, 0, sizeof(m_field_valid));

        m_epoch_mask.reset();
        m_avail_mask.reset();
        m_last_epoch.reset();
    }

    m_state = database_state::uninitialized;
}

std::string location_database::get_state_name() const
{
    std::lock_guard state_lock(m_state_mutex);
    return database_state_name(m_state);
}

// ===========================================================================
// i_location_database_writer
// ===========================================================================

void location_database::write_pvt(const pvt_data& pvt)
{
    assert_running("write_pvt");

    std::unique_lock data_lock(m_data_mutex);
    m_pvt = pvt;
    pvt_to_cache(m_field_cache, m_field_valid, pvt);
    mark_fields(k_pvt_mask);
}

void location_database::write_satellite_data(const satellite_data& sat)
{
    assert_running("write_satellite_data");

    std::unique_lock data_lock(m_data_mutex);
    m_sat = sat;
    sat_to_cache(m_field_cache, m_field_valid, sat);
    mark_fields(k_sat_mask);
}

void location_database::write_imu_data(const imu_data& imu)
{
    assert_running("write_imu_data");

    std::unique_lock data_lock(m_data_mutex);
    m_imu = imu;
    imu_to_cache(m_field_cache, m_field_valid, imu);
    mark_fields(k_imu_mask);
}

void location_database::write_dr_data(const dr_data& dr)
{
    assert_running("write_dr_data");

    std::unique_lock data_lock(m_data_mutex);
    m_dr = dr;
    dr_to_cache(m_field_cache, m_field_valid, dr);
    mark_fields(k_dr_mask);
}

void location_database::write_field(field_id id, double value)
{
    assert_running("write_field");

    const auto idx = static_cast<std::size_t>(id);
    if (idx >= FIELD_COUNT)
        throw std::out_of_range("write_field: invalid field_id");

    std::unique_lock data_lock(m_data_mutex);
    m_field_cache[idx] = value;
    m_field_valid[idx] = true;

    field_mask single;
    single.set(idx);
    mark_fields(single);
}

void location_database::commit_epoch()
{
    assert_running("commit_epoch");

    field_mask snapshot;
    {
        std::unique_lock data_lock(m_data_mutex);
        snapshot     = m_epoch_mask;
        m_last_epoch = m_epoch_mask;
        m_epoch_mask.reset();
    }

    // Dispatch outside the data lock so readers inside callbacks are not
    // deadlocked.
    m_subsc_mgr.dispatch(snapshot);
}

// ===========================================================================
// i_location_database_reader
// ===========================================================================

pvt_data location_database::read_pvt() const
{
    std::shared_lock data_lock(m_data_mutex);
    return m_pvt;
}

satellite_data location_database::read_satellite_data() const
{
    std::shared_lock data_lock(m_data_mutex);
    return m_sat;
}

imu_data location_database::read_imu_data() const
{
    std::shared_lock data_lock(m_data_mutex);
    return m_imu;
}

dr_data location_database::read_dr_data() const
{
    std::shared_lock data_lock(m_data_mutex);
    return m_dr;
}

bool location_database::read_field(field_id id, double& value) const
{
    const auto idx = static_cast<std::size_t>(id);
    if (idx >= FIELD_COUNT)
        throw std::out_of_range("read_field: invalid field_id");

    std::shared_lock data_lock(m_data_mutex);
    if (!m_field_valid[idx])
        return false;

    value = m_field_cache[idx];
    return true;
}

uint64_t location_database::subscribe(i_field_update_listener* listener,
                                      const field_mask& mask)
{
    return m_subsc_mgr.subscribe(listener, mask);
}

void location_database::unsubscribe(uint64_t token)
{
    m_subsc_mgr.unsubscribe(token);
}

field_mask location_database::get_available_mask() const
{
    std::shared_lock data_lock(m_data_mutex);
    return m_avail_mask;
}

field_mask location_database::get_last_epoch_mask() const
{
    std::shared_lock data_lock(m_data_mutex);
    return m_last_epoch;
}

// ===========================================================================
// Private helpers
// ===========================================================================

void location_database::assert_running(const char* caller) const
{
    if (m_state != database_state::running)
        throw std::logic_error(std::string(caller) +
            ": database is not in running state (current: " +
            database_state_name(m_state) + ")");
}

void location_database::mark_fields(const field_mask& bits) noexcept
{
    // Called while m_data_mutex is already held (unique_lock)
    m_epoch_mask |= bits;
    m_avail_mask |= bits;
}

} // namespace db
} // namespace locmgr
