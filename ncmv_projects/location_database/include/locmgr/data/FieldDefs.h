/**
 * @file FieldDefs.h
 * @brief Defines all data field identifiers and the field_mask used for
 *        subscription registration in the Location Manager Database.
 *
 * Design rationale:
 *  - A single enum class field_id maps every individual data field to one bit
 *    position in a std::bitset.
 *  - Subscribers register a field_mask (a bitset) describing which fields they
 *    care about.  The database fires a notification only when ALL bits in the
 *    subscriber's mask have been freshly updated in the same "epoch" (i.e.
 *    one write cycle), preventing partial-data callbacks.
 *  - Adding a new field requires only appending to field_id and bumping
 *    FIELD_COUNT – no existing code changes.
 *
 * SOLID notes:
 *  - SRP  : this file is solely responsible for the field vocabulary.
 *  - OCP  : extend by adding new enum values; no modification of consumer code.
 */

#pragma once

#include <bitset>
#include <cstddef>
#include <string>
#include <stdexcept>
#include <unordered_map>

namespace locmgr {
namespace db {

// ---------------------------------------------------------------------------
// field_id – one enumerator per logical data field
// ---------------------------------------------------------------------------
enum class field_id : std::size_t
{
    // ---- PVT (Position / Velocity / Time) ----------------------------------
    PVT_LATITUDE            = 0,
    PVT_LONGITUDE           = 1,
    PVT_ALTITUDE_MSL        = 2,   ///< altitude above mean sea level (metres)
    PVT_ALTITUDE_HAE        = 3,   ///< altitude above WGS-84 ellipsoid (metres)
    PVT_HEADING_MOTION      = 4,   ///< heading of motion (degrees, 0-360)
    PVT_HEADING_VEHICLE     = 5,   ///< heading of vehicle body (degrees)
    PVT_SPEED               = 6,   ///< ground speed (m/s)
    PVT_HORIZONTAL_ACC      = 7,   ///< horizontal accuracy estimate (metres)
    PVT_VERTICAL_ACC        = 8,   ///< vertical accuracy estimate (metres)
    PVT_SPEED_ACC           = 9,   ///< speed accuracy estimate (m/s)
    PVT_HEADING_ACC         = 10,  ///< heading accuracy estimate (degrees)
    PVT_TIMESTAMP_UTC       = 11,  ///< UTC epoch timestamp (ms)
    PVT_FIX_TYPE            = 12,  ///< 0=no fix,2=2D,3=3D,4=GNSS+DR
    PVT_FIX_FLAGS           = 13,  ///< bitfield: gnss_fix_ok, diff_soln
    PVT_NUM_SV              = 14,  ///< number of SVs used in navigation solution
    PVT_PDOP                = 15,  ///< position DOP
    PVT_HDOP                = 16,
    PVT_VDOP                = 17,

    // ---- Satellite data (per-epoch aggregated) ----------------------------
    SAT_COUNT               = 18,  ///< total satellites in view
    SAT_USED_IN_FIX         = 19,  ///< number of sats used in fix
    SAT_AVG_CN0             = 20,  ///< average C/N0 across tracked sats (dBHz)
    SAT_MAX_CN0             = 21,  ///< best C/N0 value in current epoch
    SAT_CONSTELLATION_MASK  = 22,  ///< bitfield: GPS|GLONASS|GALILEO|BEIDOU

    // ---- IMU data (from GNSS chip internal sensor) ------------------------
    IMU_ACCEL_X             = 23,  ///< acceleration X (m/s^2)
    IMU_ACCEL_Y             = 24,
    IMU_ACCEL_Z             = 25,
    IMU_GYRO_X              = 26,  ///< angular rate X (deg/s)
    IMU_GYRO_Y              = 27,
    IMU_GYRO_Z              = 28,
    IMU_TEMPERATURE         = 29,  ///< chip temperature (degC)
    IMU_CALIBRATION_STATUS  = 30,  ///< bitmask: accel_calib|gyro_calib

