/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#include "gnss_diagnostic/antenna_short_monitor.h"

namespace gnss_diagnostic
{

antenna_short_monitor::antenna_short_monitor(dtc_config       cfg,
                                             i_dtc_service&   dtc_svc,
                                             const i_clock&   clk)
    : dtc_monitor_base(cfg, dtc_svc, clk)
{
}

void antenna_short_monitor::on_antenna(const antenna_event& ev)
{
    last_status_ = ev.status;
}

bool antenna_short_monitor::condition_currently_failing(
    std::chrono::steady_clock::time_point /*now*/) const
{
    return last_status_ == antenna_status::short_to_gnd;
}

} // namespace gnss_diagnostic
