/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#include "gnss_diagnostic/antenna_open_monitor.h"

namespace gnss_diagnostic
{

antenna_open_monitor::antenna_open_monitor(dtc_config       cfg,
                                           i_dtc_service&   dtc_svc,
                                           const i_clock&   clk)
    : dtc_monitor_base(cfg, dtc_svc, clk)
{
}

void antenna_open_monitor::on_antenna(const antenna_event& ev)
{
    last_status_ = ev.status;
}

bool antenna_open_monitor::condition_currently_failing(
    std::chrono::steady_clock::time_point /*now*/) const
{
    return last_status_ == antenna_status::open_circuit;
}

} // namespace gnss_diagnostic