    // ---- Dead-Reckoning (DR) output ----------------------------------------
    DR_POSITION_VALID       = 31,
    DR_SPEED                = 32,
    DR_HEADING              = 33,

    // ---- Sentinel - must always be last ------------------------------------
    FIELD_COUNT             = 34
};

// ---------------------------------------------------------------------------
// field_mask – a bitset where bit N corresponds to field_id value N
// ---------------------------------------------------------------------------
static constexpr std::size_t FIELD_COUNT = static_cast<std::size_t>(field_id::FIELD_COUNT);

using field_mask = std::bitset<FIELD_COUNT>;

// ---------------------------------------------------------------------------
// Helper utilities
// ---------------------------------------------------------------------------

/// Set a single field bit in a mask
inline field_mask& set_field(field_mask& mask, field_id id) noexcept
{
    mask.set(static_cast<std::size_t>(id));
    return mask;
}

/// Clear a single field bit in a mask
inline field_mask& clear_field(field_mask& mask, field_id id) noexcept
{
    mask.reset(static_cast<std::size_t>(id));
    return mask;
}

/// Test whether a field bit is set
inline bool has_field(const field_mask& mask, field_id id) noexcept
{
    return mask.test(static_cast<std::size_t>(id));
}

/// Build a mask from an initializer list of field_ids (convenience factory)
inline field_mask make_field_mask(std::initializer_list<field_id> ids) noexcept
{
    field_mask m;
    for (auto id : ids)
        m.set(static_cast<std::size_t>(id));
    return m;
}

/// Human-readable name for a field_id (useful in logging / debug)
inline std::string field_name(field_id id) noexcept
{
    static const std::unordered_map<std::size_t, std::string> k_names {
        { 0,  "PVT_LATITUDE"           },
        { 1,  "PVT_LONGITUDE"          },
        { 2,  "PVT_ALTITUDE_MSL"       },
        { 3,  "PVT_ALTITUDE_HAE"       },
        { 4,  "PVT_HEADING_MOTION"     },
        { 5,  "PVT_HEADING_VEHICLE"    },
        { 6,  "PVT_SPEED"              },
        { 7,  "PVT_HORIZONTAL_ACC"     },
        { 8,  "PVT_VERTICAL_ACC"       },
        { 9,  "PVT_SPEED_ACC"          },
        { 10, "PVT_HEADING_ACC"        },
        { 11, "PVT_TIMESTAMP_UTC"      },
        { 12, "PVT_FIX_TYPE"           },
        { 13, "PVT_FIX_FLAGS"          },
        { 14, "PVT_NUM_SV"             },
        { 15, "PVT_PDOP"               },
        { 16, "PVT_HDOP"               },
        { 17, "PVT_VDOP"               },
        { 18, "SAT_COUNT"              },
        { 19, "SAT_USED_IN_FIX"        },
        { 20, "SAT_AVG_CN0"            },
        { 21, "SAT_MAX_CN0"            },
        { 22, "SAT_CONSTELLATION_MASK" },
        { 23, "IMU_ACCEL_X"            },
        { 24, "IMU_ACCEL_Y"            },
        { 25, "IMU_ACCEL_Z"            },
        { 26, "IMU_GYRO_X"             },
        { 27, "IMU_GYRO_Y"             },
        { 28, "IMU_GYRO_Z"             },
        { 29, "IMU_TEMPERATURE"        },
        { 30, "IMU_CALIBRATION_STATUS" },
        { 31, "DR_POSITION_VALID"      },
        { 32, "DR_SPEED"               },
        { 33, "DR_HEADING"             },
    };
    auto it = k_names.find(static_cast<std::size_t>(id));
    return (it != k_names.end()) ? it->second : "UNKNOWN_FIELD";
}

} // namespace db
} // namespace locmgr
