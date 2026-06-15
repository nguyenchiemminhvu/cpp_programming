/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#include "gnss_diagnostic/pvt_timeout_monitor.h"

namespace gnss_diagnostic
{

pvt_timeout_monitor::pvt_timeout_monitor(dtc_config                 cfg,
                                         i_dtc_service&             dtc_svc,
                                         const i_clock&             clk,
                                         std::chrono::milliseconds  freshness_threshold)
    : dtc_monitor_base(cfg, dtc_svc, clk)
    , freshness_threshold_(freshness_threshold)
{
}

void pvt_timeout_monitor::on_pvt(const pvt_event& ev)
{
    last_pvt_ts_   = ev.ts;
    ever_received_ = true;
}

bool pvt_timeout_monitor::condition_currently_failing(
    std::chrono::steady_clock::time_point now) const
{
    if (!ever_received_)
    {
        // Until the first PVT arrives we treat the stream as stale.
        return true;
    }
    return (now - last_pvt_ts_) > freshness_threshold_;
}

} // namespace gnss_diagnostic
