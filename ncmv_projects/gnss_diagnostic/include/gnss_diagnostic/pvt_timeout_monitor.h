/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_PVT_TIMEOUT_MONITOR_H
#define GNSS_DIAGNOSTIC_PVT_TIMEOUT_MONITOR_H

#pragma once

#include "dtc_monitor_base.h"

namespace gnss_diagnostic
{

/*
 * Monitor that detects a stalled 10 Hz PVT stream.
 *
 * "Failing" is defined as: no PVT received within freshness_threshold.
 * The base class then enforces:
 *   - cfg.trigger_duration of continuous failing -> set DTC
 *   - cfg.clear_duration   of continuous healthy -> clear DTC
 */
class pvt_timeout_monitor final : public dtc_monitor_base
{
public:
    pvt_timeout_monitor(dtc_config                 cfg,
                        i_dtc_service&             dtc_svc,
                        const i_clock&             clk,
                        std::chrono::milliseconds  freshness_threshold);

    void on_pvt(const pvt_event& ev) override;

protected:
    bool condition_currently_failing(
        std::chrono::steady_clock::time_point now) const override;

private:
    std::chrono::milliseconds              freshness_threshold_;
    std::chrono::steady_clock::time_point  last_pvt_ts_{};
    bool                                   ever_received_ = false;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_PVT_TIMEOUT_MONITOR_H
