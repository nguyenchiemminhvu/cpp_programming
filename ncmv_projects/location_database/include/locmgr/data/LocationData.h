/**
 * @file LocationData.h
 * @brief Plain-data structs for every data category managed by the Location
 *        Manager Database: pvt_data, satellite_data, imu_data, and dr_data.
 *
 * Design rationale:
 *  - Each struct is a pure value type (no behaviour), keeping data separated
 *    from logic (SRP).
 *  - Fields map 1-to-1 to the field_id enumerators defined in FieldDefs.h.
 *  - Aggregated satellite data (averages/counts) is stored here; per-satellite
 *    detail lives in satellite_info which is held in a separate container.
 *  - All floating-point fields are double to avoid precision loss on embedded
 *    targets that promote floats anyway.
 *  - Timestamps are int64_t (ms since Unix epoch) - avoids platform time_t
 *    width issues.
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace locmgr {
namespace db {

// ---------------------------------------------------------------------------
// satellite_info - per-satellite detail record (not individually field-tracked)
// ---------------------------------------------------------------------------
struct satellite_info
{
    uint8_t  sv_id           { 0 };     ///< satellite vehicle ID
    uint8_t  gnss_id         { 0 };     ///< 0=GPS,1=SBAS,2=Galileo,3=BeiDou,5=QZSS,6=GLONASS
    int8_t   elevation       { -1 };    ///< degrees above horizon (-1 = unknown)
    int16_t  azimuth         { -1 };    ///< degrees from true north (-1 = unknown)
    float    cno             { 0.0f };  ///< carrier-to-noise density (dBHz)
    bool     used_in_fix     { false };
    bool     ephemeris_valid { false };
    bool     alm_valid       { false };
    bool     health_ok       { true  };
};

// ---------------------------------------------------------------------------
// pvt_data - Position / Velocity / Time solution
// ---------------------------------------------------------------------------
struct pvt_data
{
    // ---- Position ----------------------------------------------------------
    double   latitude        { 0.0 };   ///< degrees, WGS-84, negative = South
    double   longitude       { 0.0 };   ///< degrees, WGS-84, negative = West
    double   altitude_msl    { 0.0 };   ///< metres above MSL
    double   altitude_hae    { 0.0 };   ///< metres above WGS-84 ellipsoid

    // ---- Velocity / Heading ------------------------------------------------
    double   heading_motion  { 0.0 };   ///< heading of motion  (0-360 deg)
    double   heading_vehicle { 0.0 };   ///< heading of vehicle body (0-360 deg)
    double   speed           { 0.0 };   ///< ground speed (m/s)

    // ---- Accuracy estimates ------------------------------------------------
    double   horizontal_acc  { 9999.0 };///< horizontal accuracy (metres, 1-sigma)
    double   vertical_acc    { 9999.0 };
    double   speed_acc       { 9999.0 };
    double   heading_acc     { 360.0 };

    // ---- Time --------------------------------------------------------------
    int64_t  timestamp_utc_ms { 0 };    ///< ms since 1970-01-01 00:00:00 UTC

    // ---- Fix quality -------------------------------------------------------
    uint8_t  fix_type        { 0 };     ///< 0=none,2=2D,3=3D,4=GNSS+DR,5=time
    uint8_t  fix_flags       { 0 };     ///< bit0=gnss_fix_ok,bit1=diff_soln
    uint8_t  num_sv_used     { 0 };     ///< SVs contributing to solution

    // ---- Dilution of precision ---------------------------------------------
    float    pdop            { 99.0f };
    float    hdop            { 99.0f };
    float    vdop            { 99.0f };

    // ---- Helpers -----------------------------------------------------------
    bool is_valid() const noexcept { return (fix_type >= 2); }
};

// ---------------------------------------------------------------------------
// satellite_data - per-epoch aggregated satellite metrics
// ---------------------------------------------------------------------------
struct satellite_data
{
    uint8_t  total_count          { 0 };    ///< satellites in view
    uint8_t  used_in_fix          { 0 };    ///< satellites contributing to fix
    float    avg_cno              { 0.0f }; ///< mean C/N0 of tracked sats (dBHz)
    float    max_cno              { 0.0f }; ///< best C/N0 in current epoch
    uint8_t  constellation_mask   { 0 };    ///< bit0=GPS,bit1=GLONASS,bit2=Galileo,bit3=BeiDou

    /// Detailed per-satellite records - not individually field-indexed
    std::vector<satellite_info> satellites;

    void clear() noexcept {
        total_count = used_in_fix = 0;
        avg_cno = max_cno = 0.0f;
        constellation_mask = 0;
        satellites.clear();
    }
};

// ---------------------------------------------------------------------------
// imu_data - Inertial Measurement Unit (from GNSS chip internal sensor)
// ---------------------------------------------------------------------------
struct imu_data
{
    // ---- Accelerometer (m/s^2) ---------------------------------------------
    double   accel_x         { 0.0 };
    double   accel_y         { 0.0 };
    double   accel_z         { 0.0 };

    // ---- Gyroscope (deg/s) -------------------------------------------------
    double   gyro_x          { 0.0 };
    double   gyro_y          { 0.0 };
    double   gyro_z          { 0.0 };

    // ---- Housekeeping ------------------------------------------------------
    float    temperature     { 0.0f };  ///< chip temperature (degC)
    uint8_t  calib_status    { 0 };     ///< bit0=accel_calib,bit1=gyro_calib

    bool is_calibrated() const noexcept { return (calib_status & 0x03) == 0x03; }
};

// ---------------------------------------------------------------------------
// dr_data - Dead-Reckoning output
// ---------------------------------------------------------------------------
struct dr_data
{
    bool     position_valid  { false };
    double   speed           { 0.0 };   ///< m/s
    double   heading         { 0.0 };   ///< degrees (0-360)
};

} // namespace db
} // namespace locmgr
