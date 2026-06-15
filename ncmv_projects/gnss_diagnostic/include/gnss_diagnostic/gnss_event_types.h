/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_GNSS_EVENT_TYPES_H
#define GNSS_DIAGNOSTIC_GNSS_EVENT_TYPES_H

#pragma once

#include <chrono>
#include <cstdint>

namespace gnss_diagnostic
{

enum class fix_type : std::uint8_t
{
    no_fix    = 0,
    fix_2d    = 1,
    fix_3d    = 2,
    dgnss     = 3,
    rtk_float = 4,
    rtk_fixed = 5,
};

struct pvt_event
{
    std::chrono::steady_clock::time_point ts;
    fix_type        fix;
    std::uint8_t    sat_count;
    double          latitude_deg;
    double          longitude_deg;
    double          altitude_m;
};

enum class antenna_status : std::uint8_t
{
    unknown        = 0,
    ok             = 1,
    open_circuit   = 2,
    short_to_gnd   = 3,
};

struct antenna_event
{
    std::chrono::steady_clock::time_point ts;
    antenna_status status;
};

enum class power_mode : std::uint8_t
{
    active     = 0,
    low_power  = 1,
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_GNSS_EVENT_TYPES_H
