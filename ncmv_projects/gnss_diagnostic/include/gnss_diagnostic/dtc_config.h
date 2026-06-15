/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_DTC_CONFIG_H
#define GNSS_DIAGNOSTIC_DTC_CONFIG_H

#pragma once

#include <chrono>
#include <cstdint>

namespace gnss_diagnostic
{

/*
 * Per-DTC debounce configuration.
 *
 * trigger_duration: how long the failing condition must persist before set_dtc().
 * clear_duration  : how long the healthy condition must persist before clear_dtc().
 *
 * Both durations are independent and externally provided, e.g. loaded from
 * the vehicle calibration (KL30) database at boot time.
 */
struct dtc_config
{
    std::uint32_t              dtc_code;
    std::chrono::milliseconds  trigger_duration;
    std::chrono::milliseconds  clear_duration;
};

/*
 * Recovery escalation timings.
 *
 * level1_timeout : how long a DTC must remain set before performing chip reset.
 * level2_timeout : how long after a chip reset before requesting platform reboot.
 */
struct recovery_config
{
    std::chrono::milliseconds level1_timeout;
    std::chrono::milliseconds level2_timeout;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_DTC_CONFIG_H
