/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_ANTENNA_SHORT_MONITOR_H
#define GNSS_DIAGNOSTIC_ANTENNA_SHORT_MONITOR_H

#pragma once

#include "dtc_monitor_base.h"

namespace gnss_diagnostic
{

class antenna_short_monitor final : public dtc_monitor_base
{
public:
    antenna_short_monitor(dtc_config       cfg,
                          i_dtc_service&   dtc_svc,
                          const i_clock&   clk);

    void on_antenna(const antenna_event& ev) override;

protected:
    bool condition_currently_failing(
        std::chrono::steady_clock::time_point now) const override;

private:
    antenna_status last_status_ = antenna_status::unknown;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_ANTENNA_SHORT_MONITOR_H
