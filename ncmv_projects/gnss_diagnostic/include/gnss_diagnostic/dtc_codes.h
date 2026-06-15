/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_DTC_CODES_H
#define GNSS_DIAGNOSTIC_DTC_CODES_H

#pragma once

#include <cstdint>

namespace gnss_diagnostic
{

/*
 * Vehicle-defined DTC codes for GNSS subsystem.
 * The numeric values are placeholders; align them with the OEM DTC catalog.
 */
namespace dtc_codes
{
    constexpr std::uint32_t pvt_stream_timeout    = 0xC10101u;
    constexpr std::uint32_t antenna_short_to_gnd  = 0xC10102u;
    constexpr std::uint32_t antenna_open_circuit  = 0xC10103u;
}

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_DTC_CODES_H
